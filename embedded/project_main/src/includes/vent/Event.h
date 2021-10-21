#ifndef SRC_HEADERS_EVENT_H_
#define SRC_HEADERS_EVENT_H_


#ifndef HEADERS_EVENT_H_
#define HEADERS_EVENT_H_


class Event {
public:
	enum event_type { eEnter, eExit, eTick, eAuto, eManual, eMqtt, ePres, eFreq };
	Event(event_type e = eTick, int64_t v = 0) : type(e), value(v) {};
	event_type type;
	int64_t value;
};


#endif


#endif /* SRC_HEADERS_EVENT_H_ */
