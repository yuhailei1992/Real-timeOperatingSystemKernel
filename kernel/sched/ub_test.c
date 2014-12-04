/** @file ub_test.c
 * 
 * @brief The UB Test for basic schedulability
 *
 * @author Hailei Yu <haileiy@andrew.cmu.edu>
 *         Hongyi Zhang <hongyiz@andrew.cmu.edu>
 *         Team #22
 * @date 2014-12-03
 */

//#define DEBUG 0

#include <sched.h>
#ifdef DEBUG
#include <exports.h>
#endif

/*
 * it is difficult to calculate double float in arm, so I just
 * use a table 
 */
static double table[64] = {1.0 ,
	0.8284271247461903 ,0.7797631496846196 ,
	0.7568284600108841 ,0.7434917749851755 ,
	0.7347722898562381 ,0.7286265957166858 ,
	0.7240618613220615 ,0.720537650030755  ,
	0.7177346253629313 ,0.7154519838395894 ,
	0.7135571323115437 ,0.7119589942614066 ,
	0.7105929411450722 ,0.7094118423094009 ,
	0.7083805188386201 ,0.7074721810599254 ,
	0.7066660685731811 ,0.7059458444776459 ,
	0.7052984768275516 ,0.7047134431475821 ,
	0.7041821541526114 ,0.7036975292962169 ,
	0.7032536794438098 ,0.7028456664016636 ,
	0.702469318428085  ,0.7021210870900434 ,
	0.701797935044489  ,0.7014972472209191 ,
	0.7012167599032493 ,0.7009545036393181 ,
	0.7007087569317321 ,0.7004780084070155 ,
	0.7002609257059471 ,0.7000563297419649 ,
	0.6998631732772136 ,0.6996805229942746 ,
	0.6995075444158751 ,0.6993434891582322 ,
	0.6991876841074518 ,0.6990395221887002 ,
	0.6988984544612906 ,0.6987639833227344 ,
	0.6986356566442611 ,0.6985130626923775 ,
	0.6983958257160272 ,0.6982836020999479 ,
	0.6981760770011398 ,0.6980729613990364 ,
	0.6979739895014547 ,0.697878916456967  ,
	0.6977875163324176 ,0.697699580320482  ,
	0.6976149151471231 ,0.6975333416535634 ,
	0.6974546935308403 ,0.6973788161880392 ,
	0.6973055657380844 ,0.6972348080871495 ,
	0.6971664181153514 ,0.6971002789384473 ,
	0.6970362812412048 ,0.6969743226744181 ,
	0.6969143073088304};

/*
 * selection sort
 * rank the tasks by their T in ascending order
 * satisfies rate monotonic principle
 */
static void rank_by_T2 (task_t* tasks, int num_tasks)
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

 
/**
 * @brief Perform UB Test and reorder the task list.
 *
 * The task list at the end of this method will be sorted in order is priority
 * -- from highest priority (lowest priority number) to lowest priority
 * (highest priority number).
 *
 * @param tasks  An array of task pointers containing the task set to schedule.
 * @param num_tasks  The number of tasks in the array.
 *
 * @return 0  The test failed.
 * @return 1  Test succeeded.  The tasks are now in order.
 */

int assign_schedule(task_t** tasks  __attribute__((unused)), size_t num_tasks  __attribute__((unused)))
{
	task_t *tmp = *tasks;
	rank_by_T2(tmp, num_tasks);
	task_t *tmp2 = *tasks;
	double sum = 0.0;
	unsigned i;
	for (i = 0; i < num_tasks-1; ++i)
	{
		sum += (double)tmp2[i].C / (double)tmp2[i].T;
	}
	sum += ((double)tmp2[i].C + (double)tmp2[i].B) / (double)tmp2[i].T;
	if (sum > table[num_tasks-1])
		return 0;
	else
		return 1;
}
	
