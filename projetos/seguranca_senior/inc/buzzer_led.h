#ifndef BUZZER_LED_H
#define BUZZER_LED_H

/**
 * @file buzzer_led.h
 * @brief Biblioteca para controle de buzzer e LED via GPIO/PWM.
 *
 * Este cabeçalho contém as definições de pinos e as declarações de funções
 * para controlar buzzers e LEDs.
 * 
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

// Definições de pinos
#define BUZZER1_PIN 21  ///< Pino GPIO do primeiro buzzer
#define BUZZER2_PIN 10  ///< Pino GPIO do segundo buzzer
#define LED_GREEN  11   ///< Pino GPIO do LED verde
#define LED_RED    13   ///< Pino GPIO do LED vermelho

// Declaração das funções
void buzzer_led_init();
void buzzer_led_init_success();
void buzzer_led_fail();
void buzzer_led_msg_1();
void buzzer_led_msg_2();
void buzzer_led_msg_3();
void buzzer_led_msg_4();
 
#endif // BUZZER_LED_H
