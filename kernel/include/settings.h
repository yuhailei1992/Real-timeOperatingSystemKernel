/*
 *
 * Author: Hongyi Zhang <hongyiz@andrew.cmu.edu>
 * Date:   0:41 Nov 2, 2014
 *
 */

#define IRQ_STACK_SIZE 100
//mask all interrupts except OSMR0
#define ICMR_VALUE 1 << 26
//set OSMR0 as irq
#define ICLR_MASK 0x4000000

#define OIER_MASK 0xf

#define TIME_INTERVAL 32500

#define HEAP_LOW_BOUND 0xa3edf000
#define HEAP_HIGH_BOUND 0xa3edefff


