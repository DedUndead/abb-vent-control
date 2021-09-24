/*
===============================================================================
 Name        : main.c
 Author      : Arefyev Pavel, Daniel Liberman, Alex Franko
 Version     : X
 License     : GPL-3.0 License
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

/* MACROS, CONSTANTS */
#define TICKRATE_HZ 1000

/* MAIN FUNCTIONS */

int main(void) {

#if defined (__USE_LPCOPEN)
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    Board_Init();
#endif
#endif

    // Configure systick clock

    /* Main polling loop */
    while(1) {
        __asm volatile ("nop");
    }


    return 0;
}
