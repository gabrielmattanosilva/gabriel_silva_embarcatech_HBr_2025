#include <stdio.h>
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "internal_temperature.h"

int main() {
    setup_temperature_sensor();

    while (true) {
        uint16_t adc_value = adc_read();
        float temperature = adc_to_celsius(adc_value);
        printf("Temperatura: %.2f °C\n", temperature);
        sleep_ms(1000);
    }

    return 0;
}