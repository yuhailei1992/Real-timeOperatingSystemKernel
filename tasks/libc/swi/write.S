/** @file write.S
 *
 * @brief write sycall wrapper
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 07 Oct 2007 01:53:46 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-29
 */

#include <asm.h>
#include <bits/swi.h>

	.file "write.S"

FUNC(write)
	stmfd	sp!, {r1-r12, lr}	@store registers in stack
	swi	WRITE_SWI		@call WRITE_SWI
	cmp	r0, #0			@judge if there is an error
	bge	.DONE			@no error, done
	ldr	r1, =errno		@error occured
	mov	r2, #0		
	sub	r3, r2, r0		@compute the negative of return value
	str 	r3, [r1]		@set the global errno
	mov	r0, #-1			@set r0 to -1, indicating an error
.DONE:
	ldmfd	sp!, {r1-r12, lr}	@restore general purpose registers
	mov	pc, lr

