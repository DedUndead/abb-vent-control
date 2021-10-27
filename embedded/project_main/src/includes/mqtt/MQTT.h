#ifndef INCLUDES_MQTT_MQTT_H_
#define INCLUDES_MQTT_MQTT_H_


#include "MQTTClient.h"
#include <string>

#define SEND_BUF_LENGTH 256
#define READ_BUF_LENGTH 2556
#define MQTT_TIMEOUT    30000
#define MQTT_VERSION    3
#define CLIENT_ID       "lpcgrp1"


class MQTT {
public:
	MQTT(void (*message_handler_)(MessageData* data));
	int connect(const char* ssid, const char* pass, char* ip, int port);
	void disconnect();
	int subscribe(const char* topic);
	int publish(const char* topic, char* data, size_t data_size);
	int publish(const char* topic, std::string data, size_t data_size);
	int yield(int duration);
	int get_status() const;
private:
	MQTTClient client;
	Network network;
	MQTTPacket_connectData connect_data;
	unsigned char sendbuf[SEND_BUF_LENGTH];
	unsigned char readbuf[READ_BUF_LENGTH];
	int status;
	int count;

	void (*message_handler)(MessageData* data);
};


#endif /* INCLUDES_MQTT_MQTT_H_ */
