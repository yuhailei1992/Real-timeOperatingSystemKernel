/** @file proc.c
 * 
 * @brief Implementation of `process' syscalls
 *
 * @author Hailei Yu <haileiy@andrew.cmu.edu>
 *         Hongyi Zhang <hongyiz@andrew.cmu.edu>
 *         Team #22
 * date    2014-11-24
 */

#include <exports.h>
#include <bits/errno.h>
#include <config.h>
#include <kernel.h>
#include <syscall.h>
#include <sched.h>

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>
#include <device.h>

extern tcb_t system_tcb[OS_MAX_TASKS];

/* 
 * @brief rank the tasks by their T
 * if two tasks have the same T, their relative position is maintained
 * @param tasks, an array of tasks
 * @param num_tasks, the size of task array
 * @return void
 */
static void rank_by_T (task_t* tasks, int num_tasks)
{
  int i;
  for (i = 0; i < num_tasks; ++i)
  {
    int min_pos = i;
    unsigned min = tasks[i].T;
    int j = i;
    for (; j < num_tasks; ++j)
    {
      if (tasks[j].T < min)
      {
        min = tasks[j].T;
        min_pos = j;
      }
    }
    task_t tmp = tasks[min_pos];
    tasks[min_pos] = tasks[i];
    tasks[i] = tmp;
  }
}

/*
 * @brief task_create takes a set of tasks, verifies that they are schedulable, 
 * and then begins to schedule them. All current tasks are cancelled, and a 
 * entirely new task set is scheduled upon success
 * @param tasks, an array of tasks
 * @param num_tasks, the size of task array
 * @return 0 upon success, negative value upon failure
 */
int task_create(task_t* tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
  /* num_tasks is greater than the maximum number of tasks */
  if(num_tasks > 64){
    return -EINVAL;
  }
  /* tasks point to region whose bounds lie outside valid address space */
  else if (valid_addr(tasks, num_tasks * sizeof(task_t), USR_START_ADDR, USR_END_ADDR) == 0)
  {
    return -EFAULT;
  }
  /* check if the tasks are not schedulable will be done in part 2 */
  else
  {
    rank_by_T(tasks, num_tasks);
    allocate_tasks(&tasks, num_tasks);
    dispatch_nosave();  
    return 0;
  }
}
/*
 * @brief event_wait puts the calling task to sleep on a given device numbers sleep
 * queue until the device next signals the device event. Device events are 
 * signalled regularly based on the signalling frequency of the device
 * @param dev, device number
 * @return 0 upon success, negative value upon failure
 */
int event_wait(unsigned int dev  __attribute__((unused)))
{
  /* invalid device number */
  if (dev > NUM_DEVICES)
  {
    return -EINVAL;
  }
  else if(get_cur_tcb()->holds_lock > 0){
    return -EHOLDSLOCK;
  }
  else
  {
    dev_wait(dev);
    dispatch_sleep();
    return 0;
  }
}

/* An invalid syscall causes the kernel to exit. */
void invalid_syscall(unsigned int call_num  __attribute__((unused)))
{
	printf("Kernel panic: invalid syscall -- 0x%08x\n", call_num);

	disable_interrupts();
	while(1);
}
