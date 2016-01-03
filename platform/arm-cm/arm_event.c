#include <stdlib.h>
#include <arm_systick.h>
#include <arm_event.h>

void eventSourceInit(EventSource *eventSource) {
	eventSource->firstListener = NULL;
}

void eventSourceDestroy(EventSource *eventSource) {
	
}

void eventSourceSendSignal(EventSource *eventSource) {
	EventListener *listener;
	while ((listener = eventSource->firstListener) != NULL) {
		eventSource->firstListener = listener->next;
		listener->fired = TRUE;
		threadSetReady(listener->thread);
	}
}

BOOL eventSourceWaitSignalTimeout(EventSource *eventSource, timestamp_t timeout) {
	if (timeout == 0) return FALSE;
	if (currentThread == NULL) return TRUE;
	EventListener listener;
	listener.fired = FALSE;
	listener.thread = currentThread;
	listener.next = eventSource->firstListener;
	eventSource->firstListener = &listener;
	threadSleepUntil(sysTickGetTickCount() + timeout);
	return listener.fired;
}
