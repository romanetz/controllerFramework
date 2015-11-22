#include <platform.h>
#include <math.h>

STM32RCC rcc(STM32_CLOCKSOURCE_PLL, 8000000, STM32_CLOCKSOURCE_HSE, 168000000);
SysTick sysTick(1000, rcc.systemFrequency());

STM32GPIOPort gpioD(GPIOD);

STM32GPIOPad blueLed(gpioD, 15, GPIO_MODE_ALTFN, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE, STM32_GPIO_SPEED_2MHZ, STM32_AF2);
STM32GPIOPad redLed(gpioD, 14, GPIO_MODE_ALTFN, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE, STM32_GPIO_SPEED_2MHZ, STM32_AF2);
STM32GPIOPad orangeLed(gpioD, 13, GPIO_MODE_ALTFN, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE, STM32_GPIO_SPEED_2MHZ, STM32_AF2);
STM32GPIOPad greenLed(gpioD, 12, GPIO_MODE_ALTFN, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE, STM32_GPIO_SPEED_2MHZ, STM32_AF2);

STM32Timer tim4(TIM4);

int main(void) {
	tim4.setFrequency(500);
	tim4.enable();
	for (int i = 0; i < tim4.channelCount(); i++) {
		tim4.channel(i)->enable();
	}
	float value = 0.0f;
	while (1) {
		for (int i = 0; i < tim4.channelCount(); i++) {
			PWMChannel *pwm = tim4.channel(i);
			pwm->setValue((sinf(value) + 1.0f) * pwm->maxValue() / 2);
		}
		usleep(10);
		value += 3.14f / 10000.0f;
		if (value > 2.0f * 3.14f) value = 0.0f;
	}
	return 0;
}
