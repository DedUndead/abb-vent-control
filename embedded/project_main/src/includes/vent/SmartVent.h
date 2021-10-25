#ifndef INCLUDES_STATE_MACHINES_SMARTVENT_H_
#define INCLUDES_STATE_MACHINES_SMARTVENT_H_


#include "I2C/SdpSensor.h"
#include "modbus/AbbDrive.h"
#include "Event.h"

#define SDP_ERR         0x1F5
#define STATUS_OK       0
#define STATUS_LOADING  1
#define STATUS_FAIL    -1
#define STATUS_TIMEOUT -2
#define FREQ_STEP	    500
#define PRES_ERROR      0
#define MAX_FREQ		30000
#define MIN_FREQ		0

class SmartVent;

typedef void (SmartVent::*state_ptr)(const Event &);
typedef struct status {
	int mode;
	int frequency;
	int pressure;
	int operation_status;
	int target_pressure;
} status;

class SmartVent {
public:
	SmartVent(SdpSensor* sdp_, AbbDrive* drive_);
	void handle_state(const Event& e);
	status get_status();

	enum mode { mManual, mAuto };
private:
	state_ptr current_state;
	void set_state(state_ptr new_state);
	void startup(const Event& e);
	void mode_auto(const Event& e);
	void mode_manual(const Event& e);
	void mqtt_parse(const Event& e);
	void set_pressure(const Event& e);
	int read_pressure();
	bool target_reached();
	void autoadjust_frequency();
	void set_frequency(int value);

	int timer;
	SdpSensor* sdp;
	AbbDrive* drive;
	status current_status;
};


#endif /* INCLUDES_STATE_MACHINES_SMARTVENT_H_ */
