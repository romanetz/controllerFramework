#ifndef __HMC5883_H__
#define __HMC5883_H__

#include <SimpleI2CDevice.h>
#include <hmc5883regs.h>

class HMC5883: public SimpleI2CDevice {
	private:
		int16_t _rawValueX;
		
		int16_t _rawValueY;
		
		int16_t _rawValueZ;
		
	public:
		HMC5883(I2CBus& i2c, uint8_t addr);
		
		bool detect();
		
		bool setMeasurementMode(HMC5883MeasurementMode mode);
		
		bool setDataRate(HMC5883DataRate rate);
		
		bool setGain(HMC5883Gain gain);
		
		bool setMode(HMC5883Mode mode);
		
		bool updateData();
		
		int16_t rawValueX() { return _rawValueX; };
		int16_t rawValueY() { return _rawValueY; };
		int16_t rawValueZ() { return _rawValueZ; };
		
};

#endif
