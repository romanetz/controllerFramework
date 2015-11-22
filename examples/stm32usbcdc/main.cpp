#include <platform.h>
#include <mpu6050.h>
#include <hmc5883.h>
#include <ms5611.h>
#include <usbcdc.h>
#include "usbdesc.h"

STM32RCC rcc(STM32_CLOCKSOURCE_PLL, 8000000, STM32_CLOCKSOURCE_HSE, 72000000);
SysTick sysTick(1000, rcc.systemFrequency());

STM32GPIOPort gpioE(GPIOE);

STM32GPIOPad led1(gpioE, 0, GPIO_MODE_OUTPUT);
STM32GPIOPad led2(gpioE, 2, GPIO_MODE_OUTPUT);

STM32USBDriver usbDriver(&deviceDescr, usbConfigs, usbStrings, sizeof(usbStrings) / sizeof(usbStrings[0]));
USBCDC usbCdc(usbDriver, 1, 0, 1);

int main(void) {
	led1.clear();
	led2.set();
	usbDriver.connect();
	while (1) {
		led1.toggle();
		led2.toggle();
		char chr;
		if (usbCdc.read(&chr, sizeof(chr))) {
			usbCdc.printf("You are typed: %i\r\n", chr);
		}
	}
	return 0;
}
