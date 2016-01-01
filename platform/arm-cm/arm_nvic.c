#include <arm-cm.h>

BOOL nvicIRQEnabled(uint8_t index) {
	return (NVIC_REGS->ISER[index / 32] & (1 << (index % 32))) != 0;
}

void nvicEnableIRQ(uint8_t index) {
	NVIC_REGS->ISER[index / 32] = 1 << (index % 32);
}

void nvicDisableIRQ(uint8_t index) {
	NVIC_REGS->ICER[index / 32] = 1 << (index % 32);
}

BOOL nvicIRQPending(uint8_t index) {
	return (NVIC_REGS->ISPR[index / 32] & (1 << (index % 32))) == 0;
}

void nvicSetIRQPending(uint8_t index) {
	NVIC_REGS->ISPR[index / 32] = 1 << (index % 32);
}

void nvicClearIRQPending(uint8_t index) {
	NVIC_REGS->ICPR[index / 32] = 1 << (index % 32);
}

void nvicSetIRQPriority(uint8_t index, uint8_t priority) {
	NVIC_REGS->IPR[index] = priority;
}
 
