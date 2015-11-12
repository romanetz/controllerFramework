#include <stdio.h>
#include <onewire-bus.h>

uint8_t OneWireBus::byteRW(uint8_t data) {
	uint8_t rxData = 0;
	for (int i = 0; i < 8; i++) {
		if (bitRW(data & (1 << i))) {
			rxData |= 1 << i;
		}
	}
	return rxData;
}

uint64_t OneWireBus::findFirst(OneWireBusSearchState& state, bool alarm) {
	state.lastAddress = 0;
	state.lastDifference = 0;
	state.alarm = alarm;
	state.done = false;
	return findNext(state);
}

uint64_t OneWireBus::findNext(OneWireBusSearchState& state) {
	if (state.done) return -1;
	if (!reset()) return -1;
	writeByte(state.alarm ? 0xEC : 0xF0);
	uint64_t addr = 0;
	bool nextBit;
	char lastDifference = -1;
	state.done = true;
	for (int i = 0; i < 64; i++) {
		bool firstBit = readBit();
		bool secondBit = readBit();
		if (firstBit && secondBit) { // 1 1
			return -1;
		} else if (firstBit) { // 1 0
			nextBit = true;
		} else if (secondBit) { // 0 1
			nextBit = false;
		} else { // 0 0
			if (i < state.lastDifference) {
				nextBit = state.lastAddress & ((uint64_t)1 << i);
			} else if (i == state.lastDifference) {
				nextBit = true;
				state.lastDifference = lastDifference;
			} else {
				nextBit = false;
				state.lastDifference = i;
			}
			state.done = state.done & nextBit;
			lastDifference = i;
		}
		if (nextBit) {
			addr |= (uint64_t)1 << i;
		}
		writeBit(nextBit);
	}
	state.lastAddress = addr;
	addr = 0;
	for (int i = 0; i < 64; i += 8) {
		addr |= ((state.lastAddress >> i) & 0xFF) << (56 - i);
	}
	return addr;
}

void OneWireBus::selectAll() {
	reset();
	writeByte(0xCC);
}

void OneWireBus::selectOne(uint64_t addr) {
	reset();
	writeByte(0x55);
	for (int i = 56; i >= 0; i -= 8) {
		writeByte((addr >> i) & 0xFF);
	}
}

int OneWireBus::writeData(const char *data, int len) {
	for (int i = 0; i < len; i++) {
		writeByte(data[i]);
	}
	return len;
}

int OneWireBus::readData(char *buffer, int len) {
	for (int i = 0; i < len; i++) {
		buffer[i] = readByte();
	}
	return len;
}