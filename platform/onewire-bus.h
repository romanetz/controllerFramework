#ifndef __ONEWIRE_BUS_H__
#define __ONEWIRE_BUS_H__

#include <stdint.h>
#include <iochannel.h>

struct OneWireBusSearchState {
	uint64_t lastAddress;
	bool alarm;
	char lastDifference;
	bool done;
};

class OneWireBus: public IOChannel {
	public:
		virtual bool reset() = 0;
		
		virtual bool bitRW(bool bit) = 0;
		
		virtual uint8_t byteRW(uint8_t data);
		
		void writeBit(bool bit) {
			bitRW(bit);
		}
		
		bool readBit() {
			return bitRW(true);
		}
		
		void writeByte(uint8_t data) {
			byteRW(data);
		}
		
		uint8_t readByte() {
			return byteRW(0xFF);
		}
		
		uint64_t findFirst(OneWireBusSearchState& state, bool alarm = false);
		
		uint64_t findNext(OneWireBusSearchState& state);
		
		void selectAll();
		
		void selectOne(uint64_t addr);
		
	protected:
		int writeData(const char *data, int len);
		
		int readData(char *buffer, int len);
		
};

#endif
