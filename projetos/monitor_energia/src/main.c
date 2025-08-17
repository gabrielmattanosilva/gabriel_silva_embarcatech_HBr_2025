#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define I2C_PORT        i2c0
#define SDA_PIN         0
#define SCL_PIN         1
#define ADS1115_ADDR    0x48

#define NUM_SAMPLES     256
int16_t buffer_ch0[NUM_SAMPLES];
int16_t buffer_ch1[NUM_SAMPLES];
uint32_t timestamps_ch0[NUM_SAMPLES];
uint32_t timestamps_ch1[NUM_SAMPLES];

#define CONFIG_OS_SINGLE     (1 << 15)
#define CONFIG_MUX_AIN0      (0x4 << 12)
#define CONFIG_MUX_AIN1      (0x5 << 12)
#define CONFIG_PGA_4_096V    (0x1 << 9)
#define CONFIG_MODE_SINGLE   (1 << 8)
#define CONFIG_DR_860SPS     (0x7 << 5)
#define CONFIG_DEFAULT       (CONFIG_OS_SINGLE | CONFIG_PGA_4_096V | CONFIG_MODE_SINGLE | CONFIG_DR_860SPS)

#define LSB_4_096V (4.096 / 32768.0)

#define VOLT_DC_OFFSET 1.50
#define CURR_DC_OFFSET 1.65

#define VOLT_CONV_FACTOR 301.15
#define CURR_CONV_FACTOR 54.87

void i2c_init_ads() {
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

void ads1115_write(uint8_t reg, uint16_t value) {
    uint8_t data[3] = { reg, value >> 8, value & 0xFF };
    i2c_write_blocking(I2C_PORT, ADS1115_ADDR, data, 3, false);
}

int16_t ads1115_read_conversion() {
    uint8_t reg = 0x00;
    uint8_t val[2];
    i2c_write_blocking(I2C_PORT, ADS1115_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, ADS1115_ADDR, val, 2, false);
    return (int16_t)((val[0] << 8) | val[1]);
}

bool ads1115_conversion_ready() {
    uint8_t reg = 0x01;
    uint8_t val[2];
    i2c_write_blocking(I2C_PORT, ADS1115_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, ADS1115_ADDR, val, 2, false);
    return (val[0] & 0x80);
}

void sampling_task(void *params) {
    const TickType_t delay_between_cycles = pdMS_TO_TICKS(2000);
    const TickType_t sampling_period = pdMS_TO_TICKS(5); // 200Hz

    while (1) {
        TickType_t xLastWakeTime = xTaskGetTickCount();

        for (int i = 0; i < NUM_SAMPLES; i++) {
            // CH0
            ads1115_write(0x01, CONFIG_DEFAULT | CONFIG_MUX_AIN0);
            while (!ads1115_conversion_ready()) vTaskDelay(pdMS_TO_TICKS(1));
            int16_t ch0 = ads1115_read_conversion();
            uint32_t t_ch0 = to_ms_since_boot(get_absolute_time());

            // CH1
            ads1115_write(0x01, CONFIG_DEFAULT | CONFIG_MUX_AIN1);
            while (!ads1115_conversion_ready()) vTaskDelay(pdMS_TO_TICKS(1));
            int16_t ch1 = ads1115_read_conversion();
            uint32_t t_ch1 = to_ms_since_boot(get_absolute_time());

            buffer_ch0[i] = ch0;
            buffer_ch1[i] = ch1;
            timestamps_ch0[i] = t_ch0;
            timestamps_ch1[i] = t_ch1;

            vTaskDelayUntil(&xLastWakeTime, sampling_period);
        }

        // Impressão dos dados
        for (int i = 0; i < NUM_SAMPLES; i++) {
            printf("%d,%d,%lu,%lu\n",
                   buffer_ch0[i],
                   buffer_ch1[i],
                   timestamps_ch0[i],
                   timestamps_ch1[i]);
        }

        // Cálculo RMS
        double sum_sq_ch0 = 0;
        double sum_sq_ch1 = 0;

        for (int i = 0; i < NUM_SAMPLES; i++) {
            double volts0 = buffer_ch0[i] * LSB_4_096V - VOLT_DC_OFFSET;
            double volts1 = buffer_ch1[i] * LSB_4_096V - CURR_DC_OFFSET;
            sum_sq_ch0 += volts0 * volts0;
            sum_sq_ch1 += volts1 * volts1;
        }

        double rms0 = sqrt(sum_sq_ch0 / NUM_SAMPLES);
        double rms1 = sqrt(sum_sq_ch1 / NUM_SAMPLES);

        double rms0_real = rms0 * VOLT_CONV_FACTOR;
        double rms1_real = rms1 * CURR_CONV_FACTOR;

        printf("RMS_CH0: %.4f V | RMS_CH1: %.4f V\n\n", rms0, rms1);
        printf("RMS_CH0_REAL: %.2f V | RMS_CH1_REAL: %.2f A", rms0_real, rms1_real);

        // Espera até o próximo ciclo
        vTaskDelay(delay_between_cycles);
    }
}

int main() {
    stdio_init_all();
    i2c_init_ads();

    xTaskCreate(sampling_task, "SAMPLER", 2048, NULL, 1, NULL);

    vTaskStartScheduler();
    while (1);
}
