#ifndef INCLUDES_MODBUS_AbbDrive_H_
#define INCLUDES_MODBUS_AbbDrive_H_


#include "ModbusMaster.h"
#include "ModbusRegister.h"

#define SLAVE_ID             2
#define CONTROL_WORD_ADDRESS 0
#define FREQ_CONTROL_ADDRESS 1
#define STATUS_WORD_ADDRESS  3
#define OUTPUT_FREQ_ADDRESS	 102
#define CURRENT_ADDRESS		 103
#define TRANSMISSION_RATE    9600
#define CMD_PREP             0x0406
#define CMD_START            0x047F
#define TIMEOUT_LIMIT        20
#define SUCCESS_MASK         0x0100
#define BREAK_TIME           1000

class AbbDrive {
public:
	AbbDrive();
	bool set_frequency(const uint16_t& freq);
	int get_frequency();
private:
	ModbusMaster node;
	ModbusRegister control_word;
	ModbusRegister frequency;
	ModbusRegister status_word;
	ModbusRegister output_freq;
	ModbusRegister current;
};


#endif /* INCLUDES_MODBUS_AbbDrive_H_ */
