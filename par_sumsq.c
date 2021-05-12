/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * 
 * Ivan Samuel
 * CS 446
 * Spring 2021
 * Project 5 Pthreads
 * 
 * par_sumsq.c
 *
 * CS 446.646 Project 1 (Pthreads)
 *
 * Compile with --std=c99
 */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include libraries needed 
// These libraries were already given by sumsq.c
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// Include pthread library, needed for the use of pthread functions
// pthread functions are listed in "Notes for me"
#include <pthread.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create variables needed
// Also create any structures needed
// May need structs for thread queue

// aggregate variables
long sum = 0;
long odd = 0;
long min = INT_MAX;
long max = INT_MIN;
volatile bool done = false; // Changed type to volatile bool instead of regular bool
// Changing it to volatile will ensure that it happens before relationship among threads sharing that variable
// This is important for threads in the main function

// Data to be shared with threads - given from slides
// int sum; // Initialized and used elsewhere 

// Global Variables (if needed)
// Guess we do need one, set number of workers
#define WORKERS 1;

// Global variable for number of threads - given from slides
#define NUM_THREADS 10;

/*
// An array of threads to be joined upon
pthread_t workers[NUM_THREADS];

for (int i = 0; i < NUM_THREADS; i++) {
  pthread_join (workers[i], NULL);
}
*/ // Not sure if needed

// Needed mutex initializer
// Macro initializes the static mutex - can only be used for static values
// Have 1 so far, do I need more?
pthread_mutex_t lock_queue = PTHREAD_MUTEX_INITIALIZER;
// Also need this, it is a structure initializer 
// Cannot use it to init the structure in a statemnt apart from declaration
pthread_cond_t cond_new_addition = PTHREAD_COND_INITIALIZER;

// Create nodes for task queue
// Will need to hold information and have a pointer to point to next object
typedef struct WTQueue_nodes {
  long information;
  struct WTQueue_nodes *ptr; 
} WTQueue_nodes;

// Create linked list (will keep it single linked list for now, will need to see if this needs to be changed later)
typedef struct Queue_LList {
  WTQueue_nodes *ptr_;
} Queue_LList; 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Notes for me 
/*
 * Types for variables and functions that need to be used:
 * 
 * Types of functions (learn about pthread library to understand them)
 * pthread_t 
 * pthread_mutex_t
 * pthread_cond_t
 * 
*/

/* 
 * Functions needed (Put these in front of function name)
 * Make sure to learn more about these and how they are used to create functions
 * 
 * pthread_create()
 * pthread_join()
 * 
 * pthread_mutex_init()
 * pthread_mutex_lock()
 * pthread_mutex_unlock()
 * 
 * pthread_cond_init()
 * pthread_cond_wait()
 * pthread_cond_signal()
 * pthread_cond_broadcast()
 * 
*/

/*
 * Mutex Lock Definitions (If threads are available or not)
 *
 * acquire() {
 *  while (!available)
 *  // busy wait
 *  available = false;
 * }
 * 
 * release () {
 *  available = true;
 * }
 * 
 * These two functions are to be implemented automatically
 * Both test-and-set and compare-and-swap can be used to implement these functions
*/

/*
 * Condition Variables
 *
 * These two operations are allowed on a condition variable
 * (Do other operations work on them too?)
 * 
 * Use these variables for example
 * condition x, y;
 * 
 * x.wait() - invokes the operation is suspended until x.signal() is used
 * 
 * x.signal() - resumes one process (if any) that invoked x.wait()
 * 
 * If there is no x.wait(), then x.signal() has no affect
*/

/*
 * pthread syntax
 *
 * int pthread_cond_wait (cond, mutex)
 * 
 * pthread_cond_t *cond;
 * pthread_mutex_t *mutex;
 * 
 * int pthread_cond_timedwait (cond, mutex, timeout)
 * pthread_cond_t *cond;
 * pthread_mutex_t *mutex;
 * const struct timespec *timeout;
 * 
 * Follow these guidelines when making the pthread functions
*/

// If there's a task, thread, or whatever working, just put the lock and unlock mutex
// Can't hurt to just put them everywhere right? 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function protoypes
// Functions will be explained when they are created in the next section

// Calculate square function - given
void calculate_square(long number);

// Declare create queue and delete queue functions
// Use structs created earlier
// Creates and deletes using dynamic allocation
Queue_LList *queue_creation(void);
void queue_deletion(Queue_LList *queue_input);

// After you queue a task, you must also undo that queue, that will be the job of this function
long undo_queuetask (Queue_LList *queue_input);

// Threads call this function - given from slides
void *runner (void *param); 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions

// Given calculate_square function
// May need to change, there should be mutex locks and unlocks when dealing with these values
void calculate_square(long number)
{
  // Mutex Lock, placed at beginning but has given me problems
  pthread_mutex_lock(&lock_queue);

  // calculate the square
  long the_square = number * number;

  // ok that was not so hard, but let's pretend it was
  // simulate how hard it is to square this number!
  sleep(number);

  // let's add this to our (global) sum
  sum += the_square;

  // now we also tabulate some (meaningless) statistics
  if (number % 2 == 1) {
    // how many of our numbers were odd?
    odd++;
  }

  // what was the smallest one we had to deal with?
  if (number < min) {
    min = number;
  }

  // and what was the biggest one?
  if (number > max) {
    max = number;
  }

  // Mutex Unlock, makes sense to put it last
  // threads will be done doing work
  pthread_mutex_unlock(&lock_queue);

}

// Function for threads to execute - given from slides
void *runner (void *param) {
  int i, upper = atoi (param);
  sum = 0;

  for (i = 1; i <= upper; i++) {
    sum += i;
  }

  pthread_exit(0);
  return 0;
}

// Queue_LList function
/*
Queue_LList *queue_creation() {
    pthread_mutex_lock(&lock_queue);
    Queue_LList *DAcreate = malloc(sizeof(struct Queue_LList));

    DAcreate->ptr = NULL;

    return DAcreate;
    pthread_mutex_unlock(&lock_queue);
}
*/ // Function did not work, will comment out for now

// Function in order to create queue
// Referenced portions of code from online source
Queue_LList *queue_creation() {
    // Set struct pointer to dynamically allocate memory
    Queue_LList *DAcreate = (Queue_LList*)malloc(sizeof(struct Queue_LList));
    // If this doesn't work, the function has failed to dynamically allocate memory
    if (!DAcreate) return DAcreate; 

    // Set pointer that I just created
    DAcreate->ptr_ = NULL;

    // Return pointer that dynamically allocated memory
    // return ptr_; // Can't return this value 
    return DAcreate;
}

// Function in order to delete queue
// Referenced portions of code from online source
// Will take in input queue value
void queue_deletion(Queue_LList *queue_input) {
    // If no input queue found then just return
    if (!queue_input) return;

    // Get rid of nodes that are queued already
    while (queue_input->ptr_) undo_queuetask(queue_input);

    // free function? found online, will delete queue
    free(queue_input);
}

// Function in order to undo queue
// Referenced portions of code from online source
// Will take in input queue value
long undo_queuetask(Queue_LList* queue_input) {

    // Create pointer to equal value of queue input
    // This created object will hold temporary value that will later be stored in long variable
    WTQueue_nodes *temp_value = queue_input->ptr_;

    // If temp value is true then set pointers
    if (temp_value) {
      queue_input->ptr_ = temp_value->ptr;
    }

    // If temp value is false just return 0
    if (!temp_value){
      return 0;
    } 
    
    // Create new long variable to store information of created pointer
    // Instead of returning the pointer value, set it to a long variable and then return that instead
    const long return_value = temp_value -> information;

    // get rid of new pointer, don't need it anymore
    free(temp_value);

    // Return newly created value
    return return_value;
}

/*
int undo_queuetask() { 

    pthread_mutex_lock(&lock_queue);

    WTQueue_nodes *x = queue_input->ptr_;

    if (x) {
      queue_input->ptr_ = x->ptr;
    }

    
    // Create new long variable to store information of created pointer and set it to value
    int y = x -> data; // this doesn't work

    // free up value
    free(x); // don't know if needed

    pthread_mutex_unlock(&lock_queue);
    return y;
}
*/ // Function does not work, problem with how variables are stored, will look into more

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
// argc - arguments
// argv - array of strings
int main(int argc, char* argv[])
{
  // Initialize number of workers
  // int number_of_workers = 5; // For tests
  // int number_of_workers = 10; // For tests
  int number_of_workers = WORKERS; // may need to change number 

  // To test argument value
  // check and parse command line options
  if (argc != 2) { // test if argc is not 2
    printf("Usage: sumsq <infile> <number of workers>\n");
    printf("Please enter data file and number of workers below\n");
    exit(EXIT_FAILURE);
  }

  if (argc != 3) { // test if argc is not 3
    printf("Usage: sumsq <infile> <number of workers>\n");
    printf("Please enter data file and number of workers below\n");
    exit(EXIT_FAILURE);
  }

  // Some testing
  /*
   if (argc == 2) {
    number_of_workers = 3;
    exit(EXIT_FAILURE);
   }
  */

  /*
   if (argc == 3) {
    number_of_workers = 2;
    exit(EXIT_FAILURE);
   }
  */
 
  // Some boolean values to see if task is working or finished
  bool finished = false; // Set to false because tasks are not done at start
  bool working = true; // Set to true, assume task is working

  // Some pthread things, given from slides
  pthread_t tid; // Thread identifier
  pthread_attr_t attr; // Set of thread attributes

  // Set default attributes for threads
  pthread_attr_init(&attr);

  // Create thread
  pthread_create(&tid, &attr, runner, argv[1]);

  // Wait for thread to exit
  pthread_join(tid, NULL);

  // Print sum value
  // printf("sum = %d\n", sum);

  // load numbers and add them to the queue
  char *fn = argv[1]; // Needed, assigns character value
  FILE* fin = fopen(fn, "r"); // Opening inputfile/datafile
  char action;
  long num;

  // Main section of code below - Testing, tasks, and threads
  // Will take in test files and read tasks
  // If 'p' then // 'p' = process

  /*
   * a "processing" task with number n means it is calculating the square (n^2) of the number. 
   * Furthermore, we will pretend that it takes n seconds to do this calculation. 
   * Once done, each processing task will update a few global aggregate variables, 
   * one being the resulting sum-of-squares, 
   * and then a few other stats (odd count, min, max) to simulate a variety of shared or partially-shared global variables.
  */

  // If 'w' then do nothing // 'w' = wait
  // Other values will not be accepted
  while (fscanf(fin, "%c %ld\n", &action, &num) == 2) { // store input values, p=action, inputvalues=num
    if (action == 'p') { // process, do some work

      working = true; // Task is now working

      pthread_mutex_lock(&lock_queue); // First things first, mutex lock

      calculate_square(num); // Calculate square value 

      pthread_cond_broadcast(&cond_new_addition); // Wake up threads

      pthread_mutex_unlock(&lock_queue); // Mutex unlock

      working = false; // Task is done working

    } else if (action == 'w') { // wait, nothing new happening

      working = false;
      sleep(num);

    } else {

      printf("ERROR: Unrecognized action: '%c'\n", action);
      exit(EXIT_FAILURE);

    }
  }
  fclose(fin);

  // After closing the above function, will need this so workers will no longer wait on conditional 
  pthread_mutex_lock(&lock_queue); // Mutex lock
  pthread_cond_broadcast(&cond_new_addition); // Use broadcast to wake up threads
  pthread_mutex_unlock(&lock_queue); // Mutex unlock
  
  /*
  // Test if working
  if (working != true) {
    printf("NOT WORKING!\n");
  }
  */

  // print results
  printf("%ld %ld %ld %ld\n", sum, odd, min, max);
  
  // Boolean test value - all tasks are finished
  finished = true;

  // clean up and return
  return (EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// END //
// Refer to makefile to compile and run program
/////////////////////////////////////////////////////////////////////////////////////////////////////////////