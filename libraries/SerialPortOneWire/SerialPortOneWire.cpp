#include <platform.h>
#include <SerialPortOneWire.h> 

SerialPortOneWire::SerialPortOneWire(SerialPort& serialPort) : _serialPort(serialPort) {
	_serialPort.setFlowControl(SERIALPORT_FLOWCONTROL_NONE);
	_serialPort.setDataBits(SERIALPORT_DATA_8BIT);
	_serialPort.setStopBits(SERIALPORT_STOP_1BIT);
	_serialPort.setParity(SERIALPORT_PARITY_NONE);
	_serialPort.setBaudrate(115200);
}

bool SerialPortOneWire::reset() {
	_serialPort.flush();
	_serialPort.setBaudrate(9600);
	static const uint8_t txData = 0xF0;
	_serialPort.write((const char*)&txData, sizeof(txData));
	uint8_t rxData = 0xF0;
	_serialPort.read((char*)&rxData, sizeof(rxData));
	_serialPort.setBaudrate(115200);
	return rxData != txData;
}

bool SerialPortOneWire::bitRW(bool bit) {
	_serialPort.flush();
	uint8_t txData = bit ? 0xFF : 0x00;
	_serialPort.write((char*)&txData, sizeof(txData));
	uint8_t rxData;
	_serialPort.read((char*)&rxData, sizeof(rxData));
	return txData == rxData;
}