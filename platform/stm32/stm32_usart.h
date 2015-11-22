/**
	\addtogroup STM32 STM32
	@{
	\addtogroup STM32_USART USART
	@{
*/
#ifndef __STM32_USART_H__
#define __STM32_USART_H__

#include <stm32.h>
#include <memory-buffer.h>
#include <serialport.h>

/**
	\brief STM32 USART module MMIO registers.
*/
struct STM32USARTRegs {
	uint32_t SR; ///< Status register
	uint32_t DR; ///< Data register
	uint32_t BRR; ///< Baudrate register
	uint32_t CR1; ///< Control register 1
	uint32_t CR2; ///< Control register 2
	uint32_t CR3; ///< Control register 3
	uint32_t GTRR; ///< Guard time and prescaler register	
};

#define USART1 MMIO(USART1_BASE, STM32USARTRegs)

#define USART2 MMIO(USART2_BASE, STM32USARTRegs)

#define USART3 MMIO(USART3_BASE, STM32USARTRegs)

#ifdef UART4_BASE
#define UART4 MMIO(UART4_BASE, STM32USARTRegs)
#endif

#ifdef UART5_BASE
#define UART5 MMIO(UART5_BASE, STM32USARTRegs)
#endif

#ifdef USART6_BASE
#define USART6 MMIO(USART6_BASE, STM32USARTRegs)
#endif

/**
	\brief STM32 USART TX/RX mode.
*/
enum STM32USARTMode {
	STM32_USART_DISABLED = 0, ///< USART peripheral disabled.
	STM32_USART_TX_ONLY = USART1_CR1_TE, ///< Only USART transmitter enabled.
	STM32_USART_RX_ONLY = USART1_CR1_RE, ///< Only USART receiver enabled.
	STM32_USART_TX_RX = USART1_CR1_TE | USART1_CR1_RE ///< Both USART transmitter and receiver enabled.
};

/**
	\brief STM32 USART module.
*/
class STM32USART : public SerialPort {
	private:
		volatile STM32USARTRegs* _usart;
		
	public:
		/**
			\brief Initialize USART object.
			\param[in] usart STM32 USART MMIO registers.
		*/
		STM32USART(volatile STM32USARTRegs* usart, int txBufferSize, int rxBufferSize);
		
		/**
			\brief Initialize USART object and setup peripheral.
			\param[in] usart STM32 USART MMIO registers.
			\param[in] mode USART mode (only receive, only transmit or both).
			\param[in] baudrate USART baudrate in bits per second.
			\param[in] dataBits Data word length (only 8 and 9 bits supports).
			\param[in] stopBits Stop bit length.
			\param[in] parity Parity check mode.
			\param[in] flowControl Flow control mode (hardware flow control available only for some USARTs).
		*/
		STM32USART(volatile STM32USARTRegs* usart, int txBufferSize, int rxBufferSize, STM32USARTMode mode, uint32_t baudrate,
			SerialPortDataBits dataBits = SERIALPORT_DATA_8BIT, SerialPortStopBits stopBits = SERIALPORT_STOP_1BIT,
			SerialPortParity parity = SERIALPORT_PARITY_NONE, SerialPortFlowControl flowControl = SERIALPORT_FLOWCONTROL_NONE);
		
		~STM32USART();
		
		/**
			\brief Set USART RX/TX mode.
			\param[in] mode Desired mode (TX only, RX only or both).
		*/
		void setMode(STM32USARTMode mode);
		
		void setBaudrate(uint32_t baudrate);
		
		void setDataBits(SerialPortDataBits dataBits);
		
		void setStopBits(SerialPortStopBits stopBits);
		
		void setParity(SerialPortParity parity);
		
		void setFlowControl(SerialPortFlowControl flowControl);
		
		void flush();
		
		void interruptHandler();
		
	private:
		MemoryBuffer _txBuffer;
		
		MemoryBuffer _rxBuffer;
		
		int writeData(const char *data, int len, int timeout);
		
		int readData(char *buffer, int len, int timeout);
		
};

#endif
/**
	@}
	@}
*/
