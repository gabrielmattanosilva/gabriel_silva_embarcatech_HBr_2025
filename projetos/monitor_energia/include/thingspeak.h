#ifndef THINGSPEAK_H
#define THINGSPEAK_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "pico/stdlib.h"

/* Envia fields ao ThingSpeak (usa utils_resolve_dns internamente). */
void send_to_thingspeak(const char *api_key, int num_fields, ...);

#endif // THINGSPEAK_H
