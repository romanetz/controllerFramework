#ifndef __STM32I2C_H__
#define __STM32I2C_H__

#include <stm32.h>
#include <stm32i2cregs.h>
#include <i2c.h>

#ifdef USE_CLASSES

typedef struct STM32I2CClass STM32I2CClass;

struct STM32I2CClass {
	I2CClass parent;
	volatile STM32I2CRegs *regs;
};

#define STM32_I2C_CLASS(obj) ((STM32I2CClass*)(obj))

STM32I2CClass *stm32_i2cClassInit(STM32I2CClass *i2c, volatile STM32I2CRegs *regs);

#endif

#endif