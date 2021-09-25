#include <chip.h>
#include <string>
#include "headers/I2C.h"

I2C::I2C(const uint32_t clock_divider_, const uint32_t bitrate_, const uint8_t mode_) :
	clock_divider(clock_divider_),
	bitrate(bitrate_),
	mode(mode_)
{
	init_I2C_PinMux();
	setup_I2C_master();
	NVIC_DisableIRQ(I2C0_IRQn);
}

/**
 * @brief Set up all I2C parameters
 */
void I2C::setup_I2C_master(void)
{
	// Enable I2C clock and reset I2C peripheral
	Chip_I2C_Init(LPC_I2C0);
	// Set up clock rate
	Chip_I2C_SetClockDiv(LPC_I2C0, clock_divider);
	// Set up bitrate
	Chip_I2CM_SetBusSpeed(LPC_I2C0, bitrate);

	Chip_I2CM_Enable(LPC_I2C0);
}

/**
 * @brief Initialize pin muxing for I2C
 */
void I2C::init_I2C_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_DIGMODE_EN | mode);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, IOCON_DIGMODE_EN | mode);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SCL);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SDA);
}

/**
 * @brief Run and execute I2C transaction
 */
void I2C::execute_transaction(uint8_t dev_address,
							  uint8_t *txBuffPtr,
						      uint16_t txSize,
						      uint8_t *rxBuffPtr,
						      uint16_t rxSize)
{
	/* Setup I2C transfer record */
	i2cmXferRec.slaveAddr = dev_address;
	i2cmXferRec.status = 0;
	i2cmXferRec.txSz = txSize;
	i2cmXferRec.rxSz = rxSize;
	i2cmXferRec.txBuff = txBuffPtr;
	i2cmXferRec.rxBuff = rxBuffPtr;

	Chip_I2CM_XferBlocking(LPC_I2C0, &i2cmXferRec);
}

/**
 * @brief Read from slave's register using I2C
 * @param dev_address     Address of slave
 * @param target_register Register to read from
 * @param read_buffer     Buffer to save read data, rx
 * @param buffer_size     Size of rx buffer
 * @return                Status of transaction
 */
uint16_t I2C::read(uint8_t dev_address, uint8_t target_register, uint8_t* read_buffer, uint16_t buffer_size)
{
	execute_transaction(
			dev_address,
			&target_register, 1,
			read_buffer, buffer_size
	);

	return i2cmXferRec.status;
}

/**
 * @brief Write to slave's register using I2C
 * @param dev_address     Address of slave
 * @param target_register Register to write to
 * @param write_buffer    Data to write
 * @param buffer_size     Size of data in bytes
 * @return                Status of transaction
 */
uint16_t I2C::write(uint8_t dev_address, uint8_t target_register, uint8_t* write_buffer, uint16_t buffer_size)
{
	// Create transmit buffer
	uint8_t tx_buffer[MAX_LENGTH_BUFFER];
	tx_buffer[0] = target_register; // Put register address as a first byte

	// Append data to the rest of transmit buffer
	for (int i = 1; i <= buffer_size; i++) {
		tx_buffer[i] = write_buffer[i - 1];
	}

	execute_transaction(
			dev_address,
			tx_buffer, 1 + buffer_size,
			NULL, 0
	);

	return i2cmXferRec.status;
}
