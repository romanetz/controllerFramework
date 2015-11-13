#include <cortexm.h>

bool NVIC::isIRQEnabled(uint8_t index) {
	return (NVICRegs->ISER[index / 32] & (1 << (index % 32))) != 0;
}

void NVIC::enableIRQ(uint8_t index) {
	NVICRegs->ISER[index / 32] = 1 << (index % 32);
}

void NVIC::disableIRQ(uint8_t index) {
	NVICRegs->ICER[index / 32] = 1 << (index % 32);
}

bool NVIC::isIRQPending(uint8_t index) {
	return (NVICRegs->ISPR[index / 32] & (1 << (index % 32))) == 0;
}

void NVIC::setPendingIRQ(uint8_t index) {
	NVICRegs->ISPR[index / 32] = 1 << (index % 32);
}

void NVIC::clearPendingIRQ(uint8_t index) {
	NVICRegs->ICPR[index / 32] = 1 << (index % 32);
}

void setIRQPriority(uint8_t index, uint8_t priority) {
	NVICRegs->IPR[index] = priority;
}

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
bool NVIC::isIRQActive(uint8_t index) {
	return (NVICRegs->IABR[index / 32] & (1 << (index % 32))) != 0;
}
#endif
