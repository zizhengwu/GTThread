#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"

/* Tests creation.
   Should print "Hello World!" */
ucontext_t uctx_main;

void *thr1(void *in) {
  int i;
  for (i = 0; i < 1000000000; ++i)
  {
    if (i%1000000 == 0)
    {
      printf("1: %d\n", i);
    }
    // printf("1: %d\n", i);
  }
  return NULL;
}

void *thr2(void *in) {
  int i;
  for (i = 0; i < 10000; ++i)
  {
    if (i%1000000 == 0)
    {
      printf("2: %d\n", i);
    }
  }
  return NULL;
}

void *thr3(void *in) {
  int i;
  for (i = 0; i < 10000000; ++i)
  {
    if (i % 1000000 == 0)
    {
      printf("3: %d\n", i);
    }
  }
  return NULL;
}

int main() {
  gtthread_t t1, t2, t3;
  gtthread_init(500);
  gtthread_create( &t1, thr1, NULL);
  gtthread_create( &t2, thr2, NULL);
  gtthread_create( &t3, thr3, NULL);
  // gtthread_join(t1, NULL);
  // gtthread_join(t2, NULL);
  // gtthread_join(t3, NULL);
  gtthread_join(t2, NULL);
  gtthread_yield();
  // gtthread_yield();
  // gtthread_yield();
  // gtthread_yield();

  printf("main exit\n");

  return EXIT_SUCCESS;
}
