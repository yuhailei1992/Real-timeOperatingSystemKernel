@ S_SWI_Handler.S: Software Interrupt handler in assembly
@
@ Author: Hailei Yu <haileiy@andrew.cmu.edu>
@         Hongyi Zhang <hongyiz@andrew.cmu.edu>
@         Team #22
@ Date:   Fri, 07 Nov 2014 01:40

.global S_SWI_Handler
.extern print_reg

S_SWI_Handler:
	
	SUB	sp, sp, #4				@store registers
	STMFD	sp!, {r0-r12, lr}	

	MRS	r2, spsr				@store spsr

	STR	r2, [sp, #14*4]
	
	LDR	r2, =r8_table			@restore r8
    LDR	r8, [r2]

	MOV	r1, sp
	
	LDR	r0, [lr, #-4]			@get swi number
	BIC	r0, r0, #0xff000000

	MRS	r2, cpsr				@disable interrupts
	ORR	r2, r2, #0x80
	MSR	cpsr, r2

	STMFD	sp, {sp, lr}^
	ADD	sp, sp, #-8

	BL	C_SWI_Handler			@go to C_SWI_Handler

	LDMFD	sp, {sp, lr}^
	ADD	sp, sp, #8

	LDR	r2, [sp, #14*4]			@restore registers
	MSR	spsr, r2
	LDMFD	sp!, {r0-r12, lr}
	ADD	sp, sp, #4
	MOVS	pc, lr
