/** @file time.c
 *
 * @brief Kernel timer based syscall implementations
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 */

#include <types.h>
#include <config.h>
#include <bits/errno.h>
#include <arm/timer.h>
#include <syscall.h>
#include <exports.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>

extern volatile unsigned long cnt;

unsigned long time_syscall(void)
{
	enable_interrupts();
 	return cnt * 10;
}



/** @brief Waits in a tight loop for atleast the given number of milliseconds.
 *
 * @param millis  The number of milliseconds to sleep.
 *
 *
 */
void sleep_syscall(unsigned long millis  __attribute__((unused)))
{
	enable_interrupts();
	unsigned long target = (unsigned long)cnt + (millis/10);
    	while (cnt < target) {
        	//do nothing
    	}
}
