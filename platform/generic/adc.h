#ifndef __ADC_H__
#define __ADC_H__

#include <generic.h>

typedef uint8_t adc_channel_index_t;
typedef uint32_t adc_value_t;

#ifdef USE_CLASSES

typedef struct ADCClass ADCClass;

typedef adc_value_t (*ADCClassSingleConversionFunc)(ADCClass *adc, adc_channel_index_t channel);

struct ADCClass {
	ADCClassSingleConversionFunc singleConversion;
};

#define ADC_CLASS(obj) ((ADCClass*)(obj))

#define adcSingleConversion(adc, channel) ADC_CLASS(adc)->singleConversion(ADC_CLASS(adc), channel)

#endif

#endif
