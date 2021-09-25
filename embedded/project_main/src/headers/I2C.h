#ifndef SRC_HEADERS_I2C_H_
#define SRC_HEADERS_I2C_H_


#include <chip.h>
#include <string>

#define MAX_LENGTH_BUFFER 10

/**
 * @brief I2C interface abstraction
 * This implementation currently handles devices with 8-bit long registers.
 * Overloads will be added later if needed.
 */
class I2C {
public:
	I2C(const uint32_t clock_divider_, const uint32_t bitrate_, const uint8_t mode_);
	uint16_t read(uint8_t dev_address, uint8_t target_register, uint8_t* read_buffer, uint16_t buffer_size);
	uint16_t write(uint8_t dev_address, uint8_t target_register, uint8_t* write_buffer, uint16_t buffer_size);
private:
	void setup_I2C_master(void);
	void init_I2C_PinMux(void);
	void execute_transaction(uint8_t dev_address,
							 uint8_t *txBuffPtr,
							 uint16_t txSize,
							 uint8_t *rxBuffPtr,
							 uint16_t rxSize);

	I2CM_XFER_T i2cmXferRec;
	const uint32_t clock_divider;
	const uint32_t bitrate;
	const uint8_t mode;
};


#endif /* SRC_HEADERS_I2C_H_ */
