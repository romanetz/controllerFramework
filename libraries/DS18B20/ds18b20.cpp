#include <platform.h>
#include <ds18b20.h>

DS18B20::DS18B20(OneWireBus& bus, uint64_t addr) : OneWireDevice(bus, addr) {
	updateData(false);
}

bool DS18B20::convert(bool wait) {
	select();
	_bus.writeByte(0x44);
	if (wait) {
		usleep(750000);
	}
	return true;
}

bool DS18B20::updateData(bool convert) {
	if (convert) {
		this->convert(true);
	}
	select();
	_bus.writeByte(0xBE);
	_bus.read((char*)_scratchPad, sizeof(_scratchPad));
	return true;
}
