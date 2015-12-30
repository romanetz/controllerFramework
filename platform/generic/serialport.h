#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#include <iostream.h>

typedef enum SerialPortFlowControl {
	SERIALPORT_FLOWCONTROL_NONE = 0,
	SERIALPORT_FLOWCONTROL_CTS,
	SERIALPORT_FLOWCONTROL_RTS,
	SERIALPORT_FLOWCONTROL_RTS_CTS
} SerialPortFlowControl;

typedef enum SerialPortParity {
	SERIALPORT_PARITY_NONE = 0,
	SERIALPORT_PARITY_ODD,
	SERIALPORT_PARITY_EVEN
} SerialPortParity;

typedef enum SerialPortDataBits {
	SERIALPORT_DATA_7BIT,
	SERIALPORT_DATA_8BIT,
	SERIALPORT_DATA_9BIT
} SerialPortDataBits;

typedef enum SerialPortStopBits {
	SERIALPORT_STOP_0_5BIT,
	SERIALPORT_STOP_1BIT,
	SERIALPORT_STOP_1_5BIT,
	SERIALPORT_STOP_2BIT
} SerialPortStopBits;

#ifdef USE_CLASSES

typedef struct SerialPortClass SerialPortClass;

typedef void (*SerialPortSetBaudrateFunc)(SerialPortClass *port, uint32_t baudrate);
typedef void (*SerialPortSetFlowControlFunc)(SerialPortClass *port, SerialPortFlowControl flowControl);
typedef void (*SerialPortSetParityFunc)(SerialPortClass *port, SerialPortParity parity);
typedef void (*SerialPortSetDataBitsFunc)(SerialPortClass *port, SerialPortDataBits bits);
typedef void (*SerialPortSetStopBitsFunc)(SerialPortClass *port, SerialPortStopBits bits);

struct SerialPortClass {
	IOStreamClass parent;
	SerialPortSetBaudrateFunc setBaudrate;
	SerialPortSetFlowControlFunc setFlowControl;
	SerialPortSetParityFunc setParity;
	SerialPortSetDataBitsFunc setDataBits;
	SerialPortSetStopBitsFunc setStopBits;
};

#define SERIAL_PORT_CLASS(obj) ((SerialPortClass*)(obj))

#define serialPortSetBaudrate(port, baudrate) SERIAL_PORT_CLASS(port)->setBaudrate(SERIAL_PORT_CLASS(port), baudrate)
#define serialPortSetFlowControl(port, flowControl) SERIAL_PORT_CLASS(port)->setFlowControl(SERIAL_PORT_CLASS(port), flowControl)
#define serialPortSetParity(port, parity) SERIAL_PORT_CLASS(port)->setParity(SERIAL_PORT_CLASS(port), parity)
#define serialPortSetDataBits(port, bits) SERIAL_PORT_CLASS(port)->setDataBits(SERIAL_PORT_CLASS(port), bits)
#define serialPortSetStopBits(port, bits) SERIAL_PORT_CLASS(port)->setStopBits(SERIAL_PORT_CLASS(port), bits)

void serialPortSetup(void *port, uint32_t baudrate, SerialPortFlowControl flowControl, SerialPortParity parity,
	SerialPortDataBits dataBits, SerialPortStopBits stopBits);

#endif

#endif
