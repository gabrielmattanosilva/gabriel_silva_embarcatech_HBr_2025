#include <stdio.h>
#include <string.h>
#include <time.h>

#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "include/credentials.h"
#include "include/internal_temperature.h"
#include "include/mqtt_conn.h"
#include "include/wifi_conn.h"

#define CLIENT_ID   "bitdog1"
#define BROKER_IP   "192.168.3.196"
#define TOPIC       "escola/sala1/temperatura"

int main() {
    stdio_init_all();  // Inicializa a saída padrão
    setup_temperature_sensor(); // Inicaliza o sensor de temperatura interna do RP2040
    sleep_ms(1000);
    connect_to_wifi(WIFI_SSID, WIFI_PASS);
    mqtt_setup(CLIENT_ID, BROKER_IP, BROKER_USER, BROKER_PASS);
    sleep_ms(1000); // Delay para estabilizar conexão

    // Loop principal
    while (true) {
        // Leitura da temperatura interna do RP2040 
        uint16_t adc_value = adc_read();
        float temperature = adc_to_celsius(adc_value);

        // Mensagem original a ser enviada
        char mensagem[64];
        sprintf(mensagem, "{\"valor\":%.2f,\"ts\":%lu}", temperature, time(NULL));
        size_t msg_len = strlen(mensagem);

        // Buffer para mensagem criptografada (16 bytes)
        uint8_t criptografada[64];
        
        // Criptografa a mensagem usando XOR com chave 42
        xor_encrypt((uint8_t *)mensagem, criptografada, msg_len, 42);

        // Publica a mensagem criptografada
        mqtt_comm_publish(TOPIC, criptografada, msg_len);

        cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo
        sleep_ms(5000); // Garante o envio da temperatura a cada 5 segundos
    }

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}