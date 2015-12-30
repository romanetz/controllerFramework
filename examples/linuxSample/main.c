#include <platform.h>
#include <queue.h>

int main() {
	QueueClass queue;
	queueInit(&queue, sizeof(char), 100);
	queueWrite(&queue, "Test", 4);
	queueWrite(&queue, "KIVi", 4);
	queueWriteTimeout(&queue, "1234", 3, 0);
	char tmp[4];
	int count;
	while ((count = queueReadTimeout(&queue, tmp, sizeof(tmp) - 1, 0)) > 0) {
		tmp[count] = 0;
		printf("%i, %s\n", count, tmp);
	}
	queueDestroy(&queue);
	return 0;
}
