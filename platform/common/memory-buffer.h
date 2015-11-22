#ifndef __MEMORY_BUFFER_H__
#define __MEMORY_BUFFER_H__

#include <mutex.h>
#include <iochannel.h>

class MemoryBuffer: public IOChannel {
	private:
		char *_buffer;
		
		int _bufferSize;
		
		volatile int _dataBegin;
		
		volatile int _dataEnd;
		
		Mutex _writeMutex;
		
		Mutex _readMutex;
		
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
