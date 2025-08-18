#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include "lwip/ip_addr.h"

/**
 * @brief Resolve um hostname para IPv4 usando lwIP DNS.
 * @param hostname (ex.: "api.thingspeak.com" ou "pool.ntp.org")
 * @param out_ip   ponteiro que receberá o IP resolvido
 * @param timeout_ms tempo máximo de espera (ex.: 5000 ms)
 * @return true se OK (out_ip válido), false caso erro/timeout
 */
bool utils_resolve_dns(const char *hostname, ip_addr_t *out_ip, uint32_t timeout_ms);

#endif /* UTILS_H */
