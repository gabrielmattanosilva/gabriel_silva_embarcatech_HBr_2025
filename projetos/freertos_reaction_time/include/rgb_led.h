/**
 * @file rgb_led.h
 * @brief Definições para o controle do LED RGB.
 *
 * Este arquivo contém as definições para inicialização e controle do LED RGB.
 */

#ifndef RGB_LED_H
#define RGB_LED_H

#include <stdbool.h>

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

void rgb_led_init(void);
void set_led_color(bool red, bool green, bool blue);

#endif // RGB_LED_H