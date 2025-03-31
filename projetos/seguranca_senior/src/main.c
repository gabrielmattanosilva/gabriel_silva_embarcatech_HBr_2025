/**
 * @file main.c
 * @brief Sistema de Envio de Mensagens de Alerta Via Whatsapp Para Segurança Sênior
 * 
 * Este código foi desenvolvido para monitorar a interação com um receptor de controle remoto RF 433Mhz 
 * conectado aos pinos GPIO 4, 8, 9 e 20 do Raspberry Pi Pico W. Quando os botões são pressionados no 
 * controle, o receptor coloca em nível lógico alto o pino correspondente ao boão, fazendo com que o
 * sistema envie uma mensagem via WhatsApp utilizando a API do CallMeBot. O dispositivo também exibe 
 * status de operação em um display OLED alem de tocar os buzzes e piscar um led.
 * 
 * Dependências:
 * - Pico SDK
 * - Bibliotecas do diretório "inc"
 * - Configuração inicial da API CallMeBot:
 *   https://www.callmebot.com/blog/free-api-whatsapp-messages/
 *  
 * Funcionalidades:
 * - Monitoramento de 4 pinos com debounce
 * - Envio de mensagens via WhatsApp
 * - Exibição de status no display OLED
 * - Tocar buzzers e piscar led
 * 
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include <stdio.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "button_handler.h"
#include "buzzer_led.h"
#include "callmebot_whatsapp.h"
#include "credentials.h"
#include "display_oled.h"
#include "display_text.h"
#include "wifi.h"

/**
 * @brief Função principal
 * 
 * Inicializa monitor serial, o display OLED, o Wi-Fi, os buzzers, o led eos botões, 
 * e entra em um loop infinito para monitoramento do sistema.
 * 
 * @return int Retorna 0 se executado com sucesso, 1 caso haja erro
 */
int main()
{  
    // Inicialização dos periféricos
    stdio_init_all();
    display_init();
    buzzer_led_init();
    button_handler_init();
    sleep_ms(1000);

    // Inicialização do Wi-Fi
    wifi_init();

    // Inicia um temporizador para verificar os botões a cada 1s
    struct repeating_timer button_timer;
    add_repeating_timer_ms(1000, button_check_callback, NULL, &button_timer);

    // Envia uma mensagem inicial indicando que o dispositivo está pronto
    if (send_whatsapp_message("Dispositivo pronto para uso!", PHONE_NUMBER, API_KEY))
    {
        printf("Mensagem inicial enviada com sucesso!\n");
        display_text(ready_to_use, 3);
        sleep_ms(2000);
        display_text(msg_init_success, 3);
        buzzer_led_init_success();
    }
    else
    {   printf("Falha ao enviar mensagem inicial!\n");
        display_text(msg_init_fail, 3);
        buzzer_led_fail();
    }

    while (true)
    {
        cyw43_arch_poll(); // Mantém o Wi-Fi ativo
        sleep_ms(100);
    }

    cyw43_arch_deinit();
    return 0;
}
