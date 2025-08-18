/**
 * @file wifi.c
 * @brief Implementação para gerenciar a conexão Wi-Fi.
 *
 * Esta implementação contém a função `wifi_init()`, que inicializa o módulo Wi-Fi,
 * conecta-se à rede configurada e gerencia a exibição de mensagens relacionadas à
 * conexão Wi-Fi.
 *
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include "wifi.h"

/**
 * @brief Função para inicializar e conectar o módulo Wi-Fi
 *
 * Essa função inicializa o módulo Wi-Fi, configura o modo cliente (sta),
 * e tenta estabelecer uma conexão com a rede Wi-Fi.
 *
 * @return bool Retorna true se a conexão for bem-sucedida ou false em caso de falha.
 */
bool wifi_init()
{
    if (cyw43_arch_init())
    {
        printf("Falha ao inicializar Wi-Fi!\n");
        return false;
    }

    cyw43_arch_enable_sta_mode();
    printf("Wi-Fi iniciado com sucesso! Conectando...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(SSID, PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("Falha na conexão!\n");
        return false;
    }
    else
    {
        printf("Wi-Fi conectado com sucesso!\n");
        uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
        printf("IP: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    return true;
}