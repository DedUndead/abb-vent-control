/*
===============================================================================
 Name        : main.c
 Author      : Arefyev Pavel, Daniel Liberman, Alex Franko
 Version     : X
 License     : GPL-2.0 License
 Description : https://github.com/DedUndead/ABB-Vent-Control
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

/* INCLUDES */
#include <cr_section_macros.h>
#include <atomic>
#include <cstring>
#include <string>
#include "modbus/ABBDrive.h"
#include "uart/LpcUart.h" // Remove after debugging finished
#include "I2C/I2C.h"
#include "I2C/SDPSensor.h"
#include "mqtt/MQTT.h"
#include "delay.h"

/* MACROS, CONSTANTS */
#define SYSTICKRATE_HZ 1000
#define I2C_CLOCKDIV   72000000 / 1000000
#define I2C_BITRATE    1000000
#define SDP_ERR        0x1F5
#define MQTT_IP        (char *)"18.198.188.151"
#define MQTT_PORT      21883
#define NETWORK_SSID   (char *)"V46D-1"
#define NETWORK_PASS   (char *)"2483124831"
#define MQTT_BFR_LEN   240
#define JSON_LEN	   3

/* FUNCTION DEFINITIONS */
void set_systick(const int& freq);
std::string construct_json(const int& len, std::string* keys, std::string* values);

/* INTERRUPT HANDLERS */
static volatile std::atomic_int delay(0);
static volatile uint32_t systicks(0);
#ifdef __cplusplus
extern "C" {
#endif
void SysTick_Handler(void)
{
	systicks++;
	if (delay > 0)  delay--;
}
#ifdef __cplusplus
}
#endif


/* MAIN PROGRAM BODY */
int main(void) {

#if defined (__USE_LPCOPEN)
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    Board_Init();
#endif
#endif

    I2C i2c(I2C_CLOCKDIV, I2C_BITRATE, 0);
    SDPSensor pressure_sensor(&i2c);

    /* Configure systick and RIT timers */
    set_systick(SYSTICKRATE_HZ);
    Chip_RIT_Init(LPC_RITIMER);

    /* Remove after debugging finished */
	LpcPinMap none = {-1, -1}; // unused pin has negative values in it
	LpcPinMap txpin = { 0, 18 }; // transmit pin that goes to debugger's UART->USB converter
	LpcPinMap rxpin = { 0, 13 }; // receive pin that goes to debugger's UART->USB converter
	LpcUartConfig cfg = { LPC_USART0, 115200, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false, txpin, rxpin, none, none };
	LpcUart uart(cfg);

    /* Configure frequency controller */
    ABBDrive abb_drive;

    /* Configure MQTT */
    MQTT mqtt;
    mqtt.connect(NETWORK_SSID, NETWORK_PASS, MQTT_IP, MQTT_PORT);
    uart.write(std::to_string(mqtt.get_status()) + "\r\n");

	/* Test values remove after debugging */
	if (abb_drive.get_frequency() < 1000) abb_drive.set_frequency(15000);

	/* Main scope variables block */
	std::string json_keys[JSON_LEN] = { "ts", "freq", "pressure" };
	std::string json_values[JSON_LEN];

	delay_systick(100);

    /* Main polling loop */
    while(1) {
    	int16_t pressure = pressure_sensor.read();
    	uart.write(std::to_string(pressure) + "\r\n");

    	mqtt.publish("/iot/grp1", construct_json(JSON_LEN, json_keys, json_values), strlen(mes));
    	uart.write(std::to_string(mqtt.get_status()) + "\r\n");

    	if (abb_drive.get_frequency() < 1000) abb_drive.set_frequency(15000);

    	delay_systick(5000);
    }


    return 0;
}

/**
 * @brief Configure systick to a specific frequency
 * @param freq Target frequency
 */
void set_systick(const int& freq)
{
    uint32_t sys_tick_rate;
    Chip_Clock_SetSysTickClockDiv(1);
    sys_tick_rate = Chip_Clock_GetSysTickClockRate();
    SysTick_Config(sys_tick_rate / freq);
}

/**
 * @brief Delay execution by amount of systicks
 * @param ticks Number of ticks for delay
 */
void delay_systick(const int ticks)
{
	delay = ticks;
	while (delay > 0) {
		__WFI();
	}
}

// Should be moved to a separate JSON class
/**
 * @brief Construct json-like string
 * @param len    Number pf lines
 * @param keys   Keys for key-value pairs
 * @param values Values for key-value pairs
 * @return Pointer to newly constructed string
 */
char* construct_json(const int& len, std::string* keys, std::string* values)
{
	char json[MQTT_BUF_LEN];
	std::string lines("");

	for (int i = 0; i < len; i++) {
		lines += key[i] + ':' + value[i] + '\n';
	}

	sprintf(json, "{%s}", lines.c_str());

	return json;
}

void update_values(json) {

}

/**
 * @brief Arduino's millis function implementation
 * Used in modbus
 * @return Number of milliseconds from the beginning of the program
 */
uint32_t millis()
{
	return systicks;
}
