.file	"enable_irq.S"
.text
.global	enable_irq

enable_irq:
    STMFD sp!, {r1,r2}			@store registers in svc mode
    
    MRS r1, cpsr				@enable irq
    BIC r1, r1, #0x80
    MSR cpsr, r1

    LDMFD	sp!, {r1,r2}		@ restore r1,r2   
    MOV	pc, lr					@ go back to kernel

