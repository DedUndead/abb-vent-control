#ifndef INCLUDES_DELAY_H_
#define INCLUDES_DELAY_H_


#include <stdint.h>


void delay_systick(const int ticks);

#ifdef __cplusplus
extern "C" {
#endif
uint32_t millis();
#ifdef __cplusplus
}
#endif


#endif /* INCLUDES_DELAY_H_ */
