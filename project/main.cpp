#include <platform.h>
#include <mpu6050.h>

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
STM32USART usart1(USART1, STM32_USART_TX_RX, 9600);

MPU6050 mpu6050(i2c1, MPU6050_DEFAULT_ADDRESS);

int main(void) {
	led1.clear();
	led2.clear();
	usleep(1000000);
	while (1) {
		led1.toggle();
		usleep(500000);
		led2.toggle();
		usart1.printf("BUTTON1=%i BUTTON2=%i ", button1.read(), button2.read());
		if (mpu6050.detect()) {
			mpu6050.powerOn();
			mpu6050.updateData();
			usart1.printf("AX=%6i,AY=%6i,AZ=%6i,GX=%6i,GY=%6i,GZ=%6i,T=%6i", mpu6050.rawAccelX(), mpu6050.rawAccelY(), mpu6050.rawAccelZ(),
				mpu6050.rawGyroX(), mpu6050.rawGyroY(), mpu6050.rawGyroZ(), mpu6050.temperature());
		}
		usart1.write("\r\n");
	}
	return 0;
}
