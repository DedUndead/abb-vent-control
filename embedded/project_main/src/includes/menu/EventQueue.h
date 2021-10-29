/*
 * EventQueue.h
 *
 *  Created on: 29 окт. 2021 г.
 *      Author: botmo
 */

#ifndef INCLUDES_MENU_EVENTQUEUE_H_
#define INCLUDES_MENU_EVENTQUEUE_H_

#include <queue>
#include "MenuItem.h"

/**
 * @brief Short class implementing EventQueue for handling menu items
 */
class EventQueue {
public:
	EventQueue() {};
	~EventQueue() {};
	void publish(MenuItem::menuEvent event) {
		queue.push(event);
	}
	MenuItem::menuEvent consume() {
		MenuItem::menuEvent event(queue.front());
		queue.pop();
		return event;
	}
	bool pending() {
		return !queue.empty();
	}
private:
	std::queue<MenuItem::menuEvent> queue;
};


#endif /* INCLUDES_MENU_EVENTQUEUE_H_ */
