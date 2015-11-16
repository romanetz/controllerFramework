#include <platform.h>

STM32RCC rcc(STM32_CLOCKSOURCE_PLL, 8000000, STM32_CLOCKSOURCE_HSE, 168000000);
SysTick sysTick(1000, rcc.systemFrequency());

STM32GPIOPort gpioA(GPIOA);
STM32GPIOPort gpioD(GPIOD);

STM32GPIOPad blueLed(gpioD, 15, GPIO_MODE_OUTPUT);
STM32GPIOPad redLed(gpioD, 14, GPIO_MODE_OUTPUT);
STM32GPIOPad orangeLed(gpioD, 13, GPIO_MODE_OUTPUT);
STM32GPIOPad greenLed(gpioD, 12, GPIO_MODE_OUTPUT);
STM32GPIOPad button(gpioA, 0, GPIO_MODE_INPUT, GPIO_DRIVER_NONE, GPIO_PUPD_PULLDOWN);

volatile bool pause = false;

void buttonPressed(void *arg) {
	pause = !pause;
}

int main() {
	button.attachInterrupt(GPIO_INTERRUPT_FALLING, buttonPressed, 0);
	while (1) {
		blueLed.toggle();
		orangeLed.toggle();
		usleep(250000);
		greenLed.toggle();
		redLed.toggle();
		while (pause);
	}
}