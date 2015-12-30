#ifndef __ARM_NVIC_H__
#define __ARM_NVIC_H__

typedef struct NVICRegs {
	uint32_t ICTR;
	uint32_t RESERVED0[63];
	uint32_t ISER[8];
	uint32_t RESERVED1[24];
	uint32_t ICER[8];
	uint32_t RESERVED2[24];
	uint32_t ISPR[8];
	uint32_t RESERVED3[24];
	uint32_t ICPR[8];
	uint32_t RESERVED4[24];
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
	uint32_t IABR[8];
	uint32_t RESERVED5[56];
#else
	uint32_t RESERVED5[64];
#endif
	uint8_t IPR[240];
} NVICRegs;

#define NVIC_REGS MMIO(NVIC_BASE, NVICRegs)

void nvicEnableIRQ(uint8_t index);
void nvicDisableIRQ(uint8_t index);
uint8_t nvicIRQEnabled(uint8_t index);
uint8_t nvicIRQPending(uint8_t index);
void nvicSetIRQPending(uint8_t index);
void nvicClearIRQPending(uint8_t index);
void nvicSetIRQPriority(uint8_t index, uint8_t priority);

#endif
