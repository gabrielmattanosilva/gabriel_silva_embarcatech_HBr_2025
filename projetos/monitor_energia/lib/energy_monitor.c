/**
 * @file energy_monitor.c
 * @brief Cálculo de Vrms, Irms, potência instantânea e tensão por unidade (PU).
 * @details
 *  Executa uma task periódica que amostra dois canais do ADS1115 (tensão e corrente),
 *  acumula amostras, calcula valores RMS e publica o último resultado via
 *  `energy_monitor_get_last()`. Amostragem nominal: 200 Hz por ~1 s (128 amostras).
 */

#include "lib/energy_monitor.h"
#include <math.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lib/ads1115_adc.h"
#include "lib/logger.h"

#define TAG "energy_monitor"

#define NUM_SAMPLES     128U               /**< Número de amostras por canal. */
#define SAMPLE_RATE_HZ  200U               /**< Taxa de amostragem por canal (Hz). */
#define CYCLE_PERIOD_MS 1000U              /**< Período da task (ms). */

#define LSB_4_096V (4.096f / 32768.0f)     /**< Tamanho do LSB na faixa ±4.096V. */

#define VOLT_DC_OFFSET 1.50f               /**< Offset DC do canal de tensão (V). */
#define CURR_DC_OFFSET 1.65f               /**< Offset DC do canal de corrente (V). */

#define VOLT_CONV_FACTOR 301.15f           /**< Fator V_adc->V_real. */
#define CURR_CONV_FACTOR 54.87f            /**< Fator V_adc->I_real (A/V). */

#define VBASE_RMS 127.00f                  /**< Base de tensão para PU (127 Vrms). */

static int16_t buffer_ch0[NUM_SAMPLES];
static int16_t buffer_ch1[NUM_SAMPLES];
static uint32_t timestamps_ch0[NUM_SAMPLES];
static uint32_t timestamps_ch1[NUM_SAMPLES];

static energy_monitor_data_t g_last = {0};
static volatile bool g_last_valid = false;

/**
 * @brief Obtém a última medição calculada pela task.
 * @param[out] out Estrutura preenchida com os últimos valores.
 * @return true se havia dados válidos; false caso contrário.
 * @note Acesso protegido por seção crítica para consistência.
 */
bool energy_monitor_get_last(energy_monitor_data_t *out)
{
    if (!out || !g_last_valid)
    {
        return false;
    }

    taskENTER_CRITICAL();
    *out = g_last;
    taskEXIT_CRITICAL();
    return true;
}

/**
 * @brief Task FreeRTOS de monitoramento de energia (amostragem e cálculo).
 * @param params Parâmetro opcional (não utilizado).
 * @note A amostragem alterna AIN0 e AIN1 em modo single-shot.
 */
void energy_monitor_task(void *params)
{
    (void)params;

    const TickType_t cycle_period = pdMS_TO_TICKS(CYCLE_PERIOD_MS);
    const TickType_t sampling_period = pdMS_TO_TICKS(1000 / SAMPLE_RATE_HZ);

    TickType_t cycle_wake = xTaskGetTickCount();

    while (1)
    {
        vTaskDelayUntil(&cycle_wake, cycle_period);

        TickType_t sample_wake = xTaskGetTickCount();

        for (int i = 0; i < NUM_SAMPLES; i++)
        {
            ads1115_write(0x01, (uint16_t)(CONFIG_DEFAULT | CONFIG_MUX_AIN0));
            while (!ads1115_conversion_ready())
            {
                taskYIELD();
            }
            int16_t ch0 = ads1115_read_conversion();
            uint32_t t_ch0 = to_ms_since_boot(get_absolute_time());

            ads1115_write(0x01, (uint16_t)(CONFIG_DEFAULT | CONFIG_MUX_AIN1));
            while (!ads1115_conversion_ready())
            {
                taskYIELD();
            }
            int16_t ch1 = ads1115_read_conversion();
            uint32_t t_ch1 = to_ms_since_boot(get_absolute_time());

            buffer_ch0[i] = ch0;
            buffer_ch1[i] = ch1;
            timestamps_ch0[i] = t_ch0;
            timestamps_ch1[i] = t_ch1;

            vTaskDelayUntil(&sample_wake, sampling_period);
        }

        double sum_sq_ch0 = 0.0;
        double sum_sq_ch1 = 0.0;

        for (int i = 0; i < NUM_SAMPLES; i++)
        {
            const double v0 = (double)buffer_ch0[i] * (double)LSB_4_096V - (double)VOLT_DC_OFFSET;
            const double v1 = (double)buffer_ch1[i] * (double)LSB_4_096V - (double)CURR_DC_OFFSET;
            sum_sq_ch0 += v0 * v0;
            sum_sq_ch1 += v1 * v1;
        }

        const double rms0_adc = sqrt(sum_sq_ch0 / NUM_SAMPLES);
        const double rms1_adc = sqrt(sum_sq_ch1 / NUM_SAMPLES);
        const double vrms_real = rms0_adc * (double)VOLT_CONV_FACTOR;
        const double irms_real = rms1_adc * (double)CURR_CONV_FACTOR;
        const double p_instant = vrms_real * irms_real;
        const double v_pu = vrms_real / (double)VBASE_RMS;

        const int last = NUM_SAMPLES - 1;

        const uint32_t t_ms = (timestamps_ch0[last] > timestamps_ch1[last])
                                  ? timestamps_ch0[last]
                                  : timestamps_ch1[last];

        taskENTER_CRITICAL();
        g_last.vrms = vrms_real;
        g_last.irms = irms_real;
        g_last.v_pu = v_pu;
        g_last.p_instant = p_instant;
        g_last.t_ms = t_ms;
        g_last_valid = true;
        taskEXIT_CRITICAL();

        LOG(TAG, "V=%.2f V (PU=%.3f) | I=%.3f A | Pinst=%.1f W | t=%u ms",
            vrms_real, v_pu, irms_real, p_instant, t_ms);
    }
}
