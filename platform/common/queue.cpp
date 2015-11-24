#include <stdlib.h>
#include <string.h>
#include <queue.h>

Queue::Queue(int elementSize, int maxElementCount) : _maxElementCount(maxElementCount), _elementSize(elementSize) {
	_buffer = NULL;
	reallocBuffer();
}

Queue::~Queue() {
	if (_buffer) free(_buffer);
}

void Queue::clear() {
	_putMutex.lock();
	_fetchMutex.lock();
	_firstElement = 0;
	_lastElement = 0;
	_fetchMutex.unlock();
	_putMutex.unlock();
}

void Queue::reallocBuffer() {
	if (_buffer) free(_buffer);
	if (_elementSize && _maxElementCount) {
		_buffer = malloc(_elementSize * _maxElementCount);
	} else {
		_buffer = NULL;
	}
	_firstElement = 0;
	_lastElement = 0;
}

void Queue::setElementSize(int size) {
	_putMutex.lock();
	_fetchMutex.lock();
	_elementSize = size;
	reallocBuffer();
	_fetchMutex.unlock();
	_putMutex.unlock();
}

void Queue::setMaxElementCount(int count) {
	_putMutex.lock();
	_fetchMutex.lock();
	_maxElementCount = count;
	reallocBuffer();
	_fetchMutex.unlock();
	_putMutex.unlock();
}

bool Queue::put(const void *element, int timeout) {
	if (!_putMutex.lock(timeout)) {
		return false;
	}
	bool r = true;
	int nextElement = (_lastElement >= _maxElementCount) ? 0 : (_lastElement + 1);
	while ((nextElement != _firstElement) || (_maxElementCount == 0)) {
		if (timeout == 0) {
			r = false;
			break;
		}
	}
	if (r) {
		memmove((char*)_buffer + _lastElement * _elementSize, element, _elementSize);
		_lastElement = nextElement;
	}
	_putMutex.unlock();
	return r;
}

bool Queue::fetch(void *element, int timeout) {
	if (!_fetchMutex.lock(timeout)) {
		return false;
	}
	bool r = true;
	while ((_firstElement == _lastElement) || (_maxElementCount == 0)) {
		if (timeout == 0) {
			r = false;
			break;
		}
	}
	if (r) {
		memmove(element, (const char*)_buffer + _firstElement * _elementSize, _elementSize);
		_firstElement++;
	}
	_fetchMutex.unlock();
	return r;
}