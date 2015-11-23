#include <stm32.h>

static STM32ADC *adc1;
#ifdef ADC2
static STM32ADC *adc2;
#endif
#ifdef ADC3
static STM32ADC *adc3;
#endif

static STM32ADC **adcObj(volatile STM32ADCRegs *adc) {
	switch ((uint32_t)adc) {
		case ADC1_BASE:
			return &adc1;
#ifdef ADC2_BASE
		case ADC2_BASE:
			return &adc2;
#endif
#ifdef ADC3_BASE
		case ADC3_BASE:
			return &adc3;
#endif
		default:
			return NULL;
	}
}

static uint8_t adcIRQ(volatile STM32ADCRegs *adc) {
	switch ((uint32_t)adc) {
		case ADC1_BASE:
			return ADC_IRQ;
#ifdef ADC2_BASE
		case ADC2_BASE:
			return ADC_IRQ;
#endif
#ifdef ADC3_BASE
		case ADC3_BASE:
			return ADC3_IRQ;
#endif
		default:
			return -1;
	}
}

STM32ADC::STM32ADC(volatile STM32ADCRegs *adc) : _adc(adc) {
	STM32RCC::periphClockEnable(adc);
	STM32ADC **obj = adcObj(adc);
	if (obj) {
		*obj = this;
	}
	memset(_channels, 0, sizeof(_channels));
	adc->CR2 = ADC1_CR2_ADON;
	adc->CR2 |= ADC1_CR2_CAL;
	while (adc->CR2 & ADC1_CR2_CAL);
	adc->CR1 = ADC1_CR1_EOCIE;
	adc->CR2 = 0;
	NVIC::enableIRQ(adcIRQ(adc));
}

STM32ADC::~STM32ADC() {
	for (int i = 0; i < channelCount(); i++) {
		delete _channels[i];
	}
	STM32RCC::periphClockDisable(_adc);
	STM32ADC **obj = adcObj(_adc);
	if (obj) {
		*obj = NULL;
	}
}

void STM32ADC::enableTempSensor() {
	_adc->CR2 |= ADC1_CR2_TSVREFE;
}

void STM32ADC::disableTempSensor() {
	_adc->CR2 &= ~ADC1_CR2_TSVREFE;
}

ADCChannel *STM32ADC::channel(int index) {
	if ((index >= 0) && (index < channelCount())) {
		if (_channels[index] == NULL) {
			_channels[index] = new STM32ADCChannel(*this, index);
		}
		return _channels[index];
	}
	return NULL;
}

void STM32ADC::startSingleConversion(STM32ADCChannel& channel) {
	_currentChannel = &channel;
	_adc->CR2 |= (0b111 << ADC1_CR2_EXTSEL_OFFSET) | ADC1_CR2_EXTTRIG | ADC1_CR2_ADON;
	_adc->SQR[0] = 0;
	_adc->SQR[2] = channel.index();
	_adc->CR2 |= ADC1_CR2_SWSTART;
}

void STM32ADC::interruptHandler() {
	if (_adc->SR & ADC1_SR_EOC) {
		uint16_t value = _adc->DR;
		if (_currentChannel) {
			_currentChannel->conversionComplete(value);
			_currentChannel = NULL;
		}
		_adc->SR = ~ADC1_SR_EOC;
	}
}

STM32ADCChannel::STM32ADCChannel(STM32ADC& adc, int index) : _adc(adc), _index(index) {
	_callback = NULL;
}

void STM32ADCChannel::setCallback(ADCChannelCallback callback, void *arg) {
	_callback = callback;
	_callbackArg = arg;
}

int STM32ADCChannel::value() {
	return _value;
}

void STM32ADCChannel::convert(bool wait) {
	_ready = false;
	_adc.startSingleConversion(*this);
	if (wait) {
		while (!_ready);
	}
}

void STM32ADCChannel::conversionComplete(uint16_t value) {
	_value = value;
	_ready = true;
	if (_callback) {
		_callback(*this, _callbackArg);
	}
}

ISR(ADC_vect) {
	if (adc1) adc1->interruptHandler();
#ifdef ADC2_BASE
	if (adc2) adc2->interruptHandler();
#endif
}

#ifdef ADC3_BASE
ISR(ADC3_vect) {
	if (adc3) adc3->interruptHandler();
}
#endif