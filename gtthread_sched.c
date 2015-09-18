/**********************************************************************
gtthread_sched.c.  

This file contains the implementation of the scheduling subset of the
gtthreads library.  A simple round-robin queue should be used.
 **********************************************************************/
/*
  Include as needed
*/

#include "gtthread.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
/* 
   Students should define global variables and helper functions as
   they see fit.
 */

static steque_t queue;
extern ucontext_t uctx_main;


/*
  The gtthread_init() function does not have a corresponding pthread equivalent.
  It must be called from the main thread before any other GTThreads
  functions are called. It allows the caller to specify the scheduling
  period (quantum in micro second), and may also perform any other
  necessary initialization.

  Recall that the initial thread of the program (i.e. the one running
  main() ) is a thread like any other. It should have a
  gtthread_t that clients can retrieve by calling gtthread_self()
  from the initial thread, and they should be able to specify it as an
  argument to other GTThreads functions. The only difference in the
  initial thread is how it behaves when it executes a return
  instruction. You can find details on this difference in the man page
  for pthread_create.
 */
void gtthread_init(long period){


}


/*
  The gtthread_create() function mirrors the pthread_create() function,
  only default attributes are always assumed.
 */
int gtthread_create(gtthread_t *thread,
        void *(*start_routine)(void *),
        void *arg){
  if (getcontext(&(thread->gtthread_context)) == -1)
  {
    perror("getcontext");
  }

  thread->gtthread_context.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
  thread->gtthread_context.uc_stack.ss_size = SIGSTKSZ;
  thread->gtthread_context.uc_link = &uctx_main;

  makecontext(&(thread->gtthread_context), (void *)start_routine, 1, arg);
  swapcontext(&uctx_main, &(thread->gtthread_context));
}

/*
  The gtthread_join() function is analogous to pthread_join.
  All gtthreads are joinable.
 */
int gtthread_join(gtthread_t thread, void **status){


}

/*
  The gtthread_exit() function is analogous to pthread_exit.
 */
void gtthread_exit(void* retval){



}


/*
  The gtthread_yield() function is analogous to pthread_yield, causing
  the calling thread to relinquish the cpu and place itself at the
  back of the schedule queue.
 */
void gtthread_yield(void){


}

/*
  The gtthread_yield() function is analogous to pthread_equal,
  returning zero if the threads are the same and non-zero otherwise.
 */
int  gtthread_equal(gtthread_t t1, gtthread_t t2){


}

/*
  The gtthread_cancel() function is analogous to pthread_cancel,
  allowing one thread to terminate another asynchronously.
 */
int  gtthread_cancel(gtthread_t thread){

}

/*
  Returns calling thread.
 */
gtthread_t gtthread_self(void){

}
