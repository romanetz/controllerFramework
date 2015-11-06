/**
	\addtogroup platform Platform-independent
	@{
	\addtogroup SerialPort Serial port
	@{
*/
#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#include <iochannel.h>

/**
	\brief Parity check modes for serial interface.
*/
enum SerialPortParity {
	SERIALPORT_PARITY_NONE, ///< Parity check disabled
	SERIALPORT_PARITY_ODD, ///< Odd parity
	SERIALPORT_PARITY_EVEN ///< Even parity
};

/**
	\brief Number of data bits for serial interface.
*/
enum SerialPortDataBits {
	SERIALPORT_DATA_7BIT = 7, ///< 7 bit
	SERIALPORT_DATA_8BIT = 8, ///< 8 bit
	SERIALPORT_DATA_9BIT = 9 ///< 9 bit
};

/**
	\brief Stop bit length for serial interface.
*/
enum SerialPortStopBits {
	SERIALPORT_STOP_0_5BIT, ///< 0.5 bit
	SERIALPORT_STOP_1BIT, ///< 1 bit
	SERIALPORT_STOP_1_5BIT, ///< 1.5 bit
	SERIALPORT_STOP_2BIT /// 2 bit
};

/**
	\brief Serial port flow control mode.
	No all platform implementations supports hardware flow control.
*/
enum SerialPortFlowControl {
	SERIALPORT_FLOWCONTROL_NONE, ///< Hardware flow control disable
	SERIALPORT_FLOWCONTROL_RTS, ///< Enable flow control by RTS signal
	SERIALPORT_FLOWCONTROL_CTS, ///< Enable flow control by CTS signal
	SERIALPORT_FLOWCONTROL_RTS_CTS ///< Enable flow control by both RTS and CTS signals
};

/**
	\brief Basic serial port class.
*/
class SerialPort: public IOChannel {
	public:
		/**
			\brief Set serial port baudrate.
			\param[in] baudrate Serial port speed in bits per second.
		*/
		virtual void setBaudrate(uint32_t baudrate) = 0;
		
		/**
			\brief Set serial interface parity check mode.
			\param[in] parity Mode of a parity check.
		*/
		virtual void setParity(SerialPortParity parity) = 0;
		
		/**
			\brief Set count of serial interface data bits.
			\param[in] dataBits Count of data bits.
		*/
		virtual void setDataBits(SerialPortDataBits dataBits) = 0;
		
		/**
			\brief Set length of serial interface stop bit.
			\param[in] stopBits Length of serial interface stop condition in bits.
		*/
		virtual void setStopBits(SerialPortStopBits stopBits) = 0;
		
		/**
			\brief Set flow control mode.
			\param[in] flowControl Desired flow control mode.
		*/
		virtual void setFlowControl(SerialPortFlowControl flowControl) = 0;
		
};

#endif
/**
	@}
	@}
*/
