#ifndef INTERNAL_TEMPERATURE_H
#define INTERNAL_TEMPERATURE_H

#include <stdint.h>

#define ADC_TEMPERATURE_CHANNEL 4

/**
 * @brief Converte valor ADC para temperatura em Celsius
 * @param adc_value Valor lido do ADC (12 bits)
 * @return Temperatura em graus Celsius
 */
float adc_to_celsius(uint16_t adc_value);

/**
 * @brief Configura o ADC para leitura do sensor de temperatura interno
 */
void setup_adc();

/**
 * @brief Configuração inicial do sistema
 */
void setup_temperature_sensor();

#endif // INTERNAL_TEMPERATURE_H