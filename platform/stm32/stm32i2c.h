#ifndef __STM32I2C_H__
#define __STM32I2C_H__

#include <stm32.h>
#include <stm32i2cregs.h>
#include <i2c.h>

void stm32_i2cEnable(volatile STM32I2CRegs *i2c);
void stm32_i2cDisable(volatile STM32I2CRegs *i2c);

void stm32_i2cSetFrequency(volatile STM32I2CRegs *i2c, uint32_t frequency);
BOOL stm32_i2cBusy(volatile STM32I2CRegs *i2c);
BOOL stm32_i2cWaitReady(volatile STM32I2CRegs *i2c, timestamp_t timeout);
BOOL stm32_i2cWaitFlag(volatile STM32I2CRegs *i2c, uint32_t flag, timestamp_t timeout);
void stm32_i2cEnableAck(volatile STM32I2CRegs *i2c);
void stm32_i2cDisableAck(volatile STM32I2CRegs *i2c);
BOOL stm32_i2cMasterStart(volatile STM32I2CRegs *i2c, timestamp_t timeout);
BOOL stm32_i2cMasterSendAddress(volatile STM32I2CRegs *i2c, uint8_t address, BOOL oneByteRead, timestamp_t timeout);
BOOL stm32_i2cMasterSendByte(volatile STM32I2CRegs *i2c, uint8_t data, timestamp_t timeout);
int stm32_i2cMasterSendBytes(volatile STM32I2CRegs *i2c, const uint8_t *data, int count, timestamp_t timeout);
BOOL stm32_i2cMasterRecvByte(volatile STM32I2CRegs *i2c, uint8_t *buffer, BOOL lastByte, timestamp_t timeout);
int stm32_i2cMasterRecvBytes(volatile STM32I2CRegs *i2c, uint8_t *buffer, int count, timestamp_t timeout);
BOOL stm32_i2cMasterTransfer(volatile STM32I2CRegs *i2c, uint8_t address, uint8_t *buffer, int count, timestamp_t timeout);
void stm32_i2cMasterStop(volatile STM32I2CRegs *i2c);

#ifdef USE_CLASSES

typedef struct STM32I2CClass STM32I2CClass;

struct STM32I2CClass {
	I2CClass parent;
	volatile STM32I2CRegs *regs;
	uint32_t frequency;
};

#define STM32_I2C_CLASS(obj) ((STM32I2CClass*)(obj))

STM32I2CClass *stm32_i2cClassInit(STM32I2CClass *i2c, volatile STM32I2CRegs *regs, uint32_t frequency);

#endif

#endif