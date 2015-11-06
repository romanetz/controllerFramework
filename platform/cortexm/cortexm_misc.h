/**
	\addtogroup Cortex-Mx Cortex-Mx
	@{
*/
#ifndef __CORTEXM_MISC_H__
#define __CORTEXM_MISC_H__

// NVIC: Nested Vector Interrupt Controller
//#define NVIC_BASE (SCS_BASE + 0x0100)

// MPU: Memory protection unit
#define MPU_BASE (SCS_BASE + 0x0D90)

/* Defined only for Cortex-M0 */
#if defined(__ARM_ARCH_6M__)

// DEBUG: Debug control and configuration
#define DEBUG_BASE                      (SCS_BASE + 0x0DF0)

#endif

/* Defined only for ARMv7 and above */
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

// ITM: Instrumentation Trace Macrocell
#define ITM_BASE (PPBI_BASE + 0x0000)

// DWT: Data Watchpoint and Trace unit
#define DWT_BASE (PPBI_BASE + 0x1000)

// FPB: Flash Patch and Breakpoint unit
#define FPB_BASE (PPBI_BASE + 0x2000)

#define TPIU_BASE (PPBI_BASE + 0x40000)

// ITR: Interrupt Type Register
#define ITR_BASE (SCS_BASE + 0x0000)

// STE: Software Trigger Interrupt Register
#define STIR_BASE (SCS_BASE + 0x0F00)

// ID: ID space
#define ID_BASE (SCS_BASE + 0x0FD0)

#endif

#endif
/* @} */
