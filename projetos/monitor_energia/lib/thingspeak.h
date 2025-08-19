/**
 * @file thingspeak.h
 * @brief API para envio de dados ao ThingSpeak e task de telemetria.
 */

#ifndef THINGSPEAK_H
#define THINGSPEAK_H

#include <stdint.h>

#define THINGSPEAK_SEND_PERIOD_S    60U                     /**< Período de envio recorrente (s). */
#define THINGSPEAK_TICK_S           1U                      /**< Tick da task para acumular energia (s). */
#define THINGSPEAK_HOST             "api.thingspeak.com"    /**< Host do serviço. */

void thingspeak_send(const char *api_key, uint8_t num_fields, ...);
void thingspeak_task(void *params);

#endif /* THINGSPEAK_H */
