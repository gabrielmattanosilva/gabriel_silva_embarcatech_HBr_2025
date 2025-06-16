/**
 * @file rgb_led.h
 * @brief Definições para o controle do LED RGB.
 *
 * Este arquivo contém as definições para inicialização e controle do LED RGB,
 * incluindo suspensão e retomada da tarefa do LED.
 */

#ifndef RGB_LED_H
#define RGB_LED_H

#include "FreeRTOS.h"
#include "task.h"

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

extern TaskHandle_t rgb_task_handle;

void rgb_led_task(void *pvParameters);
void suspend_led_task(void);
void resume_led_task(void);

#endif // RGB_LED_H