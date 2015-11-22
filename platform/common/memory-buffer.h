#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iochannel.h>

class MemoryBuffer: public IOChannel {
	private:
		char *_buffer;
		
		int _bufferSize;
		
		volatile int _dataBegin;
		
		volatile int _dataEnd;
		
	protected:
		int writeData(const char *data, int len, int timeout);
		
		int readData(char *buffer, int len, int timeout);
		
	public:
		MemoryBuffer(int size);
		
		~MemoryBuffer();
		
		int size() { return _bufferSize; }
		
		void setSize(int size);
		
		void clear();
		
};

#endif
