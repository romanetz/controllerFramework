#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <mutex.h>

class Queue {
	private:
		void *_buffer;
		
		int _maxElementCount;
		
		int _elementSize;
		
		volatile int _firstElement;
		
		volatile int _lastElement;
		
		Mutex _putMutex;
		
		Mutex _fetchMutex;
		
		void reallocBuffer();
		
	public:
		Queue(int elementSize, int maxElementCount);
		
		virtual ~Queue();
		
		void clear();
		
		int elementSize() { return _elementSize; };
		
		void setElementSize(int size);
		
		int maxElementCount() { return _maxElementCount; }
		
		void setMaxElementCount(int count);
		
		bool empty() { return _firstElement == _lastElement; };
		
		bool put(const void *element, int timeout = -1);
		
		bool fetch(void *element, int timeout = -1);
		
};

#endif
