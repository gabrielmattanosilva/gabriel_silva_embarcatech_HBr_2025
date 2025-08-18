#ifndef THINGSPEAK_H
#define THINGSPEAK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== Configurações da janela/período ===== */
#ifndef THINGSPEAK_SEND_PERIOD_S
#define THINGSPEAK_SEND_PERIOD_S   60u   /* 10 min */
#endif

#ifndef THINGSPEAK_TICK_S
#define THINGSPEAK_TICK_S          1u     /* integração a cada 1s (via dt real) */
#endif

#define THINGSPEAK_HOST "api.thingspeak.com"

/* Sua função já existente (NÃO altere a implementação) */
void send_to_thingspeak(const char *api_key, int num_fields, ...);

/* Nova Task (implementar em thingspeak.c) */
void thingspeak_task(void *params);

#ifdef __cplusplus
}
#endif

#endif /* THINGSPEAK_H */
