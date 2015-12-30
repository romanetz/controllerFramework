#ifndef __SCB_H__
#define __SCB_H__

#include <arm-cm.h>

typedef struct SCBRegs {
	uint32_t CPUID;
	uint32_t ICSR;
	uint32_t VTOR;
	uint32_t AIRCR;
	uint32_t SCR;
	uint32_t CCR;
	uint8_t SHPR[12];
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
	uint32_t SHCSR;
	uint32_t CFSR;
	uint32_t HFSR;
	uint32_t DFSR;
	uint32_t MMFAR;
	uint32_t BFAR;
	uint32_t AFSR;
	uint32_t ID_PFR0;
	uint32_t ID_PFR1;
	uint32_t ID_DFR0;
	uint32_t ID_AFR0;
	uint32_t ID_MMFR0;
	uint32_t ID_MMFR1;
	uint32_t ID_MMFR2;
	uint32_t ID_MMFR3;
	uint32_t ID_ISAR0;
	uint32_t ID_ISAR1;
	uint32_t ID_ISAR2;
	uint32_t ID_ISAR3;
	uint32_t ID_ISAR4;
	uint32_t RESERVED1[5];
	uint32_t CPACR;
	uint32_t RESERVED2[108];
	uint32_t FPCCR;
	uint32_t FPCAR;
	uint32_t FPDSCR;
	uint32_t MVFR0;
	uint32_t MVFR1;
#endif
} SCBRegs;

#define SCB_BASE (SCS_BASE + 0x0D00)
#define SCB MMIO(SCB_BASE, SCBRegs)

#define SCB_CCR_STKALIGN (1 << 9)

/* SCB->AIRCR */
#define SCB_AIRCR_VECTKEYSTAT_OFFSET 16
#define SCB_AIRCR_VECTKEYSTAT (0xFFFF << SCB_AIRCR_VECTKEYSTAT_OFFSET)
#define SCB_AIRCR_VECTKEY (0x05FA << SCB_AIRCR_VECTKEYSTAT_OFFSET)

#define SCB_AIRCR_ENDIANESS (1 << 15)

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
#define SCB_AIRCR_PRIGROUP_GROUP16_NOSUB (0x3 << 8)
#define SCB_AIRCR_PRIGROUP_GROUP8_SUB2 (0x4 << 8)
#define SCB_AIRCR_PRIGROUP_GROUP4_SUB4 (0x5 << 8)
#define SCB_AIRCR_PRIGROUP_GROUP2_SUB8 (0x6 << 8)
#define SCB_AIRCR_PRIGROUP_NOGROUP_SUB16 (0x7 << 8)
#define SCB_AIRCR_PRIGROUP_MASK (0x7 << 8)
#define SCB_AIRCR_PRIGROUP_SHIFT 8
#endif

#define SCB_AIRCR_SYSRESETREQ (1 << 2)
#define SCB_AIRCR_VECTCLRACTIVE (1 << 1)

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
#define SCB_AIRCR_VECTRESET (1 << 0)
#endif

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

#define SCB_CPACR_NONE 0 /* Access denied */
#define SCB_CPACR_PRIV 1 /* Privileged access only */
#define SCB_CPACR_FULL 3 /* Full access */

#define SCB_CPACR_CP10 (1 << 20)
#define SCB_CPACR_CP11 (1 << 22)

#endif

void SCB_ResetSystem(void);

#endif