#include <string.h>
#include <stdio.h>
#include <time.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "include/credentials.h"
#include "include/wifi_conn.h"
#include "include/mqtt_conn.h"

#define CLIENT_ID   "bitdog2"
#define BROKER_IP   "192.168.3.196"
#define TOPIC       "escola/sala1/temperatura"

int main() {
    stdio_init_all();  // Inicializa a saída padrão
    sleep_ms(1000);
    connect_to_wifi(WIFI_SSID, WIFI_PASS);
    mqtt_setup(CLIENT_ID, BROKER_IP, BROKER_USER, BROKER_PASS);
    sleep_ms(1000); // Delay para estabilizar conexão
    
    // Subscreve ao tópico
    mqtt_comm_subscribe(TOPIC);
    
    // Loop principal
    while (true) {
        cyw43_arch_poll();  // Necessário para manter o Wi-Fi ativo
        sleep_ms(100);      // Reduz o uso da CPU
    }

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}