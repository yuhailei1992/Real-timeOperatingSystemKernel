/*
 * C_IRQ_Handler.c: irq handler written in C
 *
 * Author: Hailei Yu <haileiy@andrew.cmu.edu>
 *         Hongyi Zhang <hongyiz@andrew.cmu.edu>
 *         Team #22
 * Date:   Fri, 07 Nov 2014 01:59
 */

#include <settings.h>
#include <exports.h>
#include <arm/reg.h>
#include <arm/timer.h>
#include <arm/interrupt.h>
#include <device.h>
#include <task.h>
#include <sched.h>

extern volatile unsigned long cnt;

void C_IRQ_Handler() {
    // increase cnt
    cnt++;
    // reset OSCR to 0
    reg_write(OSTMR_OSCR_ADDR, 0);
    // and set the OSSR's corresponding bit
    reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);

    dev_update(cnt*10);
	/* preemptive */
    if(highest_prio() < get_cur_prio()){
        dispatch_save();
    }
    return;
}

