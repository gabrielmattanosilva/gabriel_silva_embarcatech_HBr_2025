/**
 * @file rgb_led.c
 * @brief Implementação do controle do LED RGB.
 *
 * Este arquivo contém a implementação das funções para inicialização e controle
 * do LED RGB, incluindo a tarefa que alterna entre as cores do LED.
 *
 */

#include "rgb_led.h"
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

static volatile bool led_task_suspended = false;
const uint8_t led_pins[] = {LED_R_PIN, LED_G_PIN, LED_B_PIN};

/**
 * @brief Tarefa do LED RGB.
 *
 * @param pvParameters Parâmetros da tarefa (não utilizado).
 *
 */
void rgb_led_task(void *pvParameters)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        gpio_init(led_pins[i]);
        gpio_set_dir(led_pins[i], GPIO_OUT);
        gpio_put(led_pins[i], 0);
    }

    uint8_t current_led = 0;
    while (true)
    {
        if (!led_task_suspended)
        {
            gpio_put(led_pins[current_led], 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_put(led_pins[current_led], 0);
            current_led = (current_led + 1) % 3;
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

/**
 * @brief Suspende a tarefa do LED.
 *
 */
void suspend_led_task(void)
{
    led_task_suspended = true;
    for (uint8_t i = 0; i < 3; i++)
    {
        gpio_put(led_pins[i], 0);
    }
}

/**
 * @brief Resume a tarefa do LED.
 *
 */
void resume_led_task(void)
{
    led_task_suspended = false;
}
