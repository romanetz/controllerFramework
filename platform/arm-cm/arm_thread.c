#include <arm-cm.h>

Thread *currentThread = NULL;
Thread *nextThread;

uint32_t **currentThreadStackPointer;
uint32_t **nextThreadStackPointer;

ISR(PEND_SV_vect) {
	asm volatile(
		"ldr r0, =currentThreadStackPointer\n"
		"ldr r1, [r0, #0]\n"
		"mrs r2, psp\n"
		"str r2, [r1, #0]\n"
		"subs r2, r2, #32\n"
		"stmia r2!, {r4-r11}\n"
		"ldr r0, =nextThreadStackPointer\n"
		"ldr r1, [r0, #0]\n"
		"ldr r2, [r1, #0]\n"
		"subs r2, r2, #32\n"
		"ldmia r2!, {r4-r11}\n"
		"msr psp, r2\n"
		"ldr r0, =nextThread\n"
		"ldr r1, [r0, #0]\n"
		"ldr r0, =currentThread\n"
		"str r1, [r0, #0]\n"
		"ldr lr, =0xFFFFFFFD\n"
		"bx lr\n"
	);
}

ISR(SV_CALL_vect) {
	asm volatile(
		"ldr r0, =currentThreadStackPointer\n"
		"ldr r1, [r0, #0]\n"
		"ldr r2, [r1, #0]\n"
		"msr psp, r2\n"
		"ldr lr, =0xFFFFFFFD\n"
		"bx lr\n"
	);
}

inline void callPendSV(void) {
	(*((volatile uint32_t *)0xE000ED04)) = (1UL << 28UL);
}

inline void switchTask(void) {
	currentThreadStackPointer = &(currentThread->stackPointer);
	nextThreadStackPointer = &(nextThread->stackPointer);
	callPendSV();
}

inline BOOL threadReady(Thread *thread, timestamp_t tickCounter) {
	return (thread->timeout <= tickCounter);
}

void threadSetReady(Thread *thread) {
	thread->timeout = 0;
}

void executeTaskSheduler(void) {
	if (currentThread == NULL) return;
	timestamp_t tickCounter = sysTickGetTickCount();
	int maxPriority = THREAD_PRIORITY_STOPPED;
	Thread *thread = currentThread;
	do {
		thread = thread->next;
		if ((thread->priority > maxPriority) && (threadReady(thread, tickCounter))) {
			maxPriority = thread->priority;
			nextThread = thread;
		}
	} while (thread != currentThread->next);
	if (nextThread) {
		switchTask();
		return;
	}
	asm volatile("cpsid i");
	while (1);
}

static void platform_threadExit(void) {
	currentThread->priority = THREAD_PRIORITY_STOPPED;
	executeTaskSheduler();
}

BOOL threadSleepUntil(timestamp_t timeout) {
	if (currentThread) {
		currentThread->timeout = timeout;
		executeTaskSheduler();
		return TRUE;
	}
	return FALSE;
}

static uint32_t *platform_threadStackInit(uint32_t *stack, ThreadFunc func, void *arg) {
	*(--stack) = 0x01000000; // XPSR
	*(--stack) = (size_t)func; // PC
	*(--stack) = (size_t)platform_threadExit; // LR
	stack -= 4; // R12, R3, R2 and R1
	*(--stack) = (size_t)arg; // R0
	return stack;
}

void platform_threadInit(Thread *thread, ThreadFunc func, void *arg) {
	thread->stack = malloc(thread->stackSize);
	thread->stackPointer = platform_threadStackInit(thread->stack + thread->stackSize / sizeof(uint32_t), func, arg);
	thread->timeout = 0;
	if (currentThread == NULL) {
		thread->next = thread;
		thread->prev = thread;
		currentThread = thread;
		currentThreadStackPointer = &(currentThread->stackPointer);
		asm volatile("msr psp, %0\nldr r0, =_stack\nmsr msp, r0\nsvc 0"::"r"(thread->stackPointer));
		return;
	}
	asm volatile("cpsid i");
	thread->next = currentThread->next;
	thread->prev = currentThread;
	thread->next->prev = thread;
	thread->prev->next = thread;
	asm volatile("cpsie i");
}
