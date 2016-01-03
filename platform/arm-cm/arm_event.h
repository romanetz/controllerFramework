#ifndef __ARM_EVENT_H__
#define __ARM_EVENT_H__

#include <event.h>
#include <thread.h>

typedef struct EventListener EventListener;

struct EventSource {
	EventListener *firstListener;
};

struct EventListener {
	EventListener *next;
	Thread *thread;
	BOOL fired;
};

void eventSourceInit(EventSource *eventSource);
void eventSourceFree(EventSource *eventSource);
void eventSourceSendSignal(EventSource *eventSource);
BOOL eventSourceWaitSignalTimeout(EventSource *eventSource, timestamp_t timeout);

#endif
