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
bool done = false;

// Global Variables (if needed)
// Guess we do need one, set number of workers
#define WORKERS 1;

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

/* Functions needed
 *
 * pthread_create ()
 * pthread_join ()
 * 
 * pthread_mutex_init ()
 * pthread_mutex_lock ()
 * pthread_mutex_unlock()
 * 
 * pthread_cond_init()
 * pthread_cond_wait()
 * pthread_cond_signal()
 * pthread_cond_broadcast()
 * 
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function protoypes
// Functions will be explained when they are created in the next section

// Calculate square function - given
void calculate_square(long number);

// Declare create queue and delete queue functions
// Use structs created earlier
// Creates and deletes using dynamic allocation
Queue_LList *create_queue(void);
void delete_queue(Queue_LList *queue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
/*
 * update global aggregate variables given a number
 */
void calculate_square(long number)
{

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
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function
int main(int argc, char* argv[])
{
  // check and parse command line options
  if (argc != 2) {
    printf("Usage: sumsq <infile>\n");
    exit(EXIT_FAILURE);
  }
  char *fn = argv[1];
  
  // load numbers and add them to the queue
  FILE* fin = fopen(fn, "r");
  char action;
  long num;

  while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
    if (action == 'p') {            // process, do some work
      calculate_square(num);
    } else if (action == 'w') {     // wait, nothing new happening
      sleep(num);
    } else {
      printf("ERROR: Unrecognized action: '%c'\n", action);
      exit(EXIT_FAILURE);
    }
  }
  fclose(fin);
  
  // print results
  printf("%ld %ld %ld %ld\n", sum, odd, min, max);
  
  // clean up and return
  return (EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// END //
// Refer to makefile to compile and run program
/////////////////////////////////////////////////////////////////////////////////////////////////////////////