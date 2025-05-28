#ifndef MQTT_CONN_H
#define MQTT_CONN_H

void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len);
void mqtt_comm_subscribe(const char *topic);
void xor_encrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key);

#endif // MQTT_CONN_H