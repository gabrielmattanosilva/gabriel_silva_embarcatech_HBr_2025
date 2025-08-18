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

int main(void)
{
    stdio_init_all();
    logger_init();
    rtc_ntp_init();
    ads1115_init();
    wifi_manager_init(SSID, PASSWORD);

    xTaskCreate(
        wifi_manager_task,
        "WiFi Manager Task",
        2048,
        NULL,
        tskIDLE_PRIORITY + 2,
        NULL);

    xTaskCreate(
        energy_monitor_task,
        "Energy Monitor Task",
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

    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }
    return 0;
}
