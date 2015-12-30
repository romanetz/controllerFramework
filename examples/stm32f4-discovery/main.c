#include <platform.h>
#include "usbdesc.h"

STM32GPIOClass greenLed, orangeLed, redLed, blueLed;
STM32USBDriverClass usbDriver;
STM32USARTClass serialPort;

void initPeripheral() {
	stm32_rccUsePLL(STM32_CLOCKSOURCE_HSE, 8000000, 168000000);
	sysTickSetup(1000);
	stm32_gpioPortEnable(GPIOA);
	stm32_gpioPortSetupAltFn(GPIOA, STM32_GPIO0 | STM32_GPIO1, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE, 8);
	stm32_gpioPortSetupAltFn(GPIOA, STM32_GPIO9 | STM32_GPIO11 | STM32_GPIO12, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE, 10);
	stm32_gpioPortEnable(GPIOD);
	stm32_gpioPortSetup(GPIOD, STM32_GPIO12 | STM32_GPIO13 | STM32_GPIO14 | STM32_GPIO15, GPIO_MODE_OUTPUT, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE);
	stm32_gpioClassInit(&greenLed, GPIOD, STM32_GPIO12);
	stm32_gpioClassInit(&orangeLed, GPIOD, STM32_GPIO13);
	stm32_gpioClassInit(&redLed, GPIOD, STM32_GPIO14);
	stm32_gpioClassInit(&blueLed, GPIOD, STM32_GPIO15);
	stm32_usartClassInit(&serialPort, UART4, 1024, 1024);
	serialPortSetup(&serialPort, 115200, SERIALPORT_FLOWCONTROL_NONE, SERIALPORT_PARITY_NONE, SERIALPORT_DATA_8BIT, SERIALPORT_STOP_1BIT);
	stm32_usbDriverClassInit(&usbDriver, &usbDeviceDescr, usbConfigs, usbStrings, 3);
}

int main(void) {
	initPeripheral();
	while (1) {
		gpioToggle(&greenLed);
		gpioToggle(&redLed);
		usleep(250000);
		gpioToggle(&orangeLed);
		gpioToggle(&blueLed);
		//ioStreamPrintf(&serialPort, "%i\r\n", stm32_rccAHBFrequency());
	}
	return 0;
}
