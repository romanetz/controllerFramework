#ifndef __ARM_VECTORS_H__
#define __ARM_VECTORS_H__

#define ISR(name) void name(void)

#define SYSTICK_IRQ -1

#define PENDSV_IRQ -2

#define DEBUG_MONITOR_IRQ -4

#define SV_CALL_IRQ -5

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

#define USAGE_FAULT_IRQ -10

#define BUS_FAULT_IRQ -11

#define MEM_MANAGE_IRQ -12

#endif

#define HARD_FAULT_IRQ -13

#define NMI_IRQ -14

#endif
 
