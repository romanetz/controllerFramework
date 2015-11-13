#ifndef __NVIC_H__
#define __NVIC_H__

struct NVICRegisters {
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
};

#define NVICRegs MMIO(NVIC_BASE, NVICRegisters)

class NVIC {
	public:
		static bool isIRQEnabled(uint8_t index);
		
		static void enableIRQ(uint8_t index);
		
		static void disableIRQ(uint8_t index);
		
		static bool isIRQPending(uint8_t index);
		
		static void setPendingIRQ(uint8_t index);
		
		static void clearPendingIRQ(uint8_t index);
		
		static void setIRQPriority(uint8_t index, uint8_t priority);
		
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
		static bool isIRQActive(uint8_t index);
		
#endif
};

#endif
