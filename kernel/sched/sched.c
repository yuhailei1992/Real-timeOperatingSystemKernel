/** @file sched.c
 * 
 * @brief Top level implementation of the scheduler.
 *
 * @author Hailei Yu <haileiy@andrew.cmu.edu>
 *         Hongyi Zhang <hongyiz@andrew.cmu.edu>
 *         Team #22
 * @date  2014-11-22
 */

#include <exports.h>
#include <types.h>
#include <assert.h>

#include <kernel.h>
#include <config.h>
#include "sched_i.h"

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>
#include <device.h>

extern uint32_t global_data;

extern void launch_task();

tcb_t system_tcb[OS_MAX_TASKS]; /*allocate memory for system TCBs */

void init_idle(tcb_t* idle_task);
void init_task(task_t* new_task, int sys_tcb_no, uint8_t prio);
static void idle();

/* initialize idle task */
void init_idle(tcb_t* idle_task){
	idle_task->native_prio = IDLE_PRIO;
	idle_task->cur_prio = IDLE_PRIO;
	idle_task->sleep_queue = 0;
	idle_task->holds_lock = 0;
	idle_task->context.r8 = global_data;
	idle_task->context.lr = (void*)idle;
	idle_task->context.sp = idle_task->kstack_high;
	/* add idle task to runqueue */
	runqueue_add(idle_task, IDLE_PRIO);
}

/* given the priority, get the corresponding tcb */
tcb_t* get_tcb_by_prio(uint8_t prio){
	return &(system_tcb[prio]);
}

//promote current task's priority to 0
void promote_cur_task(){
	get_cur_tcb()->cur_prio = 0;
}

//if current task hold no lock, degrade it's priority to 
//its native value
void degrad_cur_task(){
	if(get_cur_tcb()->holds_lock > 0)
		return;
	get_cur_tcb()->cur_prio = get_cur_tcb()->native_prio;
}

void sched_init(task_t* main_task  __attribute__((unused)))
{
	runqueue_init();
	init_idle(&(system_tcb[IDLE_PRIO]));
}

/**
 * @brief This is the idle task that the system runs when no other task is runnable
 */
 
static void __attribute__((unused)) idle(void)
{
	 enable_interrupts();
	 while(1);
}

/**
 * @brief Allocate user-stacks and initializes the kernel contexts of the
 * given threads.
 *
 * This function assumes that:
 * - num_tasks < number of tasks allowed on the system.
 * - the tasks have already been deemed schedulable and have been appropriately
 *   scheduled.  In particular, this means that the task list is sorted in order
 *   of priority -- higher priority tasks come first.
 *
 * @param tasks  A list of scheduled task descriptors.
 * @param size   The number of tasks is the list.
 */

void allocate_tasks(task_t** tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
	unsigned i;

	dev_init();
	runqueue_init();
	/* add the idle task to runqueue */
	runqueue_add(&(system_tcb[IDLE_PRIO]), IDLE_PRIO);
	/* add regular tasks to runqueue */

	/*for part2: assign priority from 1, save 0 for HLP*/
	for(i = 0; i < num_tasks; i++){
		uint8_t prio = (uint8_t)i + 1;
		init_task(&((*tasks)[i]), i + 1, prio);
		runqueue_add(&(system_tcb[i]), prio);
	}
}

/**
 * @brief initialize tasks
 * @param new_task, the task to be initialized
 * @param sys_tcb_no, the corresponding tcb id
 * @param prio, the priority
 * @return void
 */
void init_task(task_t* new_task, int sys_tcb_no, uint8_t prio){
	system_tcb[sys_tcb_no].native_prio = prio;
	system_tcb[sys_tcb_no].cur_prio = prio;
	system_tcb[sys_tcb_no].sleep_queue = 0;
	system_tcb[sys_tcb_no].holds_lock = 0;
	system_tcb[sys_tcb_no].context.r4 = (uint32_t) new_task->lambda;
	system_tcb[sys_tcb_no].context.r5 = (uint32_t) new_task->data;
	system_tcb[sys_tcb_no].context.r6 = (uint32_t)new_task->stack_pos;
	system_tcb[sys_tcb_no].context.sp = system_tcb[sys_tcb_no].kstack_high;
	system_tcb[sys_tcb_no].context.r8 = global_data;
	system_tcb[sys_tcb_no].context.lr = (void*) launch_task;
}

