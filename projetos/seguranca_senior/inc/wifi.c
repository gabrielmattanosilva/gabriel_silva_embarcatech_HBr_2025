/**
 * @file wifi.c
 * @brief Implementação para gerenciar a conexão Wi-Fi.
 *
 * Esta implementação contém a função `wifi_init()`, que inicializa o módulo Wi-Fi, 
 * conecta-se à rede configurada e gerencia a exibição de mensagens relacionadas à 
 * conexão Wi-Fi. Utiliza o módulo CYW43 para a comunicação Wi-Fi.
 * 
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include "wifi.h"
#include "credentials.h"

/**
 * @brief Função para inicializar e conectar o módulo Wi-Fi com parâmetros personalizados
 * 
 * Essa função inicializa o módulo Wi-Fi, configura o modo cliente (sta),
 * e tenta estabelecer uma conexão com a rede Wi-Fi fornecida através dos 
 * parâmetros SSID e senha. Durante o processo, são exibidas mensagens no 
 * display e, em caso de falha, um sinal sonoro e luminoso de erro é acionado. 
 * Se a conexão for bem-sucedida, o sistema aguarda um curto período de tempo 
 * antes de prosseguir.
 * 
 * @return int Retorna 0 se a conexão for bem-sucedida ou 1 em caso de falha.
 */
int wifi_init()
{
    if (cyw43_arch_init())
    {
        printf("Falha ao inicializar Wi-Fi!\n");
        display_text(wifi_init_fail, 3);
        return 1;
    }

    display_text(wifi_init_success, 3);

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");
    display_text(wifi_connecting, 3);

    if (cyw43_arch_wifi_connect_timeout_ms(SSID, PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {   
        printf("Wi-Fi não conectado!\n");
        display_text(wifi_not_conected, 3);
        buzzer_led_fail();
        return 1;
    }
    else
    {
        printf("Wi-Fi conectado!\n");
        display_text(wifi_conected, 3);
        sleep_ms(2000);
    }
    
    return 0;
}