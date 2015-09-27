#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <pthread.h>
#include "gtthread.h"
#include "philosopher.h"

/*
 * Performs necessary initialization of mutexes.
 */
static gtthread_mutex_t chopsticks_mutex[5];

void chopsticks_init(){
  int i;
  for (i = 0; i < 5; ++i)
  {
    gtthread_mutex_init(&chopsticks_mutex[i]);
  }
  
}

/*
 * Cleans up mutex resources.
 */
void chopsticks_destroy(){

}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */   
void pickup_chopsticks(int phil_id){
  if (philosopher_chopstics_left_right_correspondance(phil_id, 0) > philosopher_chopstics_left_right_correspondance(phil_id, 1))
  {
    gtthread_mutex_lock(&chopsticks_mutex[philosopher_chopstics_left_right_correspondance(phil_id, 1)]);
    pickup_right_chopstick(phil_id);
    gtthread_mutex_lock(&chopsticks_mutex[philosopher_chopstics_left_right_correspondance(phil_id, 0)]);
    pickup_left_chopstick(phil_id);
  }
  else
  {
    gtthread_mutex_lock(&chopsticks_mutex[philosopher_chopstics_left_right_correspondance(phil_id, 0)]);
    pickup_left_chopstick(phil_id);
    gtthread_mutex_lock(&chopsticks_mutex[philosopher_chopstics_left_right_correspondance(phil_id, 1)]);
    pickup_right_chopstick(phil_id);
  }
}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */   
void putdown_chopsticks(int phil_id){
  if (philosopher_chopstics_left_right_correspondance(phil_id, 1) > philosopher_chopstics_left_right_correspondance(phil_id, 0))
  {
    putdown_right_chopstick(phil_id);
    gtthread_mutex_unlock(&chopsticks_mutex[philosopher_chopstics_left_right_correspondance(phil_id, 1)]);
    putdown_left_chopstick(phil_id);
    gtthread_mutex_unlock(&chopsticks_mutex[philosopher_chopstics_left_right_correspondance(phil_id, 0)]);
  }
  else
  {
    putdown_left_chopstick(phil_id);
    gtthread_mutex_unlock(&chopsticks_mutex[philosopher_chopstics_left_right_correspondance(phil_id, 0)]);
    putdown_right_chopstick(phil_id);
    gtthread_mutex_unlock(&chopsticks_mutex[philosopher_chopstics_left_right_correspondance(phil_id, 1)]);
  }
}

int philosopher_chopstics_left_right_correspondance(int phil_id, int left_right_indicator){
  // 0 for left, 1 for right
  if (left_right_indicator == 0)
  {
    if (phil_id == 0)
      return 4;
    else
      return(phil_id - 1);
  }
  else if (left_right_indicator == 1)
  {
    return phil_id;
  }
  return -1;
}