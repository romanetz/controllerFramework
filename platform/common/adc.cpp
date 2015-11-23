#include <adc.h>

uint32_t ADCChannel::convertAndRead() {
	convert(true);
	return value();
}