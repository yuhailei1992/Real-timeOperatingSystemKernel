/* set_irq_env: set irq mode environment
 *
 * Author: Hailei Yu <haileiy@andrew.cmu.edu>
 *         Hongyi Zhang <hongyiz@andrew.cmu.edu>
 *         Team #22
 * Date:   Fri, 07 Nov 2014 01:55
 */

#include <settings.h>
#include <exports.h>
#include <arm/interrupt.h>
#include <arm/reg.h>
#include <arm/timer.h>

extern unsigned cnt;
extern void enable_irq();

unsigned old_ICLR;
unsigned old_ICMR;
unsigned old_OSMR0;
unsigned old_OIER;

void set_IC_regs();
void restore_IC_regs();
void set_timer_regs();
void restore_timer_regs();


/*
 * store old ICMR, ICLR values, set ICMR, ICLR to appropriate values
 */
void set_IC_regs() {
    //store ICMR and mask all interrupts except OSMR0
    old_ICMR = reg_read(INT_ICMR_ADDR);
    reg_set(INT_ICMR_ADDR, 1 << INT_OSTMR_0);
    //store ICLR value and set bit 26 to 0 to set
    //OSMR0 to irq
    old_ICLR = reg_read(INT_ICLR_ADDR);
    reg_clear(INT_ICLR_ADDR, 1 << INT_OSTMR_0);

    printf(">>Setting IC registers...Done\n");
}

/*
 * store old OIER, OSMR values, set OIER, OSCR, OSMR to appropriate values
 */
void set_timer_regs() {
    // store old OIER and OSMR values
    old_OIER = reg_read(OSTMR_OIER_ADDR);
    old_OSMR0 = reg_read(OSTMR_OSMR_ADDR(0));
    // set OSMR to 32500, set OSCR to 0
    reg_write(OSTMR_OSMR_ADDR(0), 32500);
    reg_write(OSTMR_OSCR_ADDR, 0);

    reg_clear(OSTMR_OIER_ADDR,
              OSTMR_OIER_E1 | OSTMR_OIER_E2 | OSTMR_OIER_E3);
    reg_set(OSTMR_OIER_ADDR, OSTMR_OIER_E0);
    printf(">>Setting timer registers...Done\n");
}

/*
 * restore ICMR, ICLR
 */
void restore_IC_regs() {
    reg_write(INT_ICMR_ADDR, old_ICMR);
    reg_write(INT_ICLR_ADDR, old_ICLR);
}

/*
 * restore OIER, OSMR
 */
void restore_timer_regs() {
    reg_write(OSTMR_OIER_ADDR, old_OIER);
    reg_write(OSTMR_OSMR_ADDR(0), old_OSMR0);
}

/*
 * set irq environment. This function includes setting irq_sp, IC registers,
 * and timer registers
 */
int set_irq_env() {
    set_IC_regs();
    set_timer_regs();
    return 1;
}
/*
 * restore irq environment. This function includes restoring IC registers,
 * and timer registers
 */
int restore_irq_env() {
    restore_IC_regs();
    restore_timer_regs();
    printf(">>Restoring regs...Done\n");
    return 1;
}

