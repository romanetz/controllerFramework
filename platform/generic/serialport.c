#include <serialport.h>

#ifdef USE_CLASSES

void serialPortSetup(void *port, uint32_t baudrate, SerialPortFlowControl flowControl, SerialPortParity parity,
		SerialPortDataBits dataBits, SerialPortStopBits stopBits) {
	serialPortSetBaudrate(port, baudrate);
	serialPortSetFlowControl(port, flowControl);
	serialPortSetParity(port, parity);
	serialPortSetDataBits(port, dataBits);
	serialPortSetStopBits(port, stopBits);
}

#endif