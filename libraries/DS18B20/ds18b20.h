#ifndef __DS18B20_H__
#define __DS18B20_H__

#include <OneWireDevice.h>

class DS18B20: public OneWireDevice {
	private:
		uint8_t _scratchPad[9];
		
	public:
		DS18B20(OneWireBus& bus, uint64_t addr);
		
		bool convert(bool wait = false);
		
		bool updateData(bool convert = true);
		
		uint8_t scratchPad(uint8_t addr) {
			return _scratchPad[addr];
		}
		
		int16_t temperature() {
			int16_t value = _scratchPad[0] | (_scratchPad[1] << 8);
			return value / 16;
		}
		
};

#endif
