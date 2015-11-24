#include <adc.h>

ADCGroup::ADCGroup() : Queue(0, 0) {
	
}

uint32_t ADCChannel::convertAndRead() {
	convert(true);
	return value();
}