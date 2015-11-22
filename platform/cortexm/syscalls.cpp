#include <stdlib.h>

extern unsigned _ebss;

ptrdiff_t heapPtr = (ptrdiff_t)&_ebss;

extern "C" void *_sbrk(ptrdiff_t incr) {
	ptrdiff_t oldHeapPtr = heapPtr;
	heapPtr += incr;
	return (void*)oldHeapPtr;
}
