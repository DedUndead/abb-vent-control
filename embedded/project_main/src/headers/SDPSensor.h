#ifndef HEADERS_PRESSURESENSOR_H_
#define HEADERS_PRESSURESENSOR_H_

#define TRIGGER_MEASUREMENT_CMD 0xF1
#define DEVICE_ADDRESS			0x40
#define READ_BUFFER_SIZE        3
#define SDP6X0125_SCALING       240
#define DEFAULT_ALT_FACTOR      0.95
#define ERR					    0x1F5

#include "I2C.h"

class SDPSensor {
public:
	SDPSensor(
		I2C* i2c_,
		const int scaling_factor_ = SDP6X0125_SCALING,
		const float altitude_corr_ = DEFAULT_ALT_FACTOR
	);
	int16_t read();
private:
	I2C* i2c;
	const int scaling_factor;
	const float altitude_corr;

	int16_t transfer_function(uint8_t* reading);
};


#endif
