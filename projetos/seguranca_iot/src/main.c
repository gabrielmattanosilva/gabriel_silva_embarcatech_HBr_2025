#include <string.h>
#include <stdio.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "include/credentials.h"
#include "include/wifi_conn.h"

int main() {
    stdio_init_all();  // Inicializa a saída padrão
    sleep_ms(1000);

    connect_to_wifi(WIFI_SSID, WIFI_PASS);

    // Loop principal
    while (true) {
        cyw43_arch_poll();  // Necessário para manter o Wi-Fi ativo
        sleep_ms(100);      // Reduz o uso da CPU
    }

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}