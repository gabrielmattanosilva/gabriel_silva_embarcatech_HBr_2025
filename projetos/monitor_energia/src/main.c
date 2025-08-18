#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#include "ads1115_adc.h"
#include "energy_monitor.h"
#include "wifi_manager.h"
#include "credentials.h" // defina SSID e PASSWORD aqui
#include "logger.h"
#include "rtc_ntp.h"
#include "thingspeak.h"

int main(void)
{
    stdio_init_all();
    logger_init(); // <- inicializa mutex e nível padrão
    rtc_ntp_init();

    // Inicializa I2C e ADS1115
    ads1115_init();

    // Inicializa parâmetros do Wi-Fi
    wifi_manager_begin(SSID, PASSWORD);

    // Cria a task do Wi-Fi Manager
    xTaskCreate(
        wifi_manager_task,
        "WiFi Manager Task",
        2048, // stack
        NULL,
        tskIDLE_PRIORITY + 2, // prioridade maior para gerenciar conexões
        NULL);

    // Cria a task de amostragem de energia
    xTaskCreate(
        energy_monitor_task,
        "Energy Monitor Task",
        2048, // stack
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL);

    xTaskCreate(
        thingspeak_task,
        "ThingSpeakTask",
        3072, // ajuste se precisar
        NULL,
        tskIDLE_PRIORITY + 1, // prioridade média
        NULL);

    // Inicia o escalonador
    vTaskStartScheduler();

    // Não deve chegar aqui
    while (true)
    {
        tight_loop_contents();
    }
    return 0;
}
