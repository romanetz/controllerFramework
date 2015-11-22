/**
	\addtogroup platform Platform-independent
	@{
	\addtogroup GPIO GPIO
	@{
*/
#ifndef __GPIO_H__
#define __GPIO_H__

/**
	\brief Set of GPIO bits.
*/
typedef uint32_t GPIOBitMask;

/**
	\brief Index of one GPIO bit.
*/
typedef uint8_t GPIOBitIndex;

/**
	\brief Mode of GPIO bits.
	Usage of GPIO_MODE_ANALOG_INPUT and GPIO_MODE_ALTFN are platform-specific.
*/
enum GPIOBitMode {
	GPIO_MODE_INPUT = 0,
	GPIO_MODE_ANALOG_INPUT,
	GPIO_MODE_OUTPUT,
	GPIO_MODE_ALTFN
};

/**
	\brief Driver used for GPIO bits.
	Not all GPIO implementations supports all driver types.
*/
enum GPIOBitDriver {
	GPIO_DRIVER_PUSHPULL = 0,
	GPIO_DRIVER_OPENDRAIN,
	
	GPIO_DRIVER_NONE = 0
};

/**
	\brief Enable pullup or pulldown resistors for GPIO bits.
	Not all GPIO implementation supports this.
*/
enum GPIOPuPdMode {
	GPIO_PUPD_NONE = 0,
	GPIO_PUPD_PULLUP,
	GPIO_PUPD_PULLDOWN
};

/**
	\brief State of one bit of GPIO port.
*/
enum GPIOBitValue {
	GPIO_PAD_LOW = false,
	GPIO_PAD_HIGH = true
};

/**
	\brief Triggers for GPIO interrupt.
*/
enum GPIOInterruptMode {
	GPIO_INTERRUPT_NONE = 0, ///< Interrupt disabled.
	GPIO_INTERRUPT_RISING = 1 << 0, ///< Interrupt on a rising edge.
	GPIO_INTERRUPT_FALLING = 1 << 1, ///< Interrupt on a falling edge.
	GPIO_INTERRUPT_CHANGE = GPIO_INTERRUPT_RISING | GPIO_INTERRUPT_FALLING ///< Interrupt on both edges.
};

/**
	\brief Interrupt handler for GPIO.
*/
typedef void (*GPIOInterruptHandler)(void *arg);

/**
	\brief Abstract class for GPIO port (many GPIO pads).
*/
class GPIOPort {
	public:
		virtual ~GPIOPort() {};
		
		/**
			\brief Set options for some bits of GPIO port.
			\param[in] bits Set of GPIO bits.
			\param[in] mode Mode that need to set.
			\param[in] driver Output driver that will be used.
			\param[in] pupd - Pullup/pulldown mode.
		*/
		virtual void setup(GPIOBitMask bits, GPIOBitMode mode, GPIOBitDriver driver = GPIO_DRIVER_PUSHPULL, GPIOPuPdMode pupd = GPIO_PUPD_NONE) = 0;
		
		/**
			\brief Write all bits of GPIO port.
			\param[in] data State of all GPIO bits that will be set.
		*/
		virtual void write(GPIOBitMask data) = 0;
		
		/**
			\brief Set some bits of GPIO port to HIGH state.
			This bits should be in output mode.
			\param[in] bits Set of GPIO bits that will be changed.
		*/
		virtual void setBits(GPIOBitMask bits) = 0;
		
		/**
			\brief Set some bits of GPIO port to LOW state.
			This bits should be in output mode.
			\param[in] bits Set of GPIO bits that will be changed.
		*/
		virtual void clearBits(GPIOBitMask bits) = 0;
		
		/**
			\brief Toggle some bits of GPIO port.
			This bits should be in output mode.
			\param[in] bits Set of GPIO bits that will be changed.
		*/
		virtual void toggleBits(GPIOBitMask bits) = 0;
		
		/**
			\brief Read all bits of GPIO port.
		*/
		virtual GPIOBitMask read() = 0;
		
		/**
			\brief Read some bits of GPIO port.
			\param[in] bits Set of GPIO bits that will be returned.
		*/
		virtual GPIOBitMask readBits(GPIOBitMask bits) = 0;
		
		/**
			\brief Set one bit of GPIO port to HIGH state.
			\param[in] pad Index of bit that will be changed.
		*/
		void setBit(GPIOBitIndex pad) {
			setBits(1 << pad);
		}
		
		/**
			\brief Set one bit of GPIO port to LOW state.
			\param[in] pad Index of bit that will be changed.
		*/
		void clearBit(GPIOBitIndex pad) {
			clearBits(1 << pad);
		}
		
		/**
			\brief Toggle one bit of GPIO port.
			\param[in] pad Index of bit that will be changed.
		*/
		void toggleBit(GPIOBitIndex pad) {
			toggleBits(1 << pad);
		}
		
		/**
			\brief Set one bit of GPIO port to specified state (LOW or HIGH).
			\param[in] pad Index of bit that will be changed.
			\param[in] value New state.
		*/
		void writeBit(GPIOBitIndex pad, GPIOBitValue value) {
			if (value == GPIO_PAD_LOW) {
				clearBit(pad);
			} else {
				setBit(pad);
			}
		}
		
		/**
			\brief Read one bit of GPIO port.
			\param[in] pad Index of bit which state will be returned.
		*/
		GPIOBitValue readBit(GPIOBitIndex pad) {
			return (readBits(1 << pad) == 0) ? GPIO_PAD_LOW : GPIO_PAD_HIGH;
		}
		
		/**
			\brief Attach interrupt handler to GPIO bit.
			\param[in] pad Index of bit which should generate interrupt.
			\param[in] mode Conditions that generates interrupt.
			\param[in] handler Callback.
			\param[in] arg Callback argument.
			\return Returns true if bit supports interrupt handlers.
		*/
		virtual bool attachInterrupt(GPIOBitIndex pad, GPIOInterruptMode mode, GPIOInterruptHandler handler, void *arg) {
			return false;
		}
};

/**
	\brief Absract GPIO pad.
*/
class GPIOPad {
	public:
		/**
			\brief Set GPIO pad options.
		*/
		virtual void setup(GPIOBitMode mode, GPIOBitDriver driver = GPIO_DRIVER_PUSHPULL, GPIOPuPdMode pupd = GPIO_PUPD_NONE) = 0;
		
		/**
			\brief Set GPIO pad to HIGH state.
		*/
		virtual void set() = 0;
		
		/**
			\brief Set GPIO pad to LOW state.
		*/
		virtual void clear() = 0;
		
		/**
			\brief Toggle GPIO pad state.
		*/
		virtual void toggle() = 0;
		
		/**
			\brief Read GPIO pad state.
		*/
		virtual GPIOBitValue read() = 0;
		
		/**
			\brief Set GPIO pad to specified state (LOW or HIGH).
		*/
		void write(GPIOBitValue value) {
			if (value == GPIO_PAD_LOW) {
				clear();
			} else {
				set();
			}
		}
		
		/**
			\brief Attach interrupt handler to GPIO pad.
			\param[in] mode Conditions that generates interrupt.
			\param[in] handler Callback.
			\param[in] arg Callback argument.
			\return Returns true if bit supports interrupt handlers.
		*/
		virtual bool attachInterrupt(GPIOInterruptMode mode, GPIOInterruptHandler handler, void *arg) {
			return false;
		}
};

/**
	\brief A class representing a single bit of GPIOPort as an independent object.
*/
class GPIOBit : public GPIOPad {
	private:
		GPIOPort& _port;
		const GPIOBitIndex _bit;
	
	public:
		GPIOBit(GPIOPort& port, GPIOBitIndex bit) : _port(port), _bit(bit) {}
		
		GPIOBit(GPIOPort& port, GPIOBitIndex bit, GPIOBitMode mode, GPIOBitDriver driver = GPIO_DRIVER_PUSHPULL,
				GPIOPuPdMode pupd = GPIO_PUPD_NONE) : _port(port), _bit(bit) {
			setup(mode, driver, pupd);
		}
		
		void setup(GPIOBitMode mode, GPIOBitDriver driver = GPIO_DRIVER_PUSHPULL, GPIOPuPdMode pupd = GPIO_PUPD_NONE) {
			_port.setup(1 << _bit, mode, driver, pupd);
		}
		
		void set() {
			_port.setBit(_bit);
		}
		
		void clear() {
			_port.clearBit(_bit);
		}
		
		void toggle() {
			_port.toggleBit(_bit);
		}
		
		GPIOBitValue read() {
			return _port.readBit(_bit);
		}
		
		bool attachInterrupt(GPIOInterruptMode mode, GPIOInterruptHandler handler, void *arg) {
			return _port.attachInterrupt(_bit, mode, handler, arg);
		}
		
};

#endif
/** @}
@} */
