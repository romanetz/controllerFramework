#ifndef __ONEWIRE_DEVICE_H__
#define __ONEWIRE_DEVICE_H__

#include <onewire-bus.h>

class OneWireDevice {
	protected:
		OneWireBus& _bus;
		
		uint64_t _addr;
	
	public:
		OneWireDevice(OneWireBus& bus, uint64_t addr) : _bus(bus), _addr(addr) {
			// Do nothing
		}
		
		void select() {
			_bus.selectOne(_addr);
		}
		
};

#endif
