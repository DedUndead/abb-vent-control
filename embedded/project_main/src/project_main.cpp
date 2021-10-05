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
#include <string>
#include "headers/LpcUart.h" // Remove after debugging finished
#include "headers/I2C.h"
#include "headers/SDPSensor.h"


/* MACROS, CONSTANTS */
#define SYSTICKRATE_HZ 1000
#define I2C_CLOCKDIV   72000000 / 1000000
#define I2C_BITRATE    1000000
#define SDP_ERR        0x1F5
static volatile std::atomic_int counter;


/* FUNCTION DEFINITIONS */
void set_systick(const int& freq);
void delay_systick(const int& ticks);


/* INTERRUPT HANDLERS */
#ifdef __cplusplus
extern "C" {
#endif
void SysTick_Handler(void)
{
	if (counter > 0) counter--;
}
#ifdef __cplusplus
}
#endif


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

    /* Main polling loop */
    while(1) {
    	int16_t pressure = pressure_sensor.read();
    	uart.write(std::to_string(pressure) + "\r\n");

        delay_systick(1000);
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
void delay_systick(const int& ticks)
{
	counter = ticks;
	while (counter > 0) {
		__WFI();
	}
}
