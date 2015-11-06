#include <stm32.h>

STM32RCC rcc(STM32_CLOCKSOURCE_PLL, 8000000, STM32_CLOCKSOURCE_HSE, 72000000);
SysTick sysTick(1000, rcc.systemFrequency());

STM32GPIOPort gpioA(GPIOA);
STM32GPIOPort gpioB(GPIOB);
STM32GPIOPort gpioE(GPIOE);

STM32GPIOPad led1(gpioE, 0, GPIO_MODE_OUTPUT);
STM32GPIOPad led2(gpioE, 2, GPIO_MODE_OUTPUT);
STM32GPIOPad button1(gpioB, 10, GPIO_MODE_INPUT, GPIO_DRIVER_NONE, GPIO_PUPD_PULLUP);
STM32GPIOPad button2(gpioB, 11, GPIO_MODE_INPUT, GPIO_DRIVER_NONE, GPIO_PUPD_PULLUP);
STM32GPIOPad usart1tx(gpioA, 9, GPIO_MODE_ALTFN);
STM32GPIOPad usart1rx(gpioA, 10, GPIO_MODE_INPUT);

STM32USART usart1(USART1, STM32_USART_TX_RX, 9600);

int main(void) {
	while (1) {
		led1.toggle();
		usleep(500000);
		led2.toggle();
		usart1.printf("%llu,%i,%i\r\n", SysTick::timestamp(), button1.read(), button2.read());
	}
	return 0;
}

/* STM32RCC rcc(STM32_CLOCKSOURCE_PLL, 8000000, STM32_CLOCKSOURCE_HSE, 168000000);
SysTick sysTick(1000, rcc.systemFrequency());

STM32GPIOPort gpioD(GPIOD);
STM32GPIOPad blueLed(gpioD, 15, GPIO_MODE_OUTPUT);
STM32GPIOPad redLed(gpioD, 14, GPIO_MODE_OUTPUT);
STM32GPIOPad orangeLed(gpioD, 13, GPIO_MODE_OUTPUT);
STM32GPIOPad greenLed(gpioD, 12, GPIO_MODE_OUTPUT);

int main(void) {
	while (1) {
		blueLed.toggle();
		orangeLed.toggle();
		usleep(250000);
		redLed.toggle();
		greenLed.toggle();
	}
	return 0;
} */
