#include "chip.h"
#include "delay.h"
#include "AbbDrive.h"

/**
 * @brief ABB Drive class encapsulates frequency control
 * Modbus interface is utilized
 */
AbbDrive::AbbDrive() :
	node(SLAVE_ID),
	control_word(&node, CONTROL_WORD_ADDRESS),
	frequency(&node, FREQ_CONTROL_ADDRESS),
	status_word(&node, STATUS_WORD_ADDRESS),
	output_freq(&node, OUTPUT_FREQ_ADDRESS),
	current(&node, CURRENT_ADDRESS)
{
	node.begin(TRANSMISSION_RATE);

	// Simulator allows to combine several startup states into one assignment
	if ((status_word & 1) == 0) {
		// Prepare for starting
		control_word = CMD_PREP;
		delay_systick(BREAK_TIME);

		// Set to start mode
		control_word = CMD_START;
		delay_systick(BREAK_TIME);
	}
}

/**
 * @brief Set drive's frequency to specific value
 * @param  freq Target frequency
 * @return      True on success, false on timeout
 */
bool AbbDrive::set_frequency(const uint16_t& freq)
{
	frequency = freq;
	bool done = false;
	int timeout = 0;
	int status;

	// Wait for completion or timeout
	while (!done && timeout < TIMEOUT_LIMIT) {
		delay_systick(BREAK_TIME / 2);
		status = status_word;

		if (status >= 0 && (status & SUCCESS_MASK)) done = true;
	}

	return done;
}

/**
 * @brief Get current drive's frequency
 * @return Value of current frequency register
 */
int AbbDrive::get_frequency()
{
	return output_freq;
}
