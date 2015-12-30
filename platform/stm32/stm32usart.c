#include <stm32.h>

#ifdef USE_CLASSES

static STM32USARTClass *usart1;
static STM32USARTClass *usart2;
static STM32USARTClass *usart3;
#ifdef UART4_BASE
static STM32USARTClass *uart4;
#endif
#ifdef UART5_BASE
static STM32USARTClass *uart5;
#endif
#ifdef USART6_BASE
static STM32USARTClass *usart6;
#endif

#endif

static uint32_t stm32_usartClockFrequency(volatile STM32USARTRegs *usart) {
	if (usart == USART1) {
		return stm32_rccAPB2Frequency();
	}
#ifdef USART6_BASE
	if (usart == USART6) {
		return stm32_rccAPB2Frequency();
	}
#endif
	return stm32_rccAPB1Frequency();
}

static uint8_t stm32_usartIRQ(volatile STM32USARTRegs *usart) {
	switch ((uint32_t)usart) {
		case USART1_BASE:
			return USART1_IRQ;
		case USART2_BASE:
			return USART2_IRQ;
		case USART3_BASE:
			return USART3_IRQ;
#ifdef UART4_BASE
		case UART4_BASE:
			return UART4_IRQ;
#endif
#ifdef UART5_BASE
		case UART5_BASE:
			return UART4_IRQ;
#endif
#ifdef USART6_BASE
		case USART6_BASE:
			return USART6_IRQ;
#endif
		default:
			return -1;
	}
}

void stm32_usartEnable(volatile STM32USARTRegs *usart) {
	stm32_rccPeriphClockEnable(usart);
	usart->CR1 |= USART1_CR1_UE | USART1_CR1_TE | USART1_CR1_RE;
	nvicEnableIRQ(stm32_usartIRQ(usart));
}

void stm32_usartDisable(volatile STM32USARTRegs *usart) {
	stm32_rccPeriphClockDisable(usart);
}

void stm32_usartSetBaudrate(volatile STM32USARTRegs *usart, uint32_t baudrate) {
	uint32_t clock = stm32_usartClockFrequency(usart);
	uint32_t brr = (2 * clock + baudrate) / (2 * baudrate);
	usart->BRR = brr;
}

void stm32_usartSetFlowControl(volatile STM32USARTRegs *usart, SerialPortFlowControl flowControl) {
	uint32_t cfg;
	switch (flowControl) {
		case SERIALPORT_FLOWCONTROL_RTS:
			cfg = USART1_CR3_RTSE;
			break;
		case SERIALPORT_FLOWCONTROL_CTS:
			cfg = USART1_CR3_CTSE;
			break;
		case SERIALPORT_FLOWCONTROL_RTS_CTS:
			cfg = USART1_CR3_RTSE | USART1_CR3_CTSE;
		default:
			cfg = 0;
	}
	usart->CR3 = (usart->CR3 & ~(USART1_CR3_RTSE | USART1_CR3_CTSE)) | cfg;
}

void stm32_usartSetParity(volatile STM32USARTRegs *usart, SerialPortParity parity) {
	uint32_t value = 0;
	switch (parity) {
		case SERIALPORT_PARITY_ODD:
			value |= USART1_CR1_PS;
		case SERIALPORT_PARITY_EVEN:
			value |= USART1_CR1_PCE;
			break;
		default:
			value = 0;
	}
	usart->CR1 = (usart->CR1 & ~(USART1_CR1_PCE | USART1_CR1_PS)) | value;
}

void stm32_usartSetDataBits(volatile STM32USARTRegs *usart, SerialPortDataBits bits) {
	if (bits == SERIALPORT_DATA_9BIT) {
		usart->CR1 |= USART1_CR1_M;
	} else {
		usart->CR1 &= ~USART1_CR1_M;
	}
}

void stm32_usartSetStopBits(volatile STM32USARTRegs *usart, SerialPortStopBits bits) {
	uint32_t value = 0;
	switch (bits) {
		case SERIALPORT_STOP_1BIT:
			value = 0;
			break;
		case SERIALPORT_STOP_0_5BIT:
			value = 1;
			break;
		case SERIALPORT_STOP_2BIT:
			value = 2;
			break;
		case SERIALPORT_STOP_1_5BIT:
			value = 3;
			break;
	}
	usart->CR2 = (usart->CR2 & ~USART1_CR2_STOP_MASK) | (value << USART1_CR2_STOP_OFFSET);
}

uint8_t stm32_usartReadChar(volatile STM32USARTRegs *usart) {
	return usart->DR;
}

void stm32_usartWriteChar(volatile STM32USARTRegs *usart, uint8_t chr) {
	usart->DR = chr;
}

#ifdef USE_CLASSES

static STM32USARTClass **stm32_usartObject(volatile STM32USARTRegs* usart) {
	switch ((uint32_t)usart) {
		case USART1_BASE:
			return &usart1;
		case USART2_BASE:
			return &usart2;
		case USART3_BASE:
			return &usart3;
#ifdef UART4_BASE
		case UART4_BASE:
			return &uart4;
#endif
#ifdef UART5_BASE
		case UART5_BASE:
			return &uart5;
#endif
#ifdef USART6_BASE
		case USART6_BASE:
			return &usart6;
#endif
		default:
			return 0;
	}
}

void STM32USARTClass_setBaudrate(SerialPortClass *serialPort, uint32_t baudrate) {
	stm32_usartSetBaudrate(STM32_USART_CLASS(serialPort)->usart, baudrate);
}

void STM32USARTClass_setFlowControl(SerialPortClass *serialPort, SerialPortFlowControl flowControl) {
	stm32_usartSetFlowControl(STM32_USART_CLASS(serialPort)->usart, flowControl);
}

void STM32USARTClass_setParity(SerialPortClass *serialPort, SerialPortParity parity) {
	stm32_usartSetParity(STM32_USART_CLASS(serialPort)->usart, parity);
}

void STM32USARTClass_setDataBits(SerialPortClass *serialPort, SerialPortDataBits bits) {
	stm32_usartSetDataBits(STM32_USART_CLASS(serialPort)->usart, bits);
}

void STM32USARTClass_setStopBits(SerialPortClass *serialPort, SerialPortStopBits bits) {
	stm32_usartSetStopBits(STM32_USART_CLASS(serialPort)->usart, bits);
}

int STM32USARTClass_writeTimeout(IOStreamClass *stream, const void *data, int len, uint64_t timeout) {
	int r = queueWriteTimeout(&STM32_USART_CLASS(stream)->txQueue, data, len, timeout);
	STM32_USART_CLASS(stream)->usart->CR1 |= USART1_CR1_TXEIE;
	return r;
}

int STM32USARTClass_readTimeout(IOStreamClass *stream, void *buffer, int len, uint64_t timeout) {
	return queueReadTimeout(&STM32_USART_CLASS(stream)->rxQueue, buffer, len, timeout);
}

void STM32USARTClass_flush(IOStreamClass *stream) {
	queueClear(&STM32_USART_CLASS(stream)->txQueue);
	queueClear(&STM32_USART_CLASS(stream)->rxQueue);
}

void STM32USARTClass_close(IOStreamClass *stream) {
	stm32_usartDisable(STM32_USART_CLASS(stream)->usart);
	STM32USARTClass **obj = stm32_usartObject(STM32_USART_CLASS(stream)->usart);
	if (obj) *obj = 0;
}

STM32USARTClass *stm32_usartClassInit(STM32USARTClass *usart, volatile STM32USARTRegs *usartRegs, int txQueueSize, int rxQueueSize) {
	IO_STREAM_CLASS(usart)->writeTimeout = STM32USARTClass_writeTimeout;
	IO_STREAM_CLASS(usart)->readTimeout = STM32USARTClass_readTimeout;
	IO_STREAM_CLASS(usart)->flush = STM32USARTClass_flush;
	IO_STREAM_CLASS(usart)->close = STM32USARTClass_close;
	SERIAL_PORT_CLASS(usart)->setBaudrate = STM32USARTClass_setBaudrate;
	SERIAL_PORT_CLASS(usart)->setFlowControl = STM32USARTClass_setFlowControl;
	SERIAL_PORT_CLASS(usart)->setParity = STM32USARTClass_setParity;
	SERIAL_PORT_CLASS(usart)->setDataBits = STM32USARTClass_setDataBits;
	SERIAL_PORT_CLASS(usart)->setStopBits = STM32USARTClass_setStopBits;
	usart->usart = usartRegs;
	queueInit(&usart->txQueue, sizeof(char), txQueueSize);
	queueInit(&usart->rxQueue, sizeof(char), rxQueueSize);
	STM32USARTClass **obj = stm32_usartObject(usartRegs);
	if (obj) *obj = usart;
	stm32_usartEnable(usartRegs);
	usart->usart->CR1 |= USART1_CR1_RXNEIE;
	return usart;
}

static void stm32_usartInterruptHandler(STM32USARTClass *usart) {
	if (!usart) return;
	uint32_t sr = usart->usart->SR;
	if (sr & USART1_SR_RXNE) {
		uint8_t data = stm32_usartReadChar(usart->usart);
		queueWriteTimeout(&usart->rxQueue, (const char*)&data, sizeof(data), 0);
	}
	if (sr & USART1_SR_TXE) {
		uint8_t data;
		if (queueReadTimeout(&usart->txQueue, (char*)&data, sizeof(data), 0) >= (int)sizeof(data)) {
			stm32_usartWriteChar(usart->usart, data);
		} else {
			usart->usart->CR1 &= ~USART1_CR1_TXEIE;
		}
	}
}

ISR(USART1_vect) {
	stm32_usartInterruptHandler(usart1);
}

ISR(USART2_vect) {
	stm32_usartInterruptHandler(usart2);
}

ISR(USART3_vect) {
	stm32_usartInterruptHandler(usart3);
}

#ifdef UART4_BASE
ISR(UART4_vect) {
	stm32_usartInterruptHandler(uart4);
}
#endif

#ifdef UART5_BASE
ISR(UART5_vect) {
	stm32_usartInterruptHandler(uart5);
}
#endif

#ifdef USART6_BASE
ISR(USART6_vect) {
	stm32_usartInterruptHandler(usart6);
}
#endif

#endif