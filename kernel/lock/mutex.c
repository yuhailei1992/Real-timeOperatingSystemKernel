/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 *
 * @author Hailei Yu <haileiy@andrew.cmu.edu>
 *         Hongyi Zhang <hongyiz@andrew.cmu.edu>
 *         Team #22
 * @date  2014-11-22
 */


#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <exports.h>

#define NULL 0
mutex_t gtMutex[OS_NUM_MUTEX];

/* 
 * @brief mutex_init, initialize all the mutexes
 * @param void
 * @return void
 */
void mutex_init()
{
	int i;
	for (i = 0; i < OS_NUM_MUTEX; ++i)
	{
		gtMutex[i].bAvailable = TRUE;
		gtMutex[i].pHolding_Tcb = FALSE;
		gtMutex[i].bLock = 0;
		gtMutex[i].pSleep_queue = NULL;
	}	
}

/* 
 * @brief mutex_create, search all the mutexes and find an available one
 * @param void
 * @return id of an available mutex
 */
int mutex_create(void)
{
	int i;
	for (i = 0; i < OS_NUM_MUTEX; ++i)
	{
		if (gtMutex[i].bAvailable)
		{
			gtMutex[i].bAvailable = FALSE;
			return i;
		}
	}	
	/* cannot find any, return -ENOMEM */
	return -ENOMEM;
}

/* 
 * @brief mutex_lock, try to acquire a lock
 * @param mutex, the id of the desired mutex
 * @return 0 upon success, negative value upon failure
 */
int mutex_lock(int mutex  __attribute__((unused)))
{
	/* the mutex is invalid */
	if (mutex < 0 || mutex >= OS_NUM_MUTEX || gtMutex[mutex].bAvailable)
	{
		return -EINVAL;
	}
	/* deadlock */
	else if (gtMutex[mutex].pHolding_Tcb == get_cur_tcb())
	{
		return -EDEADLOCK;
	}
	/* the lock is available */
	else if (!gtMutex[mutex].bLock)
	{
		gtMutex[mutex].bLock = TRUE;
		gtMutex[mutex].pHolding_Tcb = get_cur_tcb();
		tcb_t *curr_tcb = get_cur_tcb();
		(*curr_tcb).holds_lock++;
		//set currrent task's priority to 0
		promote_cur_task();
		return 0;
	}
	/* another is holding the mutex */
	else
	{
		/* add current task to the queue */
		tcb_t *tmp = gtMutex[mutex].pSleep_queue;
		tcb_t *curr_tcb = get_cur_tcb();
		if (tmp == NULL)
		{
			gtMutex[mutex].pSleep_queue = curr_tcb;
			curr_tcb->sleep_queue = NULL;
		}
		else
		{
			while (tmp->sleep_queue != NULL)
			{
				tmp = (*tmp).sleep_queue;
			}
			(*tmp).sleep_queue = get_cur_tcb();
			tmp = (*tmp).sleep_queue;
			(*tmp).sleep_queue = NULL;
		}
		
		dispatch_sleep();
		return 0;
	}
}

/* 
 * @brief mutex_unlock, try to unlock a mutex
 * @param mutex, the desired mutex's id
 * @return 0 upon success, negative value upon failure
 */
int mutex_unlock(int mutex  __attribute__((unused)))
{
	/* the provided mutex identifier is invalid */
	if (mutex < 0 || mutex >= OS_NUM_MUTEX || gtMutex[mutex].bAvailable)
	{
		return -EINVAL;
	}
	/* the current task does not hold the mutex */
	else if (gtMutex[mutex].pHolding_Tcb != get_cur_tcb())
	{
		return -EPERM;
	}
	/* regular procedure */
	else
	{
		tcb_t *curr_tcb = get_cur_tcb();
		(*curr_tcb).holds_lock--;

		//degrad current task's priority back if 
		//it holds no lock now.
		degrad_cur_task();
		/* add to run_queue */
		tcb_t *to_wake = gtMutex[mutex].pSleep_queue;
		gtMutex[mutex].pHolding_Tcb = to_wake;
		/* add the first task on the mutex queue to runqueue */
		if (to_wake)
		{
			gtMutex[mutex].pSleep_queue = (*to_wake).sleep_queue;
			(*to_wake).sleep_queue = NULL;
			runqueue_add(to_wake, (*to_wake).cur_prio);
			gtMutex[mutex].bLock = TRUE;
			if(gtMutex[mutex].pHolding_Tcb->cur_prio < get_cur_prio()){
				dispatch_sleep();
			}
		}
		else
			gtMutex[mutex].bLock = FALSE;
		return 0;
	}
}

