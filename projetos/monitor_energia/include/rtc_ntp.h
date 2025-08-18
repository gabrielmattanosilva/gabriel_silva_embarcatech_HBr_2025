#ifndef RTC_NTP_H
#define RTC_NTP_H

#include <stdbool.h>
#include <stdint.h>

/* Offset de fuso horário (segundos). Ex.: GMT-3 = -3*3600. */
#define RTC_NTP_TZ_OFFSET_SECONDS  (-3 * 3600)   // America/Sao_Paulo sem DST

/* Inicializa o RTC do RP2040 em 1900-01-01 00:00:00 (base NTP). */
void rtc_ntp_init(void);

/**
 * @brief Sincroniza o RTC usando um servidor NTP (UDP/123).
 *        Converte diretamente NTP(1900) -> datetime_t aplicando fuso (TZ).
 * @param server_host ex.: "pool.ntp.org"
 * @param timeout_ms  ex.: 5000 ms
 * @return true se sucesso (RTC ajustado), false se falha/timeout/DNS
 */
bool rtc_ntp_sync(const char *server_host, uint32_t timeout_ms);

#endif /* RTC_NTP_H */
