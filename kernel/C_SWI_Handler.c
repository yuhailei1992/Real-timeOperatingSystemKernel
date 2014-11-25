/*
 * handler.c: read, write, exit, time, sleep handlers and handler dispatcher
 *
 * Author: Hailei Yu <haileiy@andrew.cmu.edu>
 *         Hongyi Zhang <hongyiz@andrew.cmu.edu>
 *         Team #22
 * Date:   Fri, 07 Nov 2014 01:55
 */

#include <exports.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <bits/swi.h>
#include <syscall.h>
#include <lock.h>
#include <sched.h>

//some constants for memory bound check
#define SDRAM_START	0xa0000000
#define SDRAM_END	0xa3ffffff
#define STRATAROM_START	0x00000000
#define STRATAROM_END	0X00ffffff
/*
 * Dispatcher. Decides which handler to invoke
 * regs[0] stores the return value
 */
void C_SWI_Handler(unsigned swi_num, unsigned *regs) {
    switch (swi_num) {
    case READ_SWI:
        regs[0] = read_syscall(regs[0], (void*)regs[1], regs[2]);
        break;
    case WRITE_SWI:
        regs[0] = write_syscall(regs[0], (const void*)regs[1], regs[2]);
        break;
    case TIME_SWI:
        regs[0] = time_syscall();
        break;
    case SLEEP_SWI:
        sleep_syscall(regs[0]);
        break;
    case CREATE_SWI:
        regs[0] = task_create((task_t*)regs[0], regs[1]);
        break;
    case EVENT_WAIT:
        regs[0] = event_wait(regs[0]);
        break;
    case MUTEX_CREATE:
        regs[0] = mutex_create();
        break;
    case MUTEX_LOCK:
        regs[0] = mutex_lock(regs[0]);
        break;
    case MUTEX_UNLOCK:
        regs[0] = mutex_unlock(regs[0]);
        break;
    default:
        invalid_syscall(swi_num);
    }
    return;
}

