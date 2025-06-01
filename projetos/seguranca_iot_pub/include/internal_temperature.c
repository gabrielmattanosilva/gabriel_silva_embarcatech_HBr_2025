#include "internal_temperature.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

float adc_to_celsius(uint16_t adc_value) {
    const float conversion_factor = 3.3f / (1 << 12);               // Fator de conversão de 12 bits (0-4095) para o intervalo de 0-3.3V
    float adc_voltage = adc_value * conversion_factor;              // Converte o valor do ADC para tensão
    float temperature = 27.0f + (adc_voltage - 0.706f) / 0.001721f; // Fórmula fornecida no datasheet para conversão em Celsius
    return temperature;
}

void setup_adc() {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_TEMPERATURE_CHANNEL);
}

void setup_temperature_sensor() {
    stdio_init_all();
    setup_adc();
}