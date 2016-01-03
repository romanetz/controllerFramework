#include <stdlib.h>
#include <string.h>
#include <queue.h>

#ifdef USE_CLASSES

QueueClass *queueInit(QueueClass *queue, int elementSize, int elementMaxCount) {
	mutexInit(&queue->writeMutex);
	mutexInit(&queue->readMutex);
	eventSourceInit(&queue->writeEventSource);
	eventSourceInit(&queue->readEventSource);
	queue->elementSize = elementSize;
	queue->elementMaxCount = 0;
	queue->buffer = NULL;
	queueResize(queue, elementMaxCount);
	return queue;
}

QueueClass *queueNew(int elementSize, int elementMaxCount) {
	QueueClass *queue = malloc(sizeof(QueueClass));
	if (queue) {
		return queueInit(queue, elementSize, elementMaxCount);
	}
	return NULL;
}

void queueDestroy(QueueClass *queue) {
	eventSourceDestroy(&queue->writeEventSource);
	eventSourceDestroy(&queue->readEventSource);
	mutexDestroy(&queue->readMutex);
	mutexDestroy(&queue->writeMutex);
	free(queue->buffer);
}

void queueFree(QueueClass *queue) {
	queueDestroy(queue);
	free(queue);
}

void queueClear(QueueClass *queue) {
	mutexLock(&queue->readMutex);
	mutexLock(&queue->writeMutex);
	queue->dataBegin = 0;
	queue->dataEnd = 0;
	mutexUnlock(&queue->writeMutex);
	mutexUnlock(&queue->readMutex);
	eventSourceSendSignal(&queue->readEventSource);
}

void queueResize(QueueClass *queue, int newSize) {
	mutexLock(&queue->readMutex);
	mutexLock(&queue->writeMutex);
	if (queue->buffer) {
		free(queue->buffer);
	}
	queue->elementMaxCount = newSize;
	queue->buffer = malloc(queue->elementSize * queue->elementMaxCount);
	queue->dataBegin = 0;
	queue->dataEnd = 0;
	mutexUnlock(&queue->writeMutex);
	mutexUnlock(&queue->readMutex);
	eventSourceSendSignal(&queue->readEventSource);
}

int queueWrite(QueueClass *queue, const void *data, int count) {
	return queueWriteTimeout(queue, data, count, -1);
}

int queueWriteTimeout(QueueClass *queue, const void *data, int count, timestamp_t timeout) {
	int i = 0;
	while (i < count) {
		if (queue->elementMaxCount == 0) break;
		if (!mutexLockTimeout(&queue->writeMutex, timeout)) break;
		int chunkCount = 0;
		if (queue->dataEnd >= queue->dataBegin) {
			chunkCount = queue->elementMaxCount - queue->dataEnd;
			if (queue->dataBegin == 0) {
				chunkCount--;
			}
		} else {
			chunkCount = queue->dataBegin - queue->dataEnd - 1;
		}
		if (chunkCount > (count - i)) chunkCount = count - i;
		if (chunkCount > 0) {
			memmove(queue->buffer + queue->elementSize * queue->dataEnd, data + queue->elementSize * i, queue->elementSize * chunkCount);
			queue->dataEnd += chunkCount;
			if (queue->dataEnd >= queue->elementMaxCount) {
				queue->dataEnd = 0;
			}
			i += chunkCount;
		}
		mutexUnlock(&queue->writeMutex);
		if (chunkCount <= 0) {
			if (!eventSourceWaitSignalTimeout(&queue->readEventSource, timeout)) break;
		}
	}
	return i;
}

int queueRead(QueueClass *queue, void *buffer, int count) {
	return queueWriteTimeout(queue, buffer, count, -1);
}

int queueReadTimeout(QueueClass *queue, void *buffer, int count, timestamp_t timeout) {
	int i = 0;
	while (i < count) {
		if (queue->elementMaxCount == 0) break;
		if (!mutexLockTimeout(&queue->readMutex, timeout)) break;
		int chunkCount = 0;
		if (queue->dataBegin <= queue->dataEnd) {
			chunkCount = queue->dataEnd - queue->dataBegin;
		} else {
			chunkCount = queue->elementMaxCount - queue->dataBegin;
		}
		if (chunkCount > (count - i)) chunkCount = count - i; 
		if (chunkCount > 0) {
			memmove(buffer + i * queue->elementSize, queue->buffer + queue->dataBegin * queue->elementSize, chunkCount * queue->elementSize);
			queue->dataBegin += chunkCount;
			if (queue->dataBegin >= queue->elementMaxCount) {
				queue->dataBegin = 0;
			}
			i += chunkCount;
		}
		mutexUnlock(&queue->readMutex);
		if (chunkCount <= 0) {
			if (!eventSourceWaitSignalTimeout(&queue->writeEventSource, timeout)) break;
		}
	}
	return i;
}

#endif
