#include <stm32.h>

#ifdef USE_CLASSES

STM32I2CClass *stm32_i2cClassInit(STM32I2CClass *i2c, volatile STM32I2CRegs *regs) {
	i2c->regs = regs;
	return i2c;
}

#endif