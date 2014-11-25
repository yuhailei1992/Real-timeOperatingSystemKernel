/** @file main.c
 *
 * @brief kernel main
 *
 * @author Hailei Yu <haileiy@andrew.cmu.edu>
 *         Hongyi Zhang <hongyiz@andrew.cmu.edu>
 *         Team #22
 * @date  2014-11-22
 */

#include <kernel.h>
#include <task.h>
#include <lock.h>
#include <sched.h>
#include <device.h>
#include <assert.h>
#include <exports.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/interrupt.h>
#include <arm/timer.h>
#include <bits/swi.h>
#include <settings.h>
#include <irq_env.h>

#define	VECTOR_CONTENT_INST_MASK 0xfffff000
#define	SWI_VECTOR_ADDR 0x8
#define IRQ_VECTOR_ADDR 0x18
#define	SWI_VECTOR_PC_PLUS 0xe59ff000
#define	SWI_VECTOR_PC_MINUS 0xe51ff000
#define	PC_OFFSET 0X8

// set_user_env saves svc registers, prepare user stack, pass arguments, and
// jump to the user program
extern void set_user_env(int argc, char *argv[]);

// S_Handler is a reentrant handler
extern void S_SWI_Handler();
//irq_wrapper provided by professor
extern void irq_wrapper();

// global variables for storing instructions, svc_sp, and actual handler addr
unsigned svc_sp;
// The following global variables store the original handler instructions of irq and swi handler
unsigned orig_inst0_swi = 0;
unsigned orig_inst1_swi = 0;

unsigned orig_inst0_irq = 0;
unsigned orig_inst1_irq = 0;

unsigned r8_table;

//actual_addr is the address where we can install our handler
volatile unsigned long cnt = 0;

/*
 * Install_Handler fetch the interrupt vector, parse it, and find the address
 * to install our handler
 * params: vector_addr is the vector address in VECTOR table. For swi, it is 0x08;
 * for irq, it is 0x
 */
unsigned Install_Handler (unsigned vector_addr, unsigned new_handler_addr) {
    //get the content at vector_addr
    unsigned vector_content = *(unsigned *)vector_addr;
    //mask the immediate number, get the instruction of vector_content
    unsigned vector_content_inst = vector_content & VECTOR_CONTENT_INST_MASK;
    //judge if the instruction is valid
    if (vector_content_inst != SWI_VECTOR_PC_PLUS &&
            vector_content_inst != SWI_VECTOR_PC_MINUS) {
        printf(">>The instruction is unrecognized\n");
        return 0x0badc0de;
    }
    //find the address of handler
    int offset = vector_content & ~VECTOR_CONTENT_INST_MASK;
    //if the offset is negative
    if (vector_content_inst == SWI_VECTOR_PC_MINUS) {
        offset = -offset;
    }
    //vector table addr + pc offset + offset
    unsigned interim_addr = *((unsigned *)(vector_addr + PC_OFFSET + offset));
    //get the actual_addr, where we install our handler
    unsigned *actual_addr = (unsigned *)interim_addr;
    //backup the original handler in global variables
    if (vector_addr == SWI_VECTOR_ADDR) {
        orig_inst0_swi = *actual_addr;
        orig_inst1_swi = *(actual_addr+1);
    }
    else if (vector_addr == IRQ_VECTOR_ADDR) {
        orig_inst0_irq = *actual_addr;
        orig_inst1_irq = *(actual_addr+1);
    }
    unsigned orig_inst1 = *(actual_addr+1);
    //then set the instructions!
    //ldr pc, [pc, #-4];
    *actual_addr = 0xe51ff004;
    //set the next 4bytes to the address of our handler
    *(actual_addr+1) = (unsigned)new_handler_addr;
    //check if we have successfully installed the handler
    if (*(actual_addr+1) == orig_inst1) {
        printf(">>Failed to install our handlers...\n");
    }
    else {
        if (vector_addr == SWI_VECTOR_ADDR) {
            printf(">>Installing SWI handler...Done\n");
        }
        else if (vector_addr == IRQ_VECTOR_ADDR) {
            printf(">>Installing IRQ handler...Done\n");
        }
        else {
            printf(">>Installing unknown handler...Done\n");
        }
    }
    return 0;
}
/*
 * Restore_Handler is used to restore the hijacked handler
 */
void Restore_Handler (unsigned vector_addr, unsigned new_handler_addr) {
    //0x8 is the address of interrupt vector
    unsigned *old_int_vec = (unsigned *)vector_addr;
    //get the content at 0x8
    unsigned vector_content = *old_int_vec;
    //mask the immediate number, get the instruction of vector_content
    unsigned vector_content_inst = vector_content & VECTOR_CONTENT_INST_MASK;
    //judge if the instruction is valid
    if (vector_content_inst != SWI_VECTOR_PC_PLUS &&
            vector_content_inst != SWI_VECTOR_PC_MINUS) {
        printf(">>The instruction is unrecognized\n");
    }
    //find the address of SWI handler
    int offset = vector_content & ~VECTOR_CONTENT_INST_MASK;
    //if the offset is negative
    if (vector_content_inst == SWI_VECTOR_PC_MINUS) {
        offset = -offset;
    }
    //vector table addr + pc offset + offset
    unsigned temp_addr = *((unsigned*)(vector_addr + PC_OFFSET + offset)); //haileiy
    //get the actual_addr, where we install our handler
    unsigned *actual_addr = (unsigned *)temp_addr;
    //restore original instruction from orig_inst0 and orig_inst1

    if (vector_addr == SWI_VECTOR_ADDR) {
        *actual_addr = orig_inst0_swi;
        *(actual_addr+1) = orig_inst1_swi;
    }
    else if (vector_addr == IRQ_VECTOR_ADDR) {
        *actual_addr = orig_inst0_irq;
        *(actual_addr+1) = orig_inst1_irq;
    }
    //check if we have successfully restored the handler
    if (*(actual_addr+1) == (unsigned)new_handler_addr) {
        printf(">>Failed to recover original handlers...\n");
    }
    else {
        printf(">>Recovering original handlers...Done\n");
    }
}



uint32_t global_data;

int kmain(int argc __attribute__((unused)), char** argv  __attribute__((unused)), uint32_t table)
{

    app_startup();
    global_data = table;
    /* add your code up to assert statement */
    printf(">>Starting kernel...\n");
    // install swi and irq handler
    Install_Handler(SWI_VECTOR_ADDR, (unsigned)&S_SWI_Handler);
    Install_Handler(IRQ_VECTOR_ADDR, (unsigned)&irq_wrapper);
    // initialize devices, sched and mutexes
    dev_init();
    sched_init(0);
    mutex_init();
    // setup the environment for irq
    set_irq_env();
    // setup the environment for usermode and go to user program
    set_user_env(argc, argv);
    // after returning from user program, recover handlers
    Restore_Handler(SWI_VECTOR_ADDR, (unsigned)&S_SWI_Handler);
    Restore_Handler(IRQ_VECTOR_ADDR, (unsigned)&irq_wrapper);
    // restore_irq_environment
    restore_irq_env();
    printf(">>Exitting kernel...Goodbye\n");
    return 0;
    //assert(0);        /* should never get here */
}
