#ifndef __EVENT_H__
#define __EVENT_H__

#include <generic.h>

typedef struct EventSource EventSource;

#include <platform_event.h>

EventSource *eventSourceCreate(void);
void eventSourceDestroy(EventSource *eventSource);

void eventSourceInit(EventSource *eventSource);
void eventSourceFree(EventSource *eventSource);
void eventSourceSendSignal(EventSource *eventSource);
BOOL eventSourceWaitSignalTimeout(EventSource *eventSource, timestamp_t timeout);
#define eventSourceWaitSignal(eventSource) eventSourceWaitSignalTimeout(eventSource, -1)

#endif
