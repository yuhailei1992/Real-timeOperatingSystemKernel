ARM_OBJS := reg.o psr.o int_asm.o C_IRQ_Handler.o
ARM_OBJS := $(ARM_OBJS:%=$(KDIR)/arm/%)

KOBJS += $(ARM_OBJS)
