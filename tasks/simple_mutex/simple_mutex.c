/** @file simple_mutex.c
 *
 * @brief Simple test for mutex. Should print out succeSs!
 *				Tests: 1. basic create,lock,unlock behavior, and dev_wait
 *							 2. unlock releases only oldest waiting task
 *				Overall flow: 1-2-3-3-1-2-1
 * @note   This will not work for part 2 of this lab
 *
 * @author Tao Yang(taoy)
 * @date   2010-11-30
 */
#include <stdio.h>
#include <unistd.h>
#include <task.h>
#include <lock.h>

int even_t1 = 1;
int once = 0;
int even_t2 = 1;
int even_t3 = 1;
volatile int mid = -1;
volatile int mid2 = -1;

void panic(const char* str)
{
	puts(str);
	while(1);
}

void fun1(void* str)
{
	// On first pass, create the shared mutex
	while(1){
	if(once == 0){
		mid = mutex_create();
		mid2 = mutex_create();
	}
	mutex_lock(mid);
	//puts("mid locked\n");
	mutex_lock(mid2);
	// prints e
	puts("fun1\n");
	//even_t1 = 1;
	mutex_unlock(mid);
	mutex_unlock(mid2);
	event_wait(0);
	}
}

void fun2(void* str)
{
	while(1){
	mutex_lock(mid);
	// prints e
	puts("fun2\n");
	sleep(5000);
	//even_t1 = 1;
	puts("fun2\n");
	mutex_unlock(mid);
	event_wait(1);
	}
}

void fun3(void* str)
{
	while(1){
	mutex_lock(mid);
	// prints e
	puts("fun3\n");
	//sleep(5000);
	//even_t1 = 1;
	//puts("fun2\n");
	mutex_unlock(mid);
	event_wait(1);
	}
}
int main(int argc, char** argv)
{
	task_t tasks[3];
	tasks[0].lambda = fun1;
	tasks[0].data = (void*)'s';
	tasks[0].stack_pos = (void*)0xa2000000;
	tasks[0].C = 1;
	tasks[0].T = PERIOD_DEV0;
	tasks[1].lambda = fun2;
	tasks[1].data = (void*)'u';
	tasks[1].stack_pos = (void*)0xa1000000;
	tasks[1].C = 1;
	tasks[1].T = PERIOD_DEV1;
	tasks[2].lambda = fun3;
	tasks[2].data = (void*)'c';
	tasks[2].stack_pos = (void*)0xa1200000;
	tasks[2].C = 1;
	tasks[2].T = PERIOD_DEV2;
	task_create(tasks, 3);
	// just to get rid of compiler...
	argc = argc;
	argv = argv;
	puts("WTF!\n");
	return 0;
}
