#include "MQTT.h"

/**
 * @bried Abstraction for MQTT interface
 */
MQTT::MQTT() :
	client(), network()
{
	connect_data = MQTTPacket_connectData_initializer;
}

/**
 * @brief Connect to network and broker
 * @param ssid Network SSID
 * @param pass Network password
 * @param ip   Broker ip
 * @param port Broker port
 * @return     Status of the operation
 */
int MQTT::connect(const char* ssid, const char* pass, char* ip, int port)
{
	// Connect to network and initialize MQTT
	NetworkInit(&network, ssid, pass);
	MQTTClientInit(&client, &network, MQTT_TIMEOUT, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

	// Connect to broker
	status = NetworkConnect(&network, ip, port);
	connect_data.MQTTVersion = MQTT_VERSION;
	connect_data.clientID.cstring = (char *)CLIENT_ID;

	return status;
}

/**
 * @brief Disconnect from network
 */
void MQTT::disconnect()
{
	NetworkDisconnect(&network);
}

/**
 * @brief Subscribe to topic
 * @param  topic Target topic
 * @return       Operation status
 */
int MQTT::subscribe(const char* topic)
{
	status = MQTTSubscribe(&client, topic, QOS2, arrived_message);
	return status;
}

/**
 * @brief Publish message to a topic
 * @param topic 	Target topic
 * @param data  	String of data
 * @param data_size Length of string
 * @return          Status of the operation
 */
int MQTT::publish(const char* topic, char* data, size_t data_size)
{
	MQTTMessage message;

	message.qos = QOS1;
	message.retained = 0;
	message.payload = data;
	message.payloadlen = data_size;

	status = MQTTPublish(&client, topic, &message);
	return status;
}

/**
 * @brief Listen to subcribed topic
 * @param duration Duration in milliseconds
 * @return 		   Status of operation
 */
int MQTT::listen(int duration)
{
	status = MQTTYield(&client, duration);
	return status;
}

/**
 * @brief Get current MQTT status
 * @return Current MQTT status
 */
int MQTT::get_status() const
{
	return status;
}

/**
 * @brief Subscription message handler
 */
void MQTT::arrived_message(MessageData* data)
{

}
