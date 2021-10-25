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
#include "modbus/AbbDrive.h"
#include "uart/LpcUart.h" // Remove after debugging finished
#include "I2C/I2C.h"
#include "I2C/SDPSensor.h"
#include "mqtt/MQTT.h"
#include "menu/SimpleMenu.h"
#include "menu/IntegerEdit.h"
#include "menu/StringEdit.h"
#include "vent/SmartVent.h"
#include "vent/Event.h"
#include "delay.h"

/* MACROS, TYPES */
#define SYSTICKRATE_HZ 1000
#define I2C_CLOCKDIV   72000000 / 1000000
#define I2C_BITRATE    1000000
#define I2C_MODE       0
#define DEBOUNCE       20
#define VENT_TICK_LEN  1000
#define INITIAL_DELAY  100
#define MQTT_IP        (char *)"18.198.188.151"
#define MQTT_PORT      21883
#define NETWORK_SSID   (char *)"V46D-1"
#define NETWORK_PASS   (char *)"2483124831"
#define MQTT_BFR_LEN   240
#define MENU_LABELS_N  3
#define FREQ_MAX       22000
#define FREQ_MIN       0
#define FREQ_STEP      1500
#define PRES_MAX       120
#define PRES_MIN       0
#define PRES_STEP      1
#define MENU_ITEMS_NUM 3

/* SUPPORT FUNCTIONS DECLARATIONS */
void set_systick(const int& freq);
void observe_lcd_changes(SmartVent* ventilation, PropertyEdit* items[MENU_ITEMS_NUM], int* last_values);
void handle_mode(SmartVent* ventilation, int mode);
void handle_freq(SmartVent* ventilation, int freq);
void handle_pressure(SmartVent* ventilation, int pressure);
void (*lcd_handlers[MENU_ITEMS_NUM])(SmartVent*, int) = { handle_mode, handle_freq, handle_pressure };

/* INTERRUPT HANDLERS AND SHARED VARIABLES */
static volatile std::atomic_int delay(0);
static volatile uint32_t systicks(0);
static volatile uint32_t last_pressed(0);
static SimpleMenu* menu_ptr(nullptr);

extern "C" {
	void SysTick_Handler(void)
	{
		systicks++;
		if (delay > 0)  delay--;
	}

	void PIN_INT0_IRQHandler(void)
	{
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));

		// Button debounce
		if (millis() - last_pressed < DEBOUNCE) return;
		last_pressed = millis();

		menu_ptr->event(MenuItem::up);
	}

	void PIN_INT1_IRQHandler(void)
	{
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));

		// Button debounce
		if (millis() - last_pressed < DEBOUNCE) return;
		last_pressed = millis();

		menu_ptr->event(MenuItem::ok);
	}

	void PIN_INT2_IRQHandler(void)
	{
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));

		// Button debounce
		if (millis() - last_pressed < DEBOUNCE) return;
		last_pressed = millis();

		menu_ptr->event(MenuItem::down);
	}
}

/* MAIN PROGRAM BODY */
int main(void) {

#if defined (__USE_LPCOPEN)
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    Board_Init();
#endif
#endif

    /* Pressure sensor initialization */
    I2C i2c(I2C_CLOCKDIV, I2C_BITRATE, I2C_MODE);
    SdpSensor pressure_sensor(&i2c);

    /* Configure systick and RIT timers */
    set_systick(SYSTICKRATE_HZ);
    Chip_RIT_Init(LPC_RITIMER);

    /* Drive peripheral */
    AbbDrive abb_drive;

    /* Remove after debugging finished */
	LpcPinMap none = {-1, -1}; // unused pin has negative values in it
	LpcPinMap txpin = { 0, 18 }; // transmit pin that goes to debugger's UART->USB converter
	LpcPinMap rxpin = { 0, 13 }; // receive pin that goes to debugger's UART->USB converter
	LpcUartConfig cfg = { LPC_USART0, 115200, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false, txpin, rxpin, none, none };
	LpcUart uart(cfg);

    /* LCD initialization */
    DigitalIoPin rs(0, 10);
    DigitalIoPin en(0, 9);
    DigitalIoPin d4(1, 8);
    DigitalIoPin d5(0, 5);
    DigitalIoPin d6(0, 6);
    DigitalIoPin d7(0, 7);
    LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7);

    /* Menu initialization */
    SimpleMenu menu;

    std::string labels[MENU_LABELS_N] = { "Manual", "Automatic" };
    StringEdit mode(&lcd, std::string("Mode"), labels, MENU_LABELS_N);
    IntegerEdit freq(&lcd, std::string("Frequency"), FREQ_MAX, FREQ_MIN, FREQ_STEP);
    IntegerEdit pressure(&lcd, std::string("Pressure"), PRES_MAX, PRES_MIN, PRES_STEP);

    menu.addItem(new MenuItem(&mode));
    menu.addItem(new MenuItem(&freq));
    menu.addItem(new MenuItem(&pressure));

    menu_ptr = &menu;

    /* Initial menu value */
    mode.setValue(0);
    freq.setValue(0);
    pressure.setValue(0);

    PropertyEdit* menu_items[MENU_ITEMS_NUM] = { &mode, &freq, &pressure };
    int last_values[MENU_ITEMS_NUM] = { 0, 0, 0 };
    menu.event(MenuItem::show);

    /* Menu buttons */
    DigitalIoPin up(1, 3, true, true, true);
    DigitalIoPin ok(0, 0, true, true, true);
    DigitalIoPin down(0, 24, true, true, true);

    delay_systick(INITIAL_DELAY); // Wait for the current to settle

    /* I/O interrupts initialization */
    DigitalIoPin::init_gpio_interrupts();
    up.enable_interrupt(0);
    ok.enable_interrupt(1);
    down.enable_interrupt(2);

    /* Configure MQTT */
    MQTT mqtt;
    mqtt.connect(NETWORK_SSID, NETWORK_PASS, MQTT_IP, MQTT_PORT);
    uart.write(std::to_string(mqtt.get_status()) + "\r\n");

    /* Initialize main state machine */
    SmartVent ventilation(&pressure_sensor, &abb_drive);

    /* Main polling loop */
    while (true) {
    	// Observe changes in LCD UI
    	observe_lcd_changes(&ventilation, menu_items, last_values);

    	if (millis() % VENT_TICK_LEN == 0) {
    		ventilation.handle_state(Event(Event::eTick));

    		status status = ventilation.get_status();
			uart.write("Frequency: " + std::to_string(status.frequency) + "\r\n");
			uart.write("Pressure: " + std::to_string(status.pressure) + "\r\n");
			uart.write("Mode: " + std::to_string(status.mode) + "\r\n");
			uart.write("Status: " + std::to_string(status.operation_status) + "\r\n");
			uart.write("Target pressure: " + std::to_string(status.target_pressure) + "\r\n\r\n");
    	}
    }

    return 0;
}

/* SUPPORT FUNCTIONS DEFINITIONS */

/**
 * @brief Observe user inputs in LCD
 * If any present, call the respective handler
 */
void observe_lcd_changes(SmartVent* ventilation, PropertyEdit* items[MENU_ITEMS_NUM], int* last_values)
{
	for (int i = 0; i < MENU_ITEMS_NUM; i++) {
		if (items[i]->getValue() != last_values[i]) {
			last_values[i] = items[i]->getValue(); // Update last value
			(*lcd_handlers[i])(ventilation, last_values[i]); // Call handler
		}
	}
}

/**
 * @brief Mode handler
 * Send event to switch to mode specified by the user
 */
void handle_mode(SmartVent* ventilation, int value)
{
	if (value == SmartVent::mManual) {
		ventilation->handle_state(Event(Event::eManual));
	}
	else {
		ventilation->handle_state(Event(Event::eAuto));
	}
}

/**
 * @brief Frequency handler
 * Send event to set frequency specified by the user
 * Skip if mode is automatic
 */
void handle_freq(SmartVent* ventilation, int freq)
{
	if (ventilation->get_status().mode == SmartVent::mAuto) return;
	ventilation->handle_state(Event(Event::eFreq, freq));
}

/**
 * @brief Pressure handler
 * Send event to set target pressure specified by the user
 * Skip if more is manual
 */
void handle_pressure(SmartVent* ventilation, int pressure)
{
	if (ventilation->get_status().mode == SmartVent::mManual) return;
	ventilation->handle_state(Event(Event::ePres, pressure));
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

/**
 * @brief Return time elapsed from program start
 * @return Time in ms
 */
uint32_t millis()
{
	return systicks;
}
