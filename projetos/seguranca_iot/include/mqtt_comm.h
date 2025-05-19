#ifndef CREDENTIALS_H
#define CREDENTIALS_H

static mqtt_client_t *client;
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);
static void mqtt_pub_request_cb(void *arg, err_t result);
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len);

#endif // CREDENTIALS_H