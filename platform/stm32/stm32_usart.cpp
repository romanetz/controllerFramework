#include <stm32.h>

/*
	TODO:
	USART1_CR3_DMAR - RX DMA enable
	USART1_CR3_DMAT - TX DMA enable
	USART1_CR3_EIE - Error interrupt enable
*/

static STM32USART *usart1;
static STM32USART *usart2;
static STM32USART *usart3;
#ifdef UART4_BASE
static STM32USART *uart4;
#endif
#ifdef UART5_BASE
static STM32USART *uart5;
#endif
#ifdef USART6_BASE
static STM32USART *usart6;
#endif

static STM32USART **usartObject(volatile STM32USARTRegs* usart) {
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
			return NULL;
	}
}

static uint8_t usartIRQ(volatile STM32USARTRegs *usart) {
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

STM32USART::STM32USART(volatile STM32USARTRegs* usart, int txBufferSize, int rxBufferSize) :
	_usart(usart), _txBuffer(txBufferSize), _rxBuffer(rxBufferSize)
{
	STM32USART **usartObj = usartObject(usart);
	if (usartObj) {
		*usartObj = this;
	}
	STM32RCC::periphClockEnable(_usart);
	NVIC::enableIRQ(usartIRQ(usart));
	_usart->CR1 |= USART1_CR1_RXNEIE;
}
		
STM32USART::STM32USART(volatile STM32USARTRegs* usart, int txBufferSize, int rxBufferSize, STM32USARTMode mode, uint32_t baudrate,
	SerialPortDataBits dataBits, SerialPortStopBits stopBits, SerialPortParity parity, SerialPortFlowControl flowControl) :
	_usart(usart), _txBuffer(txBufferSize), _rxBuffer(rxBufferSize)
{
	STM32USART **usartObj = usartObject(usart);
	if (usartObj) {
		*usartObj = this;
	}
	STM32RCC::periphClockEnable(_usart);
	setBaudrate(baudrate);
	setDataBits(dataBits);
	setStopBits(stopBits);
	setParity(parity);
	setFlowControl(flowControl);
	setMode(mode);
	NVIC::enableIRQ(usartIRQ(usart));
	_usart->CR1 |= USART1_CR1_RXNEIE;
}

STM32USART::~STM32USART() {
	STM32USART **usartObj = usartObject(_usart);
	if (usartObj) {
		*usartObj = NULL;
	}
	STM32RCC::periphClockDisable(_usart);
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
	int r = _txBuffer.write(data, len, timeout);
	_usart->CR1 |= USART1_CR1_TXEIE;
	return r;
}

int STM32USART::readData(char *buffer, int len, int timeout) {
	return _rxBuffer.read(buffer, len, timeout);
}

void STM32USART::flush() {
	_rxBuffer.clear();
}

void STM32USART::interruptHandler() {
	uint32_t sr = _usart->SR;
	if (sr & USART1_SR_RXNE) {
		uint8_t data = _usart->DR;
		_rxBuffer.write((const char*)&data, sizeof(data), 0);
	}
	if (sr & USART1_SR_TXE) {
		uint8_t data;
		if (_txBuffer.read((char*)&data, sizeof(data), 0) >= (int)sizeof(data)) {
			_usart->DR = data;
		} else {
			_usart->CR1 &= ~USART1_CR1_TXEIE;
		}
	}
}

ISR(USART1_vect) {
	if (usart1) {
		usart1->interruptHandler();
	}
}

ISR(USART2_vect) {
	if (usart2) {
		usart2->interruptHandler();
	}
}

ISR(USART3_vect) {
	if (usart3) {
		usart3->interruptHandler();
	}
}

#ifdef UART4_BASE
ISR(UART4_vect) {
	if (uart4) {
		uart4->interruptHandler();
	}
}
#endif

#ifdef UART5_BASE
ISR(UART5_vect) {
	if (uart5) {
		uart5->interruptHandler();
	}
}
#endif

#ifdef USART6_BASE
ISR(USART6_vect) {
	if (usart6) {
		usart6->interruptHandler();
	}
}
#endif
