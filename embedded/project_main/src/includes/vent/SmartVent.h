#ifndef INCLUDES_STATE_MACHINES_SMARTVENT_H_
#define INCLUDES_STATE_MACHINES_SMARTVENT_H_


#include "I2C/SdpSensor.h"
#include "modbus/AbbDrive.h"
#include "Event.h"

class SmartVent;
typedef void (SmartVent::*state_ptr)(const Event &);

class SmartVent {
public:
	SmartVent(SdpSensor* sdp_, AbbDrive* drive_);
	void handle_state(const Event& e);
	enum mode { mAuto, mManual };
private:
	state_ptr current_state;
	void set_state(state_ptr new_state);
	void startup(const Event& e);
	void mode_auto(const Event& e);
	void mode_manual(const Event& e);
	void mqtt_parse(const Event& e);
	void set_pressure(const Event& e);
	//void set_freq(state_ptr new_state);

	int timer;
	mode mode;
	SdpSensor* sdp;
	AbbDrive* drive;
};


#endif /* INCLUDES_STATE_MACHINES_SMARTVENT_H_ */
