#include <string.h>
#include <stdio.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "include/credentials.h"
#include "include/wifi_conn.h"
#include "include/mqtt_conn.h"

#define CLIENT_ID   "bitdog1"
#define BROKER_IP   "192.168.3.196"
#define USER        "aluno"
#define BROKER_PASS "bitdoglab"

int main() {
    stdio_init_all();  // Inicializa a saída padrão
    sleep_ms(1000);
    connect_to_wifi(WIFI_SSID, WIFI_PASS);
    mqtt_setup(CLIENT_ID, BROKER_IP, USER, BROKER_PASS);
    sleep_ms(1000); // Delay para estabilizar conexão
    mqtt_comm_publish("escola/sala1/temperatura", "26.5", strlen("26.5"));

    // Loop principal
    while (true) {
        cyw43_arch_poll();  // Necessário para manter o Wi-Fi ativo
        sleep_ms(100);      // Reduz o uso da CPU
    }

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}