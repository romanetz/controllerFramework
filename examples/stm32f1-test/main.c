#include <platform.h>
#include <usb-cdc.h>
#include "usbdesc.h"

STM32GPIOClass led1, led2;
STM32USARTClass usart1;
STM32USBDriverClass usbDriver;
USBCDCClass usbCdc;

void initPeripheral() {
	stm32_rccUsePLL(STM32_CLOCKSOURCE_HSE, 8000000, 72000000);
	sysTickSetup(1000);
	stm32_gpioPortEnable(GPIOA);
	stm32_gpioPortSetup(GPIOA, STM32_GPIO9, GPIO_MODE_ALTFN, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE);
	stm32_gpioPortSetup(GPIOA, STM32_GPIO10, GPIO_MODE_INPUT, GPIO_DRIVER_NONE, GPIO_PUPD_PULLUP);
	stm32_gpioPortEnable(GPIOE);
	stm32_gpioPortSetup(GPIOE, STM32_GPIO0 | STM32_GPIO2, GPIO_MODE_OUTPUT, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE);
	stm32_gpioClassInit(&led1, GPIOE, STM32_GPIO0);
	stm32_gpioClassInit(&led2, GPIOE, STM32_GPIO2);
	stm32_usartClassInit(&usart1, USART1, 64, 64);
	stm32_usbDriverClassInit(&usbDriver, &usbDeviceDescr, usbConfigs, usbStrings, 3);
}

int main(void) {
	initPeripheral();
	serialPortSetup(&usart1, 115200, SERIALPORT_FLOWCONTROL_NONE, SERIALPORT_PARITY_NONE, SERIALPORT_DATA_8BIT, SERIALPORT_STOP_1BIT);
	usbCdcClassInit(&usbCdc, USB_DRIVER_CLASS(&usbDriver), 128, 128, 0x02 | USB_ENDPOINT_OUT, 64, 0x02 | USB_ENDPOINT_IN, 64, 0x01 | USB_ENDPOINT_IN, 8);
	int i = 0;
	while (1) {
		gpioToggle(&led1);
		usleep(250000);
		gpioToggle(&led2);
		ioStreamPrintf(&usart1, "Test: %i\r\n", i++);
		ioStreamPrintf(&usbCdc, "Test: %i\r\n", i++);
	}
	return 0;
}
