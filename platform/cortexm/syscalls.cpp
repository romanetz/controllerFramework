#include <stdlib.h>

extern unsigned _ebss;

extern "C" void *_sbrk(ptrdiff_t incr) {
	static ptrdiff_t heapPtr = (ptrdiff_t)&_ebss;
	ptrdiff_t oldHeapPtr = heapPtr;
	heapPtr += incr;
	return (void*)oldHeapPtr;
}
