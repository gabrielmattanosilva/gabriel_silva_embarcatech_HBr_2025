/**
 * @file rgb_led.c
 * @brief Implementação do controle do LED RGB.
 *
 * Este arquivo contém a implementação das funções para inicialização e controle
 * do LED RGB.
 */

#include "rgb_led.h"
#include "pico/stdlib.h"

/**
 * @brief Inicializa o LED RGB.
 *
 */
void rgb_led_init(void)
{
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_put(LED_R_PIN, 0);

    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_put(LED_G_PIN, 0);

    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_put(LED_B_PIN, 0);
}

/**
 * @brief Define a cor do LED RGB.
 *
 * @param red Estado do LED vermelho (true = ligado, false = desligado).
 * @param green Estado do LED verde (true = ligado, false = desligado).
 * @param blue Estado do LED azul (true = ligado, false = desligado).
 */
void set_led_color(bool red, bool green, bool blue)
{
    gpio_put(LED_R_PIN, red);
    gpio_put(LED_G_PIN, green);
    gpio_put(LED_B_PIN, blue);
}
