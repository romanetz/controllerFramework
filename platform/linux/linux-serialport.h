#ifndef __LINUX_SERIALPORT_H__
#define __LINUX_SERIALPORT_H__

#include <linux.h>
#include <serialport.h>

class LinuxSerialPort: public SerialPort {
	private:
		int _fd;
		
	public:
		LinuxSerialPort(const char *fileName) {
			_fd = open(fileName, O_RDWR);
		}
		
		LinuxSerialPort(const char *fileName, uint32_t baudrate, SerialPortDataBits dataBits = SERIALPORT_DATA_8BIT,
			SerialPortStopBits stopBits = SERIALPORT_STOP_1BIT, SerialPortParity parity = SERIALPORT_PARITY_NONE,
			SerialPortFlowControl flowControl = SERIALPORT_FLOWCONTROL_NONE);
		
		void setBaudrate(uint32_t baudrate);
		
		void setParity(SerialPortParity parity);
		
		void setDataBits(SerialPortDataBits dataBits);
		
		void setStopBits(SerialPortStopBits stopBits);
		
		void setFlowControl(SerialPortFlowControl flowControl);
		
		void flush() {
			tcflush(_fd, TCIOFLUSH);
		}
		
		int fd() {
			return _fd;
		}
		
	protected:
		int writeData(const char *data, int len, int timeout) {
			return ::write(_fd, data, len);
		}
		
		int readData(char *buffer, int len, int timeout) {
			return ::read(_fd, buffer, len);
		}
		
	
};

#endif
