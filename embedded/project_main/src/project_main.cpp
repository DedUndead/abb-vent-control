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
#include "vent/SmartVent.h"
#include "delay.h"

/* MACROS, CONSTANTS */
#define SYSTICKRATE_HZ 1000
#define I2C_CLOCKDIV   72000000 / 1000000
#define I2C_BITRATE    1000000
#define MQTT_IP        (char *)"18.198.188.151"
#define MQTT_PORT      21883
#define NETWORK_SSID   (char *)"V46D-1"
#define NETWORK_PASS   (char *)"2483124831"
#define MQTT_BFR_LEN   240
#define JSON_LEN	   3

/* FUNCTION DEFINITIONS */
void set_systick(const int& freq);

/* INTERRUPT HANDLERS AND SHARED VARIABLES */
static volatile std::atomic_int delay(0);
static volatile uint32_t systicks(0);
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

		menu_ptr->event(MenuItem::up);
	}

	void PIN_INT1_IRQHandler(void)
	{
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));

		menu_ptr->event(MenuItem::ok);
	}

	void PIN_INT2_IRQHandler(void)
	{
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));

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
    I2C i2c(I2C_CLOCKDIV, I2C_BITRATE, 0);
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
    IntegerEdit mode(&lcd, std::string("Mode"), 1, 0, 1);
    IntegerEdit freq(&lcd, std::string("Frequency"), 22000, 0, 1000);
    IntegerEdit pressure(&lcd, std::string("Pressure"), 120, 0, 1);
    menu.addItem(new MenuItem(&mode));
    menu.addItem(new MenuItem(&freq));
    menu.addItem(new MenuItem(&pressure));
    menu_ptr = &menu;

    /* Initial menu value */
    mode.setValue(0);
    freq.setValue(0);
    pressure.setValue(0);
    menu.event(MenuItem::show);

    /* Menu buttons */
    DigitalIoPin up(0, 24, true, true, true);
    DigitalIoPin ok(0, 0, true, true, true);
    DigitalIoPin down(1, 3, true, true, true);
    delay_systick(100);

    /* I/O interrupts initialization */
    DigitalIoPin::init_gpio_interrupts();
    up.enable_interrupt(0);
    ok.enable_interrupt(1);
    down.enable_interrupt(2);

    /* Configure MQTT */
    MQTT mqtt;
    mqtt.connect(NETWORK_SSID, NETWORK_PASS, MQTT_IP, MQTT_PORT);
    uart.write(std::to_string(mqtt.get_status()) + "\r\n");

    /* Main polling loop */
    while (true) {
    	delay_systick(5);
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

/**
 * @brief Return time elapsed from program start
 * @return Time in ms
 */
uint32_t millis()
{
	return systicks;
}
