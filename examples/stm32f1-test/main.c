#include <platform.h>
#include <usb-cdc.h>
#include <mpu6050.h>
#include "usbdesc.h"

STM32GPIOClass led1, led2;
STM32USARTClass usart1;
STM32I2CClass i2c1;
STM32USBDriverClass usbDriver;

void initPeripheral() {
	stm32_rccUsePLL(STM32_CLOCKSOURCE_HSE, 8000000, 72000000);
	sysTickSetup(1000);
	stm32_gpioPortEnable(GPIOA);
	stm32_gpioPortSetup(GPIOA, STM32_GPIO9, GPIO_MODE_ALTFN, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE);
	stm32_gpioPortSetup(GPIOA, STM32_GPIO10, GPIO_MODE_INPUT, GPIO_DRIVER_NONE, GPIO_PUPD_PULLUP);
	stm32_gpioPortEnable(GPIOB);
	stm32_gpioPortSetup(GPIOB, STM32_GPIO6 | STM32_GPIO7, GPIO_MODE_ALTFN, GPIO_DRIVER_OPENDRAIN, GPIO_PUPD_PULLUP);
	stm32_gpioPortEnable(GPIOE);
	stm32_gpioPortSetup(GPIOE, STM32_GPIO0 | STM32_GPIO2, GPIO_MODE_OUTPUT, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE);
	stm32_gpioClassInit(&led1, GPIOE, STM32_GPIO0);
	stm32_gpioClassInit(&led2, GPIOE, STM32_GPIO2);
	stm32_usartClassInit(&usart1, USART1, 64, 64);
	stm32_i2cClassInit(&i2c1, I2C1, 400000);
	stm32_usbDriverClassInit(&usbDriver, &usbDeviceDescr, usbConfigs, usbStrings, 3);
}

USBCDCClass usbCdc;
MPU6050Class mpu6050;

int main(void) {
	initPeripheral();
	serialPortSetup(&usart1, 115200, SERIALPORT_FLOWCONTROL_NONE, SERIALPORT_PARITY_NONE, SERIALPORT_DATA_8BIT, SERIALPORT_STOP_1BIT);
	usbCdcClassInit(&usbCdc, USB_DRIVER_CLASS(&usbDriver), 128, 128, 0x02 | USB_ENDPOINT_OUT, 64, 0x02 | USB_ENDPOINT_IN, 64, 0x01 | USB_ENDPOINT_IN, 8);
	mpu6050_classInit(&mpu6050, I2C_CLASS(&i2c1), MPU6050_DEFAULT_ADDRESS << 1);
	BOOL mpu6050_error = TRUE;
	while (1) {
		gpioToggle(&led1);
		usleep(250000);
		gpioToggle(&led2);
		if (mpu6050_error && mpu6050_detect(&mpu6050)) {
			ioStreamPrintf(&usbCdc, "MPU6050 detected\r\n");
			mpu6050_error = !mpu6050_powerOn(&mpu6050);
		}
		if (!mpu6050_error) {
			mpu6050_readData(&mpu6050);
			ioStreamPrintf(&usbCdc, "Ax=%6i,Ay=%i,Az=%i,Gx=%i,Gy=%i,Gz=%i,T=%i\r\n", mpu6050.rawData.accelX, mpu6050.rawData.accelY,
				mpu6050.rawData.accelZ, mpu6050.rawData.gyroX, mpu6050.rawData.gyroY, mpu6050.rawData.gyroZ, mpu6050.rawData.temperature);
		} else {
			ioStreamPrintf(&usbCdc, "Failed to detect MPU6050\r\n");
		}
	}
	return 0;
}
