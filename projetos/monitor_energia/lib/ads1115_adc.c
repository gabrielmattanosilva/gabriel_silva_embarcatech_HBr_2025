/**
 * @file ads1115_adc.c
 * @brief Interface mínima com o ADC ADS1115 via I2C para Raspberry Pi Pico.
 * @details
 *  Fornece funções de inicialização do barramento I2C, escrita de registradores,
 *  leitura do registrador de conversão e checagem de término de conversão.
 *  As rotinas utilizam a API `i2c_*` do Pico SDK.
 */

#include "lib/ads1115_adc.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT        i2c0        /**< Porta I2C utilizada. */
#define SDA_PIN         0U          /**< GPIO para linha SDA. */
#define SCL_PIN         1U          /**< GPIO para linha SCL. */
#define ADS1115_ADDR    0x48        /**< Endereço I2C do ADS1115 (A0=GND). */

/**
 * @brief Inicializa o I2C e configura os pinos para o ADS1115.
 * @note Usa 100 kHz e resistores de pull-up internos.
 */
void ads1115_init(void)
{
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

/**
 * @brief Escreve um valor de 16 bits em um registrador do ADS1115.
 * @param reg Endereço do registrador (por exemplo, 0x01 para Config).
 * @param value Valor a ser gravado (MSB primeiro).
 */
void ads1115_write(uint8_t reg, uint16_t value)
{
    uint8_t data[3] = {reg, value >> 8, value & 0xFF};
    i2c_write_blocking(I2C_PORT, ADS1115_ADDR, data, 3, false);
}

/**
 * @brief Lê o registrador de conversão (0x00) do ADS1115.
 * @return Amostra assinada de 16 bits no formato big-endian do dispositivo.
 */
int16_t ads1115_read_conversion(void)
{
    uint8_t reg = 0x00;
    uint8_t val[2];
    i2c_write_blocking(I2C_PORT, ADS1115_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, ADS1115_ADDR, val, 2, false);
    return (int16_t)((val[0] << 8) | val[1]);
}

/**
 * @brief Verifica se há conversão pronta lendo o registrador de configuração (0x01).
 * @return true se o bit OS (bit15) indicar conversão concluída; false caso contrário.
 */
bool ads1115_conversion_ready(void)
{
    uint8_t reg = 0x01;
    uint8_t val[2];
    i2c_write_blocking(I2C_PORT, ADS1115_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, ADS1115_ADDR, val, 2, false);
    return (val[0] & 0x80);
}
