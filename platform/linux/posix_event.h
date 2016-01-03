#ifndef __POSIX_EVENT_H__
#define __POSIX_EVENT_H__

#include <event.h>
#include <pthread.h>

struct EventSource {
	pthread_cond_t condVar;
};

void eventSourceInit(EventSource *eventSource);
void eventSourceDestroy(EventSource *eventSource);
void eventSourceSendSignal(EventSource *eventSource);
BOOL eventSourceWaitSignalTimeout(EventSource *eventSource, timestamp_t timeout);

#endif
