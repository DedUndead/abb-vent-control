#include "I2C/SdpSensor.h"
#include "modbus/AbbDrive.h"
#include "Event.h"
#include "SmartVent.h"

SmartVent::SmartVent(SdpSensor* sdp_, AbbDrive* drive_) :
	sdp(sdp_),
	drive(drive_)
{
	set_state(&SmartVent::startup);

	current_status.mode = mAuto;
	current_status.frequency = drive->get_frequency();
	current_status.pressure = read_pressure();
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

void SmartVent::set_state(state_ptr new_state)
{
	// No exit events
	current_state = new_state;
	(this->*current_state)(Event(Event::eEnter));
}

void SmartVent::mode_auto(const Event& e)
{
	switch (e.type) {
	case Event::eEnter:
		break;
	case Event::eManual:
		set_state(&SmartVent::mode_manual);

		break;
	case Event::ePres:
		set_state(&SmartVent::set_pressure);

		break;
	default:
		break;
	}
}

void SmartVent::mode_manual(const Event& e)
{
	switch (e.type) {
	case Event::eEnter:
		break;
	case Event::eAuto:
		set_state(&SmartVent::mode_auto);

		break;
	case Event::eFreq:
		//set_frequency(e.value);

		break;
	default:
		break;
	}
}

void SmartVent::set_pressure(const Event& e)
{
	switch (e.type) {
	case Event::eEnter:
		timer = 0;
		break;
	case Event::eTick:
		//if (read_pressure()) {}
		if (timer > 10) {}

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

	if (result == SDP_ERR) return 0;
	else				   return result;
}
