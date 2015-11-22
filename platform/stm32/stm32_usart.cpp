#include <stm32.h>

/*
	TODO:
	USART1_CR3_DMAR - RX DMA enable
	USART1_CR3_DMAT - TX DMA enable
	USART1_CR1_RXNEIE - RX buffer not empty interrupt enable
	USART1_CR1_TXEIE - TX buffer empty interrupt enable
	USART1_CR3_EIE - Error interrupt enable
*/

STM32USART::STM32USART(volatile STM32USARTRegs* usart) : _usart(usart) {
	STM32RCC::periphClockEnable(_usart);
}
		
STM32USART::STM32USART(volatile STM32USARTRegs* usart, STM32USARTMode mode, uint32_t baudrate, SerialPortDataBits dataBits,
		SerialPortStopBits stopBits, SerialPortParity parity, SerialPortFlowControl flowControl) : _usart(usart) {
	STM32RCC::periphClockEnable(_usart);
	setBaudrate(baudrate);
	setDataBits(dataBits);
	setStopBits(stopBits);
	setParity(parity);
	setFlowControl(flowControl);
	setMode(mode);
}

void STM32USART::setMode(STM32USARTMode mode) {
	_usart->CR1 = (_usart->CR1 & ~(USART1_CR1_TE | USART1_CR1_RE)) | mode;
	if (mode != STM32_USART_DISABLED) {
		_usart->CR1 |= USART1_CR1_UE;
	} else {
		_usart->CR1 &= ~USART1_CR1_UE;
	}
}

void STM32USART::setBaudrate(uint32_t baudrate) {
	uint32_t clock = STM32RCC::apb1Frequency();
#ifdef USART6
	if ((_usart == USART1) || (_usart == USART6)) {
		clock = STM32RCC::apb2Frequency();
	}
#else
	if (_usart == USART1) {
		clock = STM32RCC::apb2Frequency();
	}
#endif
	_usart->BRR = (2 * clock + baudrate) / (2 * baudrate);
}

void STM32USART::setDataBits(SerialPortDataBits dataBits) {
	if (dataBits == SERIALPORT_DATA_9BIT) {
		_usart->CR1 |= USART1_CR1_M;
	} else {
		_usart->CR1 &= ~USART1_CR1_M;
	}
}

void STM32USART::setStopBits(SerialPortStopBits stopBits) {
	uint32_t value = 0;
	switch (stopBits) {
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
	_usart->CR2 = (_usart->CR2 & ~USART1_CR2_STOP_MASK) | (value << USART1_CR2_STOP_OFFSET);
}

void STM32USART::setParity(SerialPortParity parity) {
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
	_usart->CR1 = (_usart->CR1 & ~(USART1_CR1_PCE | USART1_CR1_PS)) | value;
}

void STM32USART::setFlowControl(SerialPortFlowControl flowControl) {
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
	_usart->CR3 = (_usart->CR3 & ~(USART1_CR3_RTSE | USART1_CR3_CTSE)) | cfg;
}

int STM32USART::writeData(const char *data, int len, int timeout) {
	for (int i = 0; i < len; i++) {
		while ((_usart->SR & USART1_SR_TXE) == 0) {
			if (timeout == 0) return i;
		}
		_usart->DR = data[i];
	}
	return len;
}

int STM32USART::readData(char *buffer, int len, int timeout) {
	for (int i = 0; i < len; i++) {
		while ((_usart->SR & USART1_SR_RXNE) == 0) {
			if (timeout == 0) return i;
		}
		buffer[i] = _usart->DR;
	}
	return len;
}
