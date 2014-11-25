@ set_user_env.S: set user mode environment
@
@ Author: Hailei Yu <haileiy@andrew.cmu.edu>
@         Hongyi Zhang <hongyiz@andrew.cmu.edu>
@         Team #22
@ Date:   Fri, 07 Nov 2014 01:42
.file	"set_user_env.S"
.text
.extern r8_table
.global	set_user_env
#include <bits/swi.h>

set_user_env:
    
    STMFD sp!, {r1-r12, lr}			@store registers in svc mode

    MRS r3, cpsr					@modify the cpsr register and save sp
    STMFD sp!, {r3}
	
    LDR	r1, =r8_table			@save r8_table
    STR r8, [r1]

    LDR	r4, =svc_sp					@store the svc_sp in a global variable
    STR	sp, [r4]
	
    BIC r3, r3, #0x5f				@mask F, enable I, set user mode bits
    ORR r3, r3, #0x50	
    MSR cpsr, r3
    
    MOV r3, #0xa3000000				@set up user mode stack
    MOV sp, r3

    MOV r3, r0						@set argc and argv on user stack
    MOV r4, #0						@NULL on the top of argv
    STMFD sp!, {r4}

.prep_stack:
    SUBS r3, r3, #1					@judge if we have pushed all the parameters
    BLT .done
    ADD r2, r1, r3, LSL #2 			@get the address of argv[i]
    LDR r4, [r2]					@load the argv.
    STMFD sp!, {r4}					@store argv[i] to the user stack
    B .prep_stack

.done:
    STMFD sp!, {r0}					@at last, store argc on the stack

    LDR r2, =0xa0000000				@jump to user program
    BLX r2					
    SWI EXIT_SWI
