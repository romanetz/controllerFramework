#ifndef __GPIO_H__
#define __GPIO_H__

typedef enum GPIOMode {
	GPIO_MODE_INPUT = 0,
	GPIO_MODE_ANALOG,
	GPIO_MODE_OUTPUT,
	GPIO_MODE_ALTFN
} GPIOMode;

typedef enum GPIODriverType {
	GPIO_DRIVER_NONE = 0,
	GPIO_DRIVER_PUSHPULL = 0,
	GPIO_DRIVER_OPENDRAIN
} GPIODriverType;

typedef enum GPIOPuPd {
	GPIO_PUPD_NONE = 0,
	GPIO_PUPD_PULLUP,
	GPIO_PUPD_PULLDOWN
} GPIOPuPd;

typedef enum GPIOValue {
	GPIO_LOW = 0,
	GPIO_HIGH
} GPIOValue;

typedef enum GPIOInterruptMode {
	GPIO_INTERRUPT_NONE = 0,
	GPIO_INTERRUPT_RISING = 1,
	GPIO_INTERRUPT_FALLING = 2,
	GPIO_INTERRUPT_ANY = GPIO_INTERRUPT_RISING | GPIO_INTERRUPT_FALLING
} GPIOInterruptMode;

#ifdef USE_CLASSES

typedef struct GPIOClass GPIOClass;

typedef void (*GPIOInterruptHandler)(void *arg);

typedef void (*GPIOSetupFunc)(GPIOClass *gpio, GPIOMode mode, GPIODriverType driver, GPIOPuPd pupd);
typedef void (*GPIOSetFunc)(GPIOClass *gpio);
typedef void (*GPIOClearFunc)(GPIOClass *gpio);
typedef void (*GPIOToggleFunc)(GPIOClass *gpio);
typedef uint8_t (*GPIOReadFunc)(GPIOClass *gpio);
typedef uint8_t (*GPIOAttachInterruptFunc)(GPIOClass *gpio, GPIOInterruptMode mode, GPIOInterruptHandler handler, void *arg);

struct GPIOClass {
	GPIOSetupFunc setup;
	GPIOSetFunc set;
	GPIOClearFunc clear;
	GPIOToggleFunc toggle;
	GPIOReadFunc read;
	GPIOAttachInterruptFunc attachInterrupt;
};

#define GPIO_CLASS(obj) ((GPIOClass*)(obj))

#define gpioSetup(gpio, mode, driver, pupd) GPIO_CLASS(gpio)->setup(GPIO_CLASS(gpio), mode, driver, pupd)
#define gpioSimpleSetup(gpio, mode) gpioSetup(gpio, mode, GPIO_DRIVER_PUSHPULL, GPIO_PUPD_NONE)
#define gpioSet(gpio) GPIO_CLASS(gpio)->set(GPIO_CLASS(gpio))
#define gpioClear(gpio) GPIO_CLASS(gpio)->clear(GPIO_CLASS(gpio))
#define gpioToggle(gpio) GPIO_CLASS(gpio)->toggle(GPIO_CLASS(gpio))
#define gpioRead(gpio) GPIO_CLASS(gpio)->read(GPIO_CLASS(gpio))
#define gpioWrite(gpio, value) do { if (value) { gpioSet(gpio); } else { gpioClear(gpio); } while(0)
#define gpioAttachInterrupt(gpio, mode, handler, arg) (GPIO_CLASS(gpio)->attachInterrupt(GPIO_CLASS(gpio), mode, handler, arg))

#endif

#endif
