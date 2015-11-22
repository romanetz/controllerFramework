#include <math.h>
#include <stm32.h>

static STM32Timer *tim1;
static STM32Timer *tim2;
static STM32Timer *tim3;
static STM32Timer *tim4;
static STM32Timer *tim5;
static STM32Timer *tim6;
static STM32Timer *tim7;
static STM32Timer *tim8;
static STM32Timer *tim9;
static STM32Timer *tim10;
static STM32Timer *tim11;
static STM32Timer *tim12;
static STM32Timer *tim13;
static STM32Timer *tim14;

static STM32Timer **timerObj(volatile STM32TimerRegs *timer) {
	switch ((uint32_t)timer) {
		case TIM1_BASE:
			return &tim1;
		case TIM2_BASE:
			return &tim2;
		case TIM3_BASE:
			return &tim3;
		case TIM4_BASE:
			return &tim4;
		case TIM5_BASE:
			return &tim5;
		case TIM6_BASE:
			return &tim6;
		case TIM7_BASE:
			return &tim7;
		case TIM8_BASE:
			return &tim8;
		case TIM9_BASE:
			return &tim9;
		case TIM10_BASE:
			return &tim10;
		case TIM11_BASE:
			return &tim11;
		case TIM12_BASE:
			return &tim12;
		case TIM13_BASE:
			return &tim13;
		case TIM14_BASE:
			return &tim14;
		default:
			return NULL;
	}
}

static uint8_t timerIRQ(volatile STM32TimerRegs *timer) {
	switch ((uint32_t)timer) {
		case TIM1_BASE:
		case TIM10_BASE:
			return TIM1_UP_TIM10_IRQ;
		case TIM2_BASE:
			return TIM2_IRQ;
		case TIM3_BASE:
			return TIM3_IRQ;
		case TIM4_BASE:
			return TIM4_IRQ;
		case TIM5_BASE:
			return TIM5_IRQ;
#ifdef TIM6_IRQ
		case TIM6_BASE:
			return TIM6_IRQ;
#endif
		case TIM7_BASE:
			return TIM7_IRQ;
		case TIM8_BASE:
		case TIM13_BASE:
			return TIM8_UP_TIM13_IRQ;
		case TIM9_BASE:
			return TIM1_BRK_TIM9_IRQ;
		case TIM11_BASE:
			return TIM1_TRG_COM_TIM11_IRQ;
		case TIM12_BASE:
			return TIM8_BRK_TIM12_IRQ;
		case TIM14_BASE:
			return TIM8_TRG_COM_TIM14_IRQ;
		default:
			return -1;
	}
}

STM32Timer::STM32Timer(volatile STM32TimerRegs *timer) : _timer(timer) {
	STM32RCC::periphClockEnable(timer);
	memset(channels, 0, sizeof(channels));
	_callback = NULL;
	NVIC::enableIRQ(timerIRQ(timer));
	STM32Timer **objPtr = timerObj(timer);
	if (objPtr) {
		*objPtr = this;
	}
}

STM32Timer::STM32Timer(volatile STM32TimerRegs *timer, int freq, TimerCallback callback, void *arg, bool enable) : _timer(timer) {
	STM32RCC::periphClockEnable(timer);
	memset(channels, 0, sizeof(channels));
	_callback = NULL;
	NVIC::enableIRQ(timerIRQ(timer));
	STM32Timer **objPtr = timerObj(timer);
	if (objPtr) {
		*objPtr = this;
	}
	setFrequency(freq);
	setCallback(callback, arg);
	if (enable) {
		this->enable();
	}
}

STM32Timer::~STM32Timer() {
	int count = channelCount();
	for (int i = 0; i < count; i++) {
		delete channels[i];
	}
	STM32RCC::periphClockDisable(_timer);
	STM32Timer **objPtr = timerObj(_timer);
	if (objPtr) {
		*objPtr = NULL;
	}
}

void STM32Timer::setFrequency(int freq) {
	uint32_t busFreq;
	switch ((uint32_t)_timer) {
		case TIM1_BASE:
		case TIM8_BASE:
		case TIM9_BASE:
		case TIM10_BASE:
		case TIM11_BASE:
			busFreq = STM32RCC::apb2Frequency();
			break;
		default:
			busFreq = STM32RCC::apb1Frequency();
	}
	uint32_t div = busFreq / freq;
	uint32_t prescaler = 1;
	if (div >= 65536) {
		uint32_t maxPrescaler = sqrtf(div);
		if (maxPrescaler < 32768.0f) {
			for (prescaler = 2; prescaler < maxPrescaler; prescaler++) {
				div = (uint16_t)(busFreq / prescaler / freq);
				if ((busFreq / (prescaler * div)) <= (uint32_t)freq) break;
			}
		} else {
			for (div = maxPrescaler; div < 65536; div++) {
				prescaler = (uint16_t)(busFreq / div / freq);
				if ((busFreq / (prescaler * div)) <= (uint32_t)freq) break;
			}
		}
	}
	setFrequency(prescaler - 1, div);
	_frequency = freq;
}

void STM32Timer::setFrequency(uint16_t prescaler, uint16_t reloadValue) {
	_timer->PSC = prescaler;
	_timer->ARR = reloadValue;
}

void STM32Timer::setCallback(TimerCallback callback, void *arg) {
	_timer->DIER &= ~TIM1_DIER_UIE;
	_callback = callback;
	_callbackArg = arg;
	if (_callback) {
		_timer->DIER |= TIM1_DIER_UIE;
	}
}

void STM32Timer::enable() {
	_timer->CR1 |= TIM1_CR1_CEN | TIM1_CR1_ARPE;
}

void STM32Timer::disable() {
	_timer->CR1 &= ~TIM1_CR1_CEN;
}

bool STM32Timer::enabled() {
	return (_timer->CR1 & TIM1_CR1_CEN) != 0;
}

void STM32Timer::interruptHandler() {
	if (_timer->SR & TIM1_SR_UIF) {
		_timer->SR = ~TIM1_SR_UIF;
		if (_callback) {
			if(!_callback(*this, _callbackArg)) {
				disable();
			}
		}
	}
}

int STM32Timer::channelCount() {
	switch ((uint32_t)_timer) {
		case TIM1_BASE:
		case TIM2_BASE:
		case TIM3_BASE:
		case TIM4_BASE:
		case TIM5_BASE:
		case TIM8_BASE:
			return 4;
		case TIM9_BASE:
		case TIM12_BASE:
			return 2;
		case TIM10_BASE:
		case TIM11_BASE:
		case TIM13_BASE:
		case TIM14_BASE:
			return 1;
		default:
			return 0;
	}
}

PWMChannel *STM32Timer::channel(int index) {
	if ((index >= 0) && (index < channelCount())) {
		if (channels[index] == NULL) {
			channels[index] = new STM32TimerChannel(*this, index);
		}
		return channels[index];
	} else {
		return NULL;
	}
}

STM32TimerChannel::STM32TimerChannel(STM32Timer& timer, int channel) : _timer(timer), _index(channel) {}

STM32TimerChannel::~STM32TimerChannel() {
	disable();
}

void STM32TimerChannel::setMode(uint8_t mode) {
	uint32_t reg = _timer.mmioRegs()->CCMR[_index / 2];
	reg &= ~(0xFF << ((_index % 2) * 8));
	reg |= mode << ((_index % 2) * 8);
	_timer.mmioRegs()->CCMR[_index / 2] = reg;
}

void STM32TimerChannel::enable() {
	setMode(TIM1_CCMR1_Output_OC1PE | (6 << TIM1_CCMR1_Output_OC1M_OFFSET));
	_timer.mmioRegs()->CCER |= TIM1_CCER_CC1E << (_index * 4);
}

void STM32TimerChannel::disable() {
	setMode(TIM1_CCMR1_Output_OC1PE | (0 << TIM1_CCMR1_Output_OC1M_OFFSET));
}

bool STM32TimerChannel::enabled() {
	return (((_timer.mmioRegs()->CCMR[_index / 2] >> ((_index % 2) * 8))) & TIM1_CCMR1_Output_OC1M_MASK) != 0;
}

int STM32TimerChannel::value() {
	return _timer.mmioRegs()->CCR[_index];
}

void STM32TimerChannel::setValue(int value) {
	_timer.mmioRegs()->CCR[_index] = value;
}

int STM32TimerChannel::minValue() {
	return 0;
}

int STM32TimerChannel::maxValue() {
	return _timer.reloadValue();
}

ISR(TIM1_UP_TIM10_vect) {
	if (tim1) tim1->interruptHandler();
	if (tim10) tim10->interruptHandler();
}

ISR(TIM2_vect) {
	if (tim2) tim2->interruptHandler();
}

ISR(TIM3_vect) {
	if (tim3) tim3->interruptHandler();
}

ISR(TIM4_vect) {
	if (tim4) tim4->interruptHandler();
}

ISR(TIM5_vect) {
	if (tim5) tim5->interruptHandler();
}

ISR(TIM6_vect) {
	if (tim6) tim6->interruptHandler();
}

ISR(TIM7_vect) {
	if (tim7) tim7->interruptHandler();
}

ISR(TIM8_UP_TIM13_vect) {
	if (tim8) tim8->interruptHandler();
	if (tim13) tim13->interruptHandler();
}

ISR(TIM1_BRK_TIM9_vect) {
	if (tim1) tim1->interruptHandler();
	if (tim9) tim9->interruptHandler();
}

ISR(TIM1_TRG_COM_TIM11_vect) {
	if (tim1) tim1->interruptHandler();
	if (tim11) tim11->interruptHandler();
}

ISR(TIM8_BRK_TIM12_vect) {
	if (tim8) tim8->interruptHandler();
	if (tim12) tim12->interruptHandler();
}

ISR(TIM8_TRG_COM_TIM14_vect) {
	if (tim8) tim8->interruptHandler();
	if (tim14) tim14->interruptHandler();
}
