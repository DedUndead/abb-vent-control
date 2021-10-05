#include "SDPSensor.h"
#include "I2C.h"

/**
 * @brief Pressure sensor abstraction.
 * Sensor measures differential pressure.
 * Default parameters are set to match SDP6x0-125Pa sensor
 * @param i2c_           Pointer to I2C interface
 * @param scaling_factor Transfer function scaling factor, Pa-1
 */
SDPSensor::SDPSensor(I2C* i2c_, const int scaling_factor_, const float altitude_corr_) :
		i2c(i2c_),
		scaling_factor(scaling_factor_),
		altitude_corr(altitude_corr_)
{ /* Empty constructor body */ }

/**
 * @brief Read data from SDP sensor
 * Convert raw data to pressure using transfer function
 * @return Pressure on success, SDP error status on fail
 */
int16_t SDPSensor::read()
{
	uint8_t rx_buffer[READ_BUFFER_SIZE];

	// Read I2C status
	uint16_t status = i2c->read(
			DEVICE_ADDRESS, TRIGGER_MEASUREMENT_CMD,
			rx_buffer, READ_BUFFER_SIZE
	);

	if (status == 0x00) {
		return transfer_function(rx_buffer);
	}
	else {
		return ERR;
	}
}

/**
 * @brief Transfer function for SDP
 * Doesn't take tube length into the account
 * @param  reading Raw reading
 * @return         Pressure value derived from raw reading
 */
int16_t SDPSensor::transfer_function(uint8_t* reading)
{
	int16_t pressure;

	// Combine reading bytes
	uint16_t raw = (reading[0] << 8) | reading[1];

	// uint containing two's complement -> int16
	if (raw & 0x8000) {
		raw = -~(raw - 1);
	}
	pressure = raw;

	// Scale value
	pressure /= scaling_factor;

	// Correct considering altitude
	pressure *= altitude_corr;

	return pressure;
}
