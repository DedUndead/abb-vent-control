#include "I2C/SdpSensor.h"
#include "modbus/AbbDrive.h"
#include "Event.h"
#include "SmartVent.h"

/**
 * Smart ventilation abstraction
 * Status codes:
 *  0 - status OK
 *  1 - status LOADING/UNCOMPLETE
 * -1 - status FAIL
 * -2 - status TIMEOUT
 * Status fail is set if request cannot be fullfilled (e.g. peripheral failure)
 */
SmartVent::SmartVent(SdpSensor* sdp_, AbbDrive* drive_) :
	timer(0),
	sdp(sdp_),
	drive(drive_)
{
	set_state(&SmartVent::mode_manual);

	current_status.mode = mManual;
	current_status.operation_status = STATUS_OK;
	current_status.frequency = drive->get_frequency();
	current_status.pressure = read_pressure();
	current_status.target_pressure = 0;
}

void SmartVent::handle_state(const Event& e)
{
	(this->*current_state)(e);
}

/**
 * @brief Obtain current values inside state machine
 * @return Current values in status structure format
 */
status SmartVent::get_status()
{
	return current_status;
}

/**
 * @brief Set desired state of state machine
 * @param new_state Function pointer to new state
 */
void SmartVent::set_state(state_ptr new_state)
{
	// No exit events
	current_state = new_state;
	(this->*current_state)(Event(Event::eEnter));
}

/**
 * @brief Automatic mode state
 */
void SmartVent::mode_auto(const Event& e)
{
	switch (e.type) {
	case Event::eEnter:
		current_status.target_pressure = current_status.pressure;
		current_status.mode = mAuto;

		break;
	case Event::eManual:
		set_state(&SmartVent::mode_manual);

		break;
	case Event::ePres:
		current_status.target_pressure = e.value;
		set_state(&SmartVent::set_pressure);

		break;
	case Event::eTick:
		current_status.pressure = read_pressure();

		if (!target_reached()) {
			set_state(&SmartVent::set_pressure);
		}
		else {
			modbus_heartbeat();
		}

		break;
	default:
		break;
	}
}

/**
 * @brief Manual mode state
 */
void SmartVent::mode_manual(const Event& e)
{
	switch (e.type) {
	case Event::eEnter:
		current_status.mode = mManual;

		break;
	case Event::eAuto:
		set_state(&SmartVent::mode_auto);

		break;
	case Event::eFreq:
		set_frequency(e.value);

		break;
	case Event::eTick:
		current_status.pressure = read_pressure();
		modbus_heartbeat();

		break;
	default:
		break;
	}
}

/**
 * @brief Setting pressure in automatic mode state
 */
void SmartVent::set_pressure(const Event& e)
{
	switch (e.type) {
	case Event::eEnter:
		timer = 0;
		current_status.operation_status = STATUS_LOADING;

		break;
	case Event::eTick:
		// Read current pressure
		current_status.pressure = read_pressure();

		// Check if requested value has been achieved
		if (!target_reached()) {
			autoadjust_frequency();
		}
		else {
			set_state(&SmartVent::mode_auto);
		}

		// Timeout
		if (timer >= 10) {
			current_status.operation_status = STATUS_TIMEOUT;
			set_state(&SmartVent::mode_auto);
		}
		timer++;

		break;
	case Event::ePres:
		current_status.target_pressure = e.value;
		timer = 0;

		break;
	case Event::eManual:
		set_state(&SmartVent::mode_manual);

		break;
	default:
		break;
	}
}

/**
 * @brief Read pressure from SDP sensor
 * @return 0 (neural value for the current class) if error, otherwise reading
 */
int SmartVent::read_pressure()
{
	int result = sdp->read();

	// Error checking
	if (result == SDP_ERR) {
		current_status.operation_status = STATUS_FAIL;
		return 0;
	}
	else {
		current_status.operation_status = STATUS_OK;
		return result;
	}
}

/**
 * @brief Calculate if target pressure was reached
 */
bool SmartVent::target_reached()
{
	return (current_status.pressure <= current_status.target_pressure + PRES_ERROR)
			&&
		   (current_status.pressure >= current_status.target_pressure - PRES_ERROR);
}

/**
 * @brief Autoadjust frequency depending on current pressure value
 */
void SmartVent::autoadjust_frequency()
{
	int target_frequency;

	// Automatic adjacement
	if (current_status.pressure > current_status.target_pressure) {
		target_frequency = current_status.frequency - FREQ_STEP;
		timer = 0;
	}
	else {
		target_frequency = current_status.frequency + FREQ_STEP;
		timer = 0;
	}

	// Sanity check
	if (target_frequency > MAX_FREQ) target_frequency = MAX_FREQ;
	if (target_frequency < MIN_FREQ) target_frequency = MIN_FREQ;

	set_frequency(target_frequency);
}

/**
 * @brief Set frequency to desired value
 * @param value Value in Hz
 */
void SmartVent::set_frequency(int value)
{
	int hz = linear_transform(value);

	// Error checking
	if (!drive->set_frequency(hz)) {
		current_status.operation_status = STATUS_FAIL;
		return;
	}

	current_status.operation_status = STATUS_OK;
	current_status.frequency = value;
}

/**
 * @brief Transform percent of speed into fan frequency
 * @param  percent Value
 * @return         Value in hz
 */
int SmartVent::linear_transform(int percent)
{
	return FREQ_LIMIT_HZ * percent / 100;
}

