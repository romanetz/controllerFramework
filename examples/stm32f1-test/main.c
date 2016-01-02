#include <platform.h>
#include <usb-cdc.h>
#include <mpu6050.h>
#include <hmc5883.h>
#include <ms5611.h>
#include "usbdesc.h"

STM32GPIOClass led1, led2;
STM32USARTClass usart1;
STM32I2CClass i2c1;
STM32USBDriverClass usbDriver;
STM32ADCClass adc1;
STM32TimerClass tim1;

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
	stm32_adcClassInit(&adc1, ADC1);
	stm32_adcEnableTempSensor(ADC1);
	stm32_timerClassInit(&tim1, TIM1);
}

USBCDCClass usbCdc;
MPU6050Class mpu6050;
HMC5883Class hmc5883;
MS5611Class ms5611;

void blink(TimerClass *timer, void *arg) {
	gpioToggle(&led1);
}

int main(void) {
	initPeripheral();
	timerSetFrequency(&tim1, 1);
	timerSetCallback(&tim1, blink, NULL);
	timerStart(&tim1);
	serialPortSetup(&usart1, 115200, SERIALPORT_FLOWCONTROL_NONE, SERIALPORT_PARITY_NONE, SERIALPORT_DATA_8BIT, SERIALPORT_STOP_1BIT);
	usbCdcClassInit(&usbCdc, USB_DRIVER_CLASS(&usbDriver), 128, 128, 0x02 | USB_ENDPOINT_OUT, 64, 0x02 | USB_ENDPOINT_IN, 64, 0x01 | USB_ENDPOINT_IN, 8);
	mpu6050_classInit(&mpu6050, I2C_CLASS(&i2c1), MPU6050_DEFAULT_ADDRESS << 1);
	hmc5883_classInit(&hmc5883, I2C_CLASS(&i2c1), HMC5883_I2C_ADDRESS << 1);
	ms5611_classInit(&ms5611, I2C_CLASS(&i2c1), MS5611_DEFAULT_ADDRESS << 1);
	BOOL mpu6050_error = TRUE;
	BOOL hmc5883_error = TRUE;
	BOOL ms5611_error = TRUE;
	while (1) {
		//gpioToggle(&led1);
		usleep(250000);
		gpioToggle(&led2);
		if (mpu6050_error) {
			if (mpu6050_detect(&mpu6050)) {
				ioStreamPrintf(&usbCdc, "MPU6050 detected\r\n");
				mpu6050_error = !mpu6050_setup(&mpu6050, MPU6050_ACCELSCALE_2G, MPU6050_GYROSCALE_250DEG, TRUE);
			}
		}
		if (!mpu6050_error) {
			mpu6050_error = !mpu6050_readData(&mpu6050);
			mpu6050_parseData(&mpu6050);
			ioStreamPrintf(&usbCdc, "Ax=%2.3f,Ay=%2.3f,Az=%2.3f,T=%3.2f\r\n", mpu6050.accelX, mpu6050.accelY, mpu6050.accelZ, mpu6050.temperature);
		} else {
			ioStreamPrintf(&usbCdc, "Failed to detect MPU6050\r\n");
		}
		if (hmc5883_error) {
			if (hmc5883_detect(&hmc5883)) {
				ioStreamPrintf(&usbCdc, "HMC5883 detected\r\n");
				hmc5883_error = !hmc5883_setup(&hmc5883, HMC5883_DATARATE_75HZ, HMC5883_GAIN_1_2GA, HMC5883_MODE_CONTINUOS,
					HMC5883_MODE_NORMAL);
			}
		}
		if (!hmc5883_error) {
			hmc5883_error = !hmc5883_readData(&hmc5883);
			hmc5883_parseData(&hmc5883);
			ioStreamPrintf(&usbCdc, "Mx=%2.3f, My=%2.3f, Mz=%2.3f\r\n", hmc5883.magX, hmc5883.magY, hmc5883.magZ);
		} else {
			ioStreamPrintf(&usbCdc, "Failed to detect HMC5883\r\n");
		}
		if (ms5611_error) {
			if (ms5611_detect(&ms5611)) {
				ioStreamPrintf(&usbCdc, "MS5611 detected\r\n");
				ms5611_error = !ms5611_reset(&ms5611, TRUE);
			}
		}
		if (!ms5611_error) {
			ms5611_error = !ms5611_convert(&ms5611, MS5611_OSR_4096);
			ioStreamPrintf(&usbCdc, "P=%i, T=%3.2f\r\n", ms5611.pressure, ms5611.temperature);
		} else {
			ioStreamPrintf(&usbCdc, "Failed to detect MS5611\r\n");
		}
		ioStreamPrintf(&usbCdc, "Temp = %i, Vref = %i\r\n", adcSingleConversion(&adc1, 16), adcSingleConversion(&adc1, 17));
	}
	return 0;
}
