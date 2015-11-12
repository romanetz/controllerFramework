#include <platform.h>
#include <SerialPortOneWire.h>
#include <ds18b20.h>

LinuxFileChannel stdOut(STDOUT_FILENO);
LinuxSerialPort serialPort("/dev/ttyUSB0");
SerialPortOneWire oneWire(serialPort);

int main() {
	OneWireBusSearchState state;
	uint64_t addr = oneWire.findFirst(state);
	while (addr != (uint64_t)-1) {
		DS18B20 sensor(oneWire, addr);
		sensor.updateData();
		stdOut.printf("%llX: %i°C\n", addr, sensor.temperature());
		addr = oneWire.findNext(state);
	}
	return 0;
}
