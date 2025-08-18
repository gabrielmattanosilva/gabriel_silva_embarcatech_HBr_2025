#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

/* Inicializa parâmetros do Wi-Fi Manager (não conecta ainda). */
void wifi_manager_begin(const char *ssid, const char *pass);

/* Informa se está conectado ao AP (link UP). */
bool wifi_manager_is_connected(void);

/* Força nova janela de conexão (zera backoff). */
void wifi_manager_force_reconnect(void);

/* Aguarda conexão até timeout (ms). */
bool wifi_manager_wait_connected(uint32_t timeout_ms);

/* Tarefa FreeRTOS: roda a FSM (sem cyw43_arch_poll). */
void wifi_manager_task(void *params);

/* IP em string "a.b.c.d" (válido quando conectado). */
const char *wifi_manager_ip_str(void);

#endif /* WIFI_MANAGER_H */
