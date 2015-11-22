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
	if (_buffer) free(_buffer);
	_buffer = (char*)malloc(size);
	_bufferSize = _buffer ? size : 0;
	clear();
}

void MemoryBuffer::clear() {
	_dataBegin = 0;
	_dataEnd = 0;
}

int MemoryBuffer::writeData(const char *data, int len, int timeout) {
	if (_buffer == NULL) return 0;
	for (int i = 0; i < len; i++) {
		int nextDataPos = (_dataEnd >= _bufferSize) ? 0 : _dataEnd + 1;
		while (nextDataPos == _dataBegin) {
			if (timeout == 0) return i;
		}
		_buffer[_dataEnd] = data[i];
		_dataEnd = nextDataPos;
	}
	return len;
}

int MemoryBuffer::readData(char *buffer, int len, int timeout) {
	if (_buffer == NULL) return 0;
	for (int i = 0; i < len; i++) {
		int nextDataPos = (_dataBegin >= _bufferSize) ? 0 : _dataBegin + 1;
		while (_dataBegin == _dataEnd) {
			if (timeout == 0) return i;
		}
		buffer[i] = _buffer[_dataBegin];
		_dataBegin = nextDataPos;
	}
	return len;
}