#include <stdio.h>
#include "hardware/adc.h"
#include "pico/stdlib.h"

#define ADC_TEMPERATURE_CHANNEL 4

// Função que converte o valor lido do ADC (valor digital de 12 bits) para a temperatura em graus Celsius
float adc_to_temperature(uint16_t adc_value)
{
    const float conversion_factor = 3.3f / (1 << 12);               // Fator de conversão de 12 bits (0-4095) para o intervalo de 0-3.3V
    float adc_voltage = adc_value * conversion_factor;              // Converte o valor do ADC para tensão
    float temperature = 27.0f - (adc_voltage - 0.706f) / 0.001721f; // Fórmula fornecida no datasheet para conversão em Celsius
    return temperature;
}

int main()
{
    stdio_init_all();
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_TEMPERATURE_CHANNEL);

    while (true)
    {
        uint16_t adc_value = adc_read();
        float temperature = adc_to_temperature(adc_value);
        printf("Temperatura: %.2f °C\n", temperature);
        sleep_ms(1000);
    }

    return 0;
}
