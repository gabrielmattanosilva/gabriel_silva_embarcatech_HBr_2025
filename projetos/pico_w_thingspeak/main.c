/**
 * @file main.c
 * @brief Exemplo de implementação para enviar dados ao ThingSpeak no Raspberry Pi Pico W.
 * 
* Dependências:
 * - Pico SDK
 * - Bibliotecas do diretório "inc"
 * - Criar conta no ThingSpeak e configurar canais, campos e API de escrita:
 *   https://thingspeak.mathworks.com/
 * 
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "credentials.h"
#include "thingspeak.h"
#include "wifi.h"

int main()
{
    stdio_init_all();

    // Inicia o módulo Wi-Fi e conecta-se a rede
    if (!wifi_init())
    {
        return -1;
    }

    // Configura o servidor DNS e resolve o hostname do ThingSpeak
    if (!resolve_dns())
    {
        return -1;
    }

    sleep_ms(1000);

    // Inicializa a semente do gerador de números aleatórios
    srand(time(NULL));

    while (true)
    {
        // Gera um número float aleatório entre 0.0 e 100.0
        float random_value = (float)rand() / (float)(RAND_MAX / 100.0);

        // Envia o número aleatório para o field1 do ThingSpeak
        send_to_thingspeak(API_KEY, 1, random_value);

        // Espera 30 segundos antes de enviar o próximo valor
        sleep_ms(30000);
    }

    return 0;
}