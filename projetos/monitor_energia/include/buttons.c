/**
 * @file buttons.c
 * @brief Implementação do controle dos botões.
 *
 * Este arquivo contém a implementação das funções para inicialização e tratamento
 * de interrupções dos botões A e B, incluindo debounce e chamadas de callback.
 */

#include "buttons.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

static button_callback_t button_a_callback = NULL;
static button_callback_t button_b_callback = NULL;
static volatile uint32_t last_button_a_time = 0;
static volatile uint32_t last_button_b_time = 0;
static volatile bool button_a_pressed = false;
static volatile bool button_b_pressed = false;

/**
 * @brief Função de interrupção para os botões.
 *
 * @param gpio Pino GPIO que gerou a interrupção.
 * @param events Eventos que triggaram a interrupção.
 *
 */
static void button_isr(uint gpio, uint32_t events)
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (gpio == BUTTON_A_PIN)
    {
        if ((current_time - last_button_a_time) > DEBOUNCE_TIME_MS)
        {
            last_button_a_time = current_time;
            button_a_pressed = true;
            if (button_a_callback)
            {
                button_a_callback();
            }
        }
    }
    else if (gpio == BUTTON_B_PIN)
    {
        if ((current_time - last_button_b_time) > DEBOUNCE_TIME_MS)
        {
            last_button_b_time = current_time;
            button_b_pressed = true;
            if (button_b_callback)
            {
                button_b_callback();
            }
        }
    }
}

/**
 * @brief Inicializa os botões.
 *
 */
void buttons_init(void)
{
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, button_isr);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);
}

/**
 * @brief Registra um callback para o botão A.
 *
 * @param callback Função a ser chamada quando o botão A for pressionado.
 *
 */
void register_button_a_callback(button_callback_t callback)
{
    button_a_callback = callback;
}

/**
 * @brief Registra um callback para o botão B.
 *
 * @param callback Função a ser chamada quando o botão B for pressionado.
 *
 */
void register_button_b_callback(button_callback_t callback)
{
    button_b_callback = callback;
}
