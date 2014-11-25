/** @file ctx_switch.c
 * 
 * @brief C wrappers around assembly context switch routines.
 *
 * @author Hailei Yu <haileiy@andrew.cmu.edu>
 *         Hongyi Zhang <hongyiz@andrew.cmu.edu>
 *         Team #22
 */
 

#include <types.h>
#include <assert.h>

#include <config.h>
#include <kernel.h>
#include "sched_i.h"

#ifdef DEBUG_MUTEX
#include <exports.h>
#endif

static __attribute__((unused)) tcb_t* cur_tcb; /* use this if needed */


/*
 * functions for testing
 */
void print_reg(unsigned reg){
	printf("ssss %u\n", reg);
}

void print_stack(unsigned* sp){
	int i = 0;
	while(i < 10){
		printf("%d: %u\n", i, sp[i]);
		i++;
	}
}

/**
 * @brief Initialize the current TCB and priority.
 *
 * Set the initialization thread's priority to IDLE so that anything
 * will preempt it when dispatching the first task.
 */
void dispatch_init(tcb_t* idle __attribute__((unused)))
{

}

/**
 * @brief Context switch to the highest priority task while saving off the 
 * current task state.
 *
 * This function needs to be externally synchronized.
 * We could be switching from the idle task.  The priority searcher has been tuned
 * to return IDLE_PRIO for a completely empty run_queue case.
 */
void dispatch_save(void)
{
	/* find the task with highest prio */
	uint8_t new_prio = highest_prio();
	tcb_t* new_tcb = get_tcb_by_prio(new_prio);
	tcb_t* old_tcb = get_cur_tcb();
	runqueue_add(old_tcb, get_cur_prio());
	/* remove the new tcb from runqueue */
	runqueue_remove(new_prio);
	/* update cur_tcb */
	cur_tcb = new_tcb;
	/* context switch to the new task */
	ctx_switch_full(&(new_tcb->context.r4), &(old_tcb->context.r4));
}

/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * don't save the current task state.
 *
 * There is always an idle task to switch to.
 */

void dispatch_nosave(void)
{
	/* find the task with highest prio */
	uint8_t prio = highest_prio();
	tcb_t* new_tcb = get_tcb_by_prio(prio);
	cur_tcb = new_tcb;
	/* remove the new tcb from runqueue */
	runqueue_remove(prio);
	/* context switch to new task */
	ctx_switch_half(&(new_tcb->context.r4));
}


/**
 * @brief Context switch to the highest priority task that is not this task -- 
 * and save the current task but don't mark is runnable.
 *
 * There is always an idle task to switch to.
 */
void dispatch_sleep(void)
{
	/* find the task with highest prio */
	uint8_t new_prio = highest_prio();
	tcb_t* new_tcb = get_tcb_by_prio(new_prio);
	tcb_t* old_tcb = get_cur_tcb();
	/* if the new tcb is not idle, remove the new tcb from runqueue */
	if(new_prio != IDLE_PRIO)
		runqueue_remove(new_prio);
	/* update cur_tcb */
	cur_tcb = new_tcb;
	/* context switch to new task */
	ctx_switch_full((sched_context_t*) &(new_tcb->context), (sched_context_t*) &(old_tcb->context));
}

/**
 * @brief Returns the priority value of the current task.
 */
uint8_t get_cur_prio(void)
{
	return cur_tcb->cur_prio;
}

/**
 * @brief Returns the TCB of the current task.
 */
tcb_t* get_cur_tcb(void)
{
	return cur_tcb; 
}
