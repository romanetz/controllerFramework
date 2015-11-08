#ifndef __HMC5883_H__
#define __HMC5883_H__

#include <SimpleI2CDevice.h>
#include <hmc5883regs.h>

class HMC5883: public SimpleI2CDevice {
	private:
		int16_t _rawValueX;
		
		int16_t _rawValueY;
		
		int16_t _rawValueZ;
		
		float scale;
		
	public:
		HMC5883(I2CBus& i2c, uint8_t addr);
		
		bool detect();
		
		bool setup(HMC5883Mode mode = HMC5883_MODE_CONTINUOS, HMC5883DataRate rate = HMC5883_DATARATE_75HZ, HMC5883Gain gain = HMC5883_GAIN_1_2GA,
			HMC5883MeasurementMode msMode = HMC5883_MODE_NORMAL);
		
		bool setMeasurementMode(HMC5883MeasurementMode mode);
		
		bool setDataRate(HMC5883DataRate rate);
		
		bool setGain(HMC5883Gain gain);
		
		bool setMode(HMC5883Mode mode);
		
		bool updateData();
		
		int16_t rawValueX() { return _rawValueX; };
		int16_t rawValueY() { return _rawValueY; };
		int16_t rawValueZ() { return _rawValueZ; };
		
		float valueX() { return _rawValueX * scale; };
		float valueY() { return _rawValueY * scale; };
		float valueZ() { return _rawValueZ * scale; };
		
};

#endif
