/**
 * @file buzzer.c
 * @brief Implementação do controle do buzzer.
 *
 * Este arquivo contém a implementação das funções para inicialização e controle
 * do buzzer, incluindo a tarefa que gera o sinal PWM para o buzzer.
 */

#include "buzzer.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "FreeRTOS.h"
#include "task.h"

static volatile bool buzzer_suspended = false;
static uint slice_num;
static uint channel;

/**
 * @brief Inicializa o buffer.
 *
 */
void buzzer_init(void)
{
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

/**
 * @brief Tarefa do buzzer.
 *
 * @param pvParameters Parâmetros da tarefa (não utilizado).
 *
 */
void buzzer_task(void *pvParameters)
{
    buzzer_init();

    while (true)
    {
        if (!buzzer_suspended)
        {
            uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
            uint32_t clock_div = clock_get_hz(clk_sys) / (5000 * 4096);
            pwm_set_clkdiv(slice_num, clock_div);
            pwm_set_gpio_level(BUZZER_PIN, 2048);
            vTaskDelay(pdMS_TO_TICKS(200));
            pwm_set_gpio_level(BUZZER_PIN, 0);
            vTaskDelay(pdMS_TO_TICKS(800));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

/**
 * @brief Suspende a tarefa do buzzer.
 *
 */
void suspend_buzzer_task(void)
{
    buzzer_suspended = true;
    pwm_set_chan_level(slice_num, channel, 0);
}

/**
 * @brief Resume a tarefa do buzzer.
 *
 */
void resume_buzzer_task(void)
{
    buzzer_suspended = false;
}