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
#include "string.h"
#include<sys/time.h>

/* 
   Students should define global variables and helper functions as
   they see fit.
 */
static steque_t queue;
static sigset_t vtalrm;
static struct itimerval *T;
static struct sigaction act;
static struct sigaction act;
static gtthread_t* uctx_main;
static gtthread_t* current_thread;
static int id = 1;
static bool first_time_swap = 1;


void alrm_handler(int sig){
  sigprocmask(SIG_BLOCK, &vtalrm, NULL);
  static gtthread_t* previous_thread;
  fprintf(stderr, "alarmed, queue size: %d\n", queue.N);
  if (first_time_swap)
  {
    current_thread = uctx_main;
    steque_pop(&queue);
    sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
    first_time_swap = 0;
  }
  else
  {
    if (!current_thread->finished)
    {
      steque_enqueue(&queue, current_thread);
      fprintf(stderr, "enqueue %d, queue size: %d, swap from %d to %d\n", current_thread->id, queue.N ,current_thread->id, ((struct gtthread_t*)(queue.front->item))->id);
      previous_thread = current_thread;
      current_thread = ((struct gtthread_t*)(queue.front->item));
      sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
      swapcontext(&previous_thread->gtthread_context, &(((struct gtthread_t*)(steque_pop(&queue))))->gtthread_context);
      if (current_thread->finished == 1)
      {
        gtthread_yield();
      }
    }
    else if (current_thread->finished)
    {
      fprintf(stderr, "queue size: %d, swap from %d to %d\n", queue.N ,current_thread->id, ((struct gtthread_t*)(queue.front->item))->id);
      previous_thread = current_thread;
      current_thread = ((struct gtthread_t*)(queue.front->item));
      sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
      swapcontext(&previous_thread->gtthread_context, &(((struct gtthread_t*)(steque_pop(&queue))))->gtthread_context);
      if (current_thread->finished == 1)
      {
        gtthread_yield();
      }
    }
    else
    {
      fprintf(stderr, "undefined behaviour\n");
    }
  }
}

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
  steque_init(&queue);
  /*
  Setting up the signal mask
  */
  sigemptyset(&vtalrm);
  sigaddset(&vtalrm, SIGVTALRM);
  sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);

  /*
    Setting up the handler
  */
  memset (&act, '\0', sizeof(act));
  act.sa_handler = &alrm_handler;
  if (sigaction(SIGVTALRM, &act, NULL) < 0) {
    perror ("sigaction");
  }

  uctx_main = (gtthread_t*)malloc(sizeof(gtthread_t));
  uctx_main->id = 0;
  uctx_main->finished = 0;
  steque_enqueue(&queue, uctx_main);

  /* 
     Setting up the alarm
  */
  T = (struct itimerval*) malloc(sizeof(struct itimerval));
  T->it_value.tv_sec = T->it_interval.tv_sec = 0;
  T->it_value.tv_usec = T->it_interval.tv_usec = period;

  if (period != 0)
  {
    setitimer(ITIMER_VIRTUAL, T, NULL);
  }
  
  gtthread_yield();
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

  thread->gtthread_context.uc_stack.ss_sp = (char*) malloc(40000);
  thread->gtthread_context.uc_stack.ss_size = 40000;
  thread->gtthread_context.uc_link = NULL;
  thread->id = id++;
  thread->finished = 0;
  thread->exited = 0;
  thread->canceled = 0;
  thread->retval = NULL;

  makecontext(&(thread->gtthread_context), (void*)gtthread_routine, 2, (void *)start_routine, arg);
  steque_enqueue(&queue, thread);
  return 0;
}

void gtthread_routine(void *(*start_routine)(void *), void *arg)
{
  current_thread->return_value = start_routine(arg);
  
  fprintf(stderr, "gtthread id: %d routine finished, queue size: %d\n", current_thread->id, queue.N);
  if (current_thread->finished)
  {
    /* code */
  }
  else if (!current_thread->finished)
  {
    current_thread->finished = 1;
  }
  
  gtthread_yield();
}

/*
  The gtthread_join() function is analogous to pthread_join.
  All gtthreads are joinable.
 */
int gtthread_join(gtthread_t thread_copy, void **status){
  fprintf(stderr, "try to join %d %d.\n", thread_copy.id, current_thread->id);
  gtthread_t* thread;
  int i;
  int found = 0;
  sigprocmask(SIG_BLOCK, &vtalrm, NULL);
  for (i = 0; i < queue.N; ++i)
  {
    if (((struct gtthread_t*)(queue.front->item))->id == thread_copy.id)
    {
      thread = ((struct gtthread_t*)(queue.front->item));
      found = 1;
    }
    steque_cycle(&queue);
  }
  sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
  if (!found)
  {
    fprintf(stderr, "not found in queue. already finished.\n");
    if (status != NULL)
    {
      if (thread->exited && thread->retval != NULL)
        *status = thread_copy.retval;
      else if (!thread->exited)
        *status = thread_copy.return_value;
    }
    return 0;
  }

  fprintf(stderr, "joined thread id = %d\n", thread->id);
  while(!thread->finished)
  {
    gtthread_yield();
  }
  fprintf(stderr, "joined thread id: %d finished, exited: %d\n", thread->id, thread->exited);
  if (status != NULL)
  {
    if (thread->exited && thread->retval != NULL)
      *status = thread->retval;
    else if (!thread->exited)
      *status= thread->return_value;
  }
  return 0;
}

/*
  The gtthread_exit() function is analogous to pthread_exit.
 */
void gtthread_exit(void* retval){
  current_thread->retval = retval;
  current_thread->exited = 1;
  current_thread->finished = 1;
  gtthread_yield();
}

/*
  The gtthread_yield() function is analogous to pthread_yield, causing
  the calling thread to relinquish the cpu and place itself at the
  back of the schedule queue.
 */
void gtthread_yield(void){
  raise(SIGVTALRM);
}

/*
  The gtthread_equal() function is analogous to pthread_equal,
  returning zero if the threads are the same and non-zero otherwise.
 */
int  gtthread_equal(gtthread_t t1, gtthread_t t2){
  return (t1.id == t2.id);
}

/*
  The gtthread_cancel() function is analogous to pthread_cancel,
  allowing one thread to terminate another asynchronously.
 */
int  gtthread_cancel(gtthread_t thread_copy){
  gtthread_t* thread;
  int i;
  for (i = 0; i < queue.N; ++i)
  {
    if (((struct gtthread_t*)(queue.front->item))->id == thread_copy.id)
    {
      thread = ((struct gtthread_t*)(queue.front->item));
    }
    steque_cycle(&queue);
  }
  thread->canceled = 1;
  thread->finished = 1;
  if (thread->id == current_thread->id)
  {
    gtthread_yield();
  }
  return 0;
}

/*
  Returns calling thread.
 */
gtthread_t gtthread_self(void){
  return *current_thread;
}

