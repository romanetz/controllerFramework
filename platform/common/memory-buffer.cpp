#include <stdlib.h>
#include <memory-buffer.h> 

MemoryBuffer::MemoryBuffer(int size) {
	_buffer = NULL;
	setSize(size);
}

MemoryBuffer::~MemoryBuffer() {
	free(_buffer);
}

void MemoryBuffer::setSize(int size) {
	_writeMutex.lock();
	_readMutex.lock();
	if (_buffer) free(_buffer);
	_buffer = (char*)malloc(size);
	_bufferSize = _buffer ? size : 0;
	_dataBegin = 0;
	_dataEnd = 0;
	_readMutex.unlock();
	_writeMutex.unlock();
}

void MemoryBuffer::clear() {
	_writeMutex.lock();
	_readMutex.lock();
	_dataBegin = 0;
	_dataEnd = 0;
	_readMutex.unlock();
	_writeMutex.unlock();
}

int MemoryBuffer::writeData(const char *data, int len, int timeout) {
	if (_buffer == NULL) return 0;
	if (!_writeMutex.lock(timeout)) return 0;
	int i;
	for (i = 0; i < len; i++) {
		int nextDataPos = (_dataEnd >= _bufferSize) ? 0 : _dataEnd + 1;
		while (nextDataPos == _dataBegin) {
			if (timeout == 0) break;
		}
		if (nextDataPos == _dataBegin) break;
		_buffer[_dataEnd] = data[i];
		_dataEnd = nextDataPos;
	}
	_writeMutex.unlock();
	return i;
}

int MemoryBuffer::readData(char *buffer, int len, int timeout) {
	if (_buffer == NULL) return 0;
	if (!_readMutex.lock(timeout)) return 0;
	int i;
	for (i = 0; i < len; i++) {
		int nextDataPos = (_dataBegin >= _bufferSize) ? 0 : _dataBegin + 1;
		while (_dataBegin == _dataEnd) {
			if (timeout == 0) break;
		}
		if (_dataBegin == _dataEnd) break;
		buffer[i] = _buffer[_dataBegin];
		_dataBegin = nextDataPos;
	}
	_readMutex.unlock();
	return i;
}
