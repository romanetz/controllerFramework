#ifndef __SERIALPORT_ONEWIRE_H__
#define __SERIALPORT_ONEWIRE_H__

#include <onewire-bus.h>
#include <serialport.h>

class SerialPortOneWire: public OneWireBus {
	private:
		SerialPort& _serialPort;
	
	public:
		SerialPortOneWire(SerialPort& serialPort);
		
		bool reset();
		
		bool bitRW(bool bit);
		
};

#endif
