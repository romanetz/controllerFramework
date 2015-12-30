#ifndef __STM32USART_H__
#define __STM32USART_H__

#include <stm32.h>
#include <stm32usartregs.h>
#include <serialport.h>
#include <queue.h>

void stm32_usartEnable(volatile STM32USARTRegs *usart);
void stm32_usartDisable(volatile STM32USARTRegs *usart);

void stm32_usartSetBaudrate(volatile STM32USARTRegs *usart, uint32_t baudrate);
void stm32_usartSetFlowControl(volatile STM32USARTRegs *usart, SerialPortFlowControl flowControl);
void stm32_usartSetParity(volatile STM32USARTRegs *usart, SerialPortParity parity);
void stm32_usartSetDataBits(volatile STM32USARTRegs *usart, SerialPortDataBits bits);
void stm32_usartSetStopBits(volatile STM32USARTRegs *usart, SerialPortStopBits bits);

uint8_t stm32_usartReadChar(volatile STM32USARTRegs *usart);
void stm32_usartWriteChar(volatile STM32USARTRegs *usart, uint8_t chr);

#ifdef USE_CLASSES

typedef struct STM32USARTClass STM32USARTClass;

struct STM32USARTClass {
	SerialPortClass parent;
	volatile STM32USARTRegs *usart;
	QueueClass txQueue;
	QueueClass rxQueue;
};

#define STM32_USART_CLASS(obj) ((STM32USARTClass*)(obj))

STM32USARTClass *stm32_usartClassInit(STM32USARTClass *usart, volatile STM32USARTRegs *usartRegs, int txQueueSize, int rxQueueSize);

#endif

#endif
