#ifndef HEADERS_IMUTEX_H_
#define HEADERS_IMUTEX_H_

#include "chip.h"

/**
 * Implementation of Critical Section class
 */
class Imutex {
public:
	Imutex() : enable(false) {}
	~Imutex() {}
	void lock() {
		enable = (__get_PRIMASK() & 1) == 0;
		__disable_irq();
	}
	void unlock() {
		if(enable) __enable_irq();
	}
private:
	bool enable;
};

#endif /* HEADERS_IMUTEX_H_ */
