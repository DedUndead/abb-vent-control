#ifndef INCLUDES_PROJECT_MAIN_H_
#define INCLUDES_PROJECT_MAIN_H_


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
#include <stdio.h>
#include "modbus/AbbDrive.h"
#include "uart/LpcUart.h"
#include "I2C/I2C.h"
#include "I2C/SDPSensor.h"
#include "MQTTClient.h"
#include "mqtt/MQTT.h"
#include "menu/SimpleMenu.h"
#include "menu/IntegerEdit.h"
#include "menu/StringEdit.h"
#include "menu/EventQueue.h"
#include "vent/SmartVent.h"
#include "vent/Event.h"
#include "json/json.hpp"
#include "delay.h"
#include "project_main.h"

/* MACROS */
#define SYSTICKRATE_HZ       1000
#define I2C_CLOCKDIV         72000000 / 1000000
#define I2C_BITRATE          1000000
#define I2C_MODE             0
#define DEBOUNCE             20
#define VENT_TICK_T          1000
#define INITIAL_DELAY        100
#define MQTT_IP              (char *)"18.198.188.151"
#define MQTT_PORT            21883
#define NETWORK_SSID         (char *)"V46D-1"
#define NETWORK_PASS         (char *)"2483124831"
#define MQTT_TOPIC_RECEIVE   (const char *)"/iot/grp1/web"
#define MQTT_TOPIC_SEND      (const char *)"/iot/grp1/mcu"
#define MQTT_BFR_LEN         240
#define MQTT_OK              0
#define MQTT_YIELD_TIME      100
#define MENU_LABELS_N        2
#define FREQ_MAX             100
#define FREQ_MIN             0
#define FREQ_LCD_STEP        1
#define PRES_MAX             120
#define PRES_MIN             0
#define PRES_STEP            1
#define MENU_ITEMS_NUM       4
#define MQTT_UPDATE_TIMEOUT  5000
#define ERROR_TIMEOUT        5000
#define VENT_HARDWARE_ERROR  -1
#define VENT_TIMEOUT_ERROR   -2
#define LCD_NEXT_LINE        16

/* SUPPORT FUNCTIONS AND TYPES DECLARATIONS */
std::string get_sample_json(SmartVent* ventilation, int sample_number);
void handle_mqtt_input(SmartVent* ventilation, PropertyEdit* items[MENU_ITEMS_NUM]);
void handle_lcd_input(SmartVent* ventilation, PropertyEdit* items[MENU_ITEMS_NUM]);
void update_lcd(SmartVent* ventilation, PropertyEdit* items[MENU_ITEMS_NUM]);
void handle_mode(SmartVent* ventilation, PropertyEdit* menu_items[MENU_ITEMS_NUM], int mode);
void handle_freq(SmartVent* ventilation, PropertyEdit* menu_items[MENU_ITEMS_NUM],int freq);
void handle_target_pressure(SmartVent* ventilation, PropertyEdit* menu_items[MENU_ITEMS_NUM], int pressure);
void handle_pressure(SmartVent* ventilation, PropertyEdit* menu_items[MENU_ITEMS_NUM], int pressure);
void set_systick(const int& freq);
void mqtt_message_handler(MessageData* data);
void ERROR_CONDITION();

void (*item_handlers[MENU_ITEMS_NUM])(SmartVent*, PropertyEdit*[MENU_ITEMS_NUM], int) = {
		handle_mode,
		handle_freq,
		handle_target_pressure,
		handle_pressure
};
enum item_indexes { iMode, iFrequency, iTarget, iPressure };

/* INTERRUPT HANDLERS AND SHARED VARIABLES */
static volatile std::atomic_int delay(0);
static volatile uint32_t systicks(0);
static volatile uint32_t last_pressed(0);
static SimpleMenu* menu_ptr(nullptr);
static EventQueue* menu_events_ptr(nullptr);
static volatile bool sample_ready(false);
static volatile bool tick_ready(false);
static std::string mqtt_message("");
static bool mqtt_message_arrived(false);


#endif /* INCLUDES_PROJECT_MAIN_H_ */
