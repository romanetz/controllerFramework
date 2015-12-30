#ifndef __QUEUE_H__
#define __QUEUE_H__

#ifdef USE_CLASSES

typedef struct QueueClass QueueClass;

#include <stdint.h>
#include <mutex.h>

struct QueueClass {
	Mutex writeMutex;
	Mutex readMutex;
	int elementSize;
	int elementMaxCount;
	volatile int dataBegin;
	volatile int dataEnd;
	void *buffer;
};

#define QUEUE_CLASS(obj) ((QueueClass*)(obj))

QueueClass *queueInit(QueueClass *queue, int elementSize, int elementMaxCount);
QueueClass *queueNew(int elementSize, int elementMaxCount);
void queueDestroy(QueueClass *queue);
void queueFree(QueueClass *queue);

void queueClear(QueueClass *queue);
void queueResize(QueueClass *queue, int newSize);

int queueWrite(QueueClass *queue, const void *data, int count);
int queueWriteTimeout(QueueClass *queue, const void *data, int count, uint64_t timeout);

int queueRead(QueueClass *queue, void *buffer, int count);
int queueReadTimeout(QueueClass *queue, void *buffer, int count, uint64_t timeout);

#endif

#endif
