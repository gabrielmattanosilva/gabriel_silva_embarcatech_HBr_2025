/**
 * @file main.c
 * @brief Ponto de entrada: cria tasks (Wi‑Fi, EnergyMonitor, ThingSpeak) e inicia o scheduler.
 * @details
 *  Inicializa subsistemas (stdio/logger/RTC/ADS1115/Wi‑Fi) e agenda as tasks:
 *   - WiFiManagerTask: gerencia conexão e NTP
 *   - EnergyMonitorTask: amostra e calcula RMS/PU/Pinst
 *   - ThingSpeakTask: acumula energia e envia telemetria
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "logger.h"
#include "rtc_ntp.h"
#include "ads1115_adc.h"
#include "energy_monitor.h"
#include "credentials.h"
#include "wifi_manager.h"
#include "thingspeak.h"

/**
 * @brief Função principal do firmware.
 * @return 0 (nunca retorna após `vTaskStartScheduler`).
 */
int main(void)
{
    /* Inicialização periféricos */
    stdio_init_all();
    logger_init();
    rtc_ntp_init();
    ads1115_init();
    wifi_manager_init(SSID, PASSWORD);

    /* Criação das tarefas */
    xTaskCreate(
        wifi_manager_task,
        "WiFiManagerTask",
        2048,
        NULL,
        tskIDLE_PRIORITY + 2,
        NULL);

    xTaskCreate(
        energy_monitor_task,
        "EnergyMonitorTask",
        2048,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL);

    xTaskCreate(
        thingspeak_task,
        "ThingSpeakTask",
        3072,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL);

    /* Inicialização do escalonador */
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }
    return 0;
}
