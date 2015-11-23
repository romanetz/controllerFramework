#include <platform.h>

STM32RCC rcc(STM32_CLOCKSOURCE_PLL, 8000000, STM32_CLOCKSOURCE_HSE, 72000000);
SysTick sysTick(1000, rcc.systemFrequency());

STM32GPIOPort gpioA(GPIOA);
STM32GPIOPort gpioE(GPIOE);
STM32ADC adc1(ADC1);

STM32GPIOPad led1(gpioE, 0, GPIO_MODE_OUTPUT);
STM32GPIOPad led2(gpioE, 2, GPIO_MODE_OUTPUT);
STM32GPIOPad usart1tx(gpioA, 9, GPIO_MODE_ALTFN);
STM32GPIOPad usart1rx(gpioA, 10, GPIO_MODE_INPUT);

STM32USART usart1(USART1, 64, 64, STM32_USART_TX_RX, 115200);

int main(void) {
	adc1.enableTempSensor();
	led1.clear();
	led2.set();
	while (1) {
		led1.toggle();
		led2.toggle();
		usleep(250000);
		usart1.printf("Temp = %i, Vref = %i\r\n", adc1.channel(16)->convertAndRead(), adc1.channel(17)->convertAndRead());
	}
	return 0;
}
