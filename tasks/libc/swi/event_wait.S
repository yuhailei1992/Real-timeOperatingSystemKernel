/** @file event_wait.S
 *
 * @brief time sycall wrapper
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-31
 */

#include <asm.h>
#include <bits/swi.h>

	.file "event_wait.S"


FUNC(event_wait)
	stmfd	sp!, {r1-r12, lr}	@store registers in stack
	swi	EVENT_WAIT		@call EVENT_WAIT
	cmp	r0, #0			@judge if there is an error
	bge	.DONE			@no error, jump to .DONE
	ldr	r1, =errno		@there is error, load address of errno to r1
	@mov	r2, #0			@get the negative of exit status
	@sub	r3, r2, r0
	mov	r3, #-1
	mul	r3, r0, r3
	str	r3, [r1]		@store the negative of exit status in errno
	mov	r0, #-1			@set r0 to -1, indicating an error
.DONE:
	ldmfd	sp!, {r1-r12, lr}	@restore registers
	mov	pc, lr
