#include <platform.h>

STM32RCC rcc(STM32_CLOCKSOURCE_PLL, 8000000, STM32_CLOCKSOURCE_HSE, 72000000);
SysTick sysTick(1000, rcc.systemFrequency());

STM32GPIOPort gpioE(GPIOE);

STM32GPIOPad led1(gpioE, 0, GPIO_MODE_OUTPUT);
STM32GPIOPad led2(gpioE, 2, GPIO_MODE_OUTPUT);

STM32Timer tim2(TIM2);

bool blink(Timer& timer, void *arg) {
	led1.toggle();
	led2.toggle();
	return true;
}

int main(void) {
	tim2.setFrequency(2);
	tim2.setCallback(blink);
	led1.clear();
	led2.set();
	tim2.enable();
	while (1);
	return 0;
}
