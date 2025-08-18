#ifndef THINGSPEAK_H
#define THINGSPEAK_H

#include <stdint.h>

#define THINGSPEAK_SEND_PERIOD_S    60u
#define THINGSPEAK_TICK_S           1u
#define THINGSPEAK_HOST             "api.thingspeak.com"

void thingspeak_send(const char *api_key, uint8_t num_fields, ...);
void thingspeak_task(void *params);

#endif /* THINGSPEAK_H */
