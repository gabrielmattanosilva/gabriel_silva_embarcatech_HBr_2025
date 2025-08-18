#include "ads1115_adc.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

void ads1115_init(void)
{
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

void ads1115_write(uint8_t reg, uint16_t value)
{
    uint8_t data[3] = {reg, value >> 8, value & 0xFF};
    i2c_write_blocking(I2C_PORT, ADS1115_ADDR, data, 3, false);
}

int16_t ads1115_read_conversion(void)
{
    uint8_t reg = 0x00;
    uint8_t val[2];
    i2c_write_blocking(I2C_PORT, ADS1115_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, ADS1115_ADDR, val, 2, false);
    return (int16_t)((val[0] << 8) | val[1]);
}

bool ads1115_conversion_ready(void)
{
    uint8_t reg = 0x01;
    uint8_t val[2];
    i2c_write_blocking(I2C_PORT, ADS1115_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, ADS1115_ADDR, val, 2, false);
    return (val[0] & 0x80);
}
