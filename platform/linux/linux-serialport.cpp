#include <linux.h>
namespace asmtermios {
#include <asm/termios.h>
};

using namespace asmtermios;

LinuxSerialPort::LinuxSerialPort(const char *fileName, uint32_t baudrate, SerialPortDataBits dataBits, SerialPortStopBits stopBits,
		SerialPortParity parity, SerialPortFlowControl flowControl) {
	_fd = open(fileName, O_RDWR);
	setBaudrate(baudrate);
	setParity(parity);
	setDataBits(dataBits);
	setStopBits(stopBits);
	setFlowControl(flowControl);
}

void LinuxSerialPort::setBaudrate(uint32_t baudrate) {
	struct termios2 options;
	ioctl(_fd, TCGETS2, &options);
	options.c_cflag &= ~CBAUD;
	options.c_cflag |= CLOCAL | CREAD | BOTHER;
	options.c_ispeed = baudrate;
	options.c_ospeed = baudrate;
	ioctl(_fd, TCSETS2, &options);
}

void LinuxSerialPort::setParity(SerialPortParity parity) {
	struct termios2 options;
	ioctl(_fd, TCGETS2, &options);
	options.c_cflag &= ~(PARENB | PARODD);
	if (parity == SERIALPORT_PARITY_ODD) {
		options.c_cflag |= PARENB | PARODD;
	} else if (parity == SERIALPORT_PARITY_EVEN) {
		options.c_cflag |= PARENB;
	}
	ioctl(_fd, TCSETS2, &options);
}

void LinuxSerialPort::setDataBits(SerialPortDataBits dataBits) {
	struct termios2 options;
	ioctl(_fd, TCGETS2, &options);
	options.c_cflag &= ~CSIZE;
	switch (dataBits) {
		case SERIALPORT_DATA_5BIT:
			options.c_cflag |= CS5;
			break;
		case SERIALPORT_DATA_6BIT:
			options.c_cflag |= CS6;
			break;
		case SERIALPORT_DATA_7BIT:
			options.c_cflag |= CS7;
			break;
		case SERIALPORT_DATA_8BIT:
		default:
			options.c_cflag |= CS8;
	}
	ioctl(_fd, TCSETS2, &options);
}

void LinuxSerialPort::setStopBits(SerialPortStopBits stopBits) {
	struct termios2 options;
	ioctl(_fd, TCGETS2, &options);
	if (stopBits == SERIALPORT_STOP_2BIT) {
		options.c_cflag |= CSTOPB;
	} else {
		options.c_cflag &= ~CSTOPB;
	}
	ioctl(_fd, TCSETS2, &options);
}

void LinuxSerialPort::setFlowControl(SerialPortFlowControl flowControl) {
	struct termios2 options;
	ioctl(_fd, TCGETS2, &options);
	if (flowControl == SERIALPORT_FLOWCONTROL_RTS_CTS) {
		options.c_cflag |= CRTSCTS;
	} else {
		options.c_cflag &= ~CRTSCTS;
	}
	ioctl(_fd, TCSETS2, &options);
}
