/**
 * @file thingspeak.h
 * @brief Biblioteca para enviar dados ao ThingSpeak.
 *
 * Esta biblioteca contém as definições constantes e as declarações de funções
 * para resolver o DNS do ThingSpeak e enviar de dados para a plataforma.
 *
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

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

bool resolve_dns();
static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void send_to_thingspeak(const char *api_key, int num_fields, ...);

#endif // THINGSPEAK_H