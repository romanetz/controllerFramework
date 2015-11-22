#include <platform.h>
#include <mpu6050.h>
#include <hmc5883.h>
#include <ms5611.h>

STM32RCC rcc(STM32_CLOCKSOURCE_PLL, 8000000, STM32_CLOCKSOURCE_HSE, 72000000);
SysTick sysTick(1000, rcc.systemFrequency());

STM32GPIOPort gpioA(GPIOA);
STM32GPIOPort gpioB(GPIOB);
STM32GPIOPort gpioE(GPIOE);

STM32GPIOPad led1(gpioE, 0, GPIO_MODE_OUTPUT);
STM32GPIOPad led2(gpioE, 2, GPIO_MODE_OUTPUT);
STM32GPIOPad i2c1scl(gpioB, 6, GPIO_MODE_ALTFN, GPIO_DRIVER_OPENDRAIN, GPIO_PUPD_PULLUP);
STM32GPIOPad i2c1sda(gpioB, 7, GPIO_MODE_ALTFN, GPIO_DRIVER_OPENDRAIN, GPIO_PUPD_PULLUP);
STM32GPIOPad button1(gpioB, 10, GPIO_MODE_INPUT, GPIO_DRIVER_NONE, GPIO_PUPD_PULLUP);
STM32GPIOPad button2(gpioB, 11, GPIO_MODE_INPUT, GPIO_DRIVER_NONE, GPIO_PUPD_PULLUP);
STM32GPIOPad usart1tx(gpioA, 9, GPIO_MODE_ALTFN);
STM32GPIOPad usart1rx(gpioA, 10, GPIO_MODE_INPUT);

STM32I2CDriver i2c1(I2C1, 400000);
STM32USART usart1(USART1, 64, 64, STM32_USART_TX_RX, 115200);

MPU6050 mpu6050(i2c1, MPU6050_DEFAULT_ADDRESS);
HMC5883 hmc5883(i2c1, HMC5883_I2C_ADDRESS);
MS5611 ms5611(i2c1, MS5611_DEFAULT_ADDRESS);

volatile int counter;

void incCounter(void *arg) {
	(void)arg;
	counter++;
}

void decCounter(void *arg) {
	(void)arg;
	counter--;
}

int main(void) {
	led1.clear();
	led2.clear();
	bool mpu6050error = true, hmc5883error = true, ms5611error = true;
	button1.attachInterrupt(GPIO_INTERRUPT_FALLING, incCounter, NULL);
	button2.attachInterrupt(GPIO_INTERRUPT_FALLING, decCounter, NULL);
	while (1) {
		led1.toggle();
		usleep(500000);
		led2.toggle();
		char chr;
		if (usart1.read(&chr, sizeof(chr), 0) >= (int)sizeof(chr)) {
			usart1.printf("YOUR ARE TYPED: %i\r\n", chr);
		}
		usart1.printf("BUTTON1=%i BUTTON2=%i COUNTER=%i\r\n", button1.read(), button2.read(), counter);
		if (mpu6050error && mpu6050.detect()) {
			mpu6050error = !mpu6050.setup();
		} else if (!mpu6050error) {
			mpu6050error = !mpu6050.updateData();
		}
		if (!mpu6050error) {
			usart1.printf("MPU6050: AX=%2.3f,AY=%2.3f,AZ=%2.3f,GX=%2.3f,GY=%3.0f,GZ=%3.0f,T=%3.0f\r\n",
				mpu6050.accelX(), mpu6050.accelY(), mpu6050.accelZ(),
				mpu6050.gyroX(), mpu6050.gyroY(), mpu6050.gyroZ(), mpu6050.temperature());
		}
		if (hmc5883error && hmc5883.detect()) {
			hmc5883error = !hmc5883.setup();
		} else if (!hmc5883error) {
			hmc5883error = !hmc5883.updateData();
		}
		if (!hmc5883error) {
			usart1.printf("HMC5883: MX=%2.3f,MY=%2.3f,MZ=%2.3f\r\n", hmc5883.valueX(), hmc5883.valueY(), hmc5883.valueZ());
		}
		if (ms5611error && ms5611.detect()) {
			ms5611error = !ms5611.setup();
		} else if (!ms5611error) {
			ms5611error = !ms5611.updateData();
		}
		if (!ms5611error) {
			usart1.printf("MS5611: P=%u,T=%i\r\n", ms5611.pressure(), ms5611.temperature());
		}
	}
	return 0;
}
