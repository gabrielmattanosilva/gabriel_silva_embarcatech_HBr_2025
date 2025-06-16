/**
 * @file buttons.h
 * @brief Definições para o controle dos botões
 *
 * Contém as definições de pinos, constantes e protótipos de funções
 * para o controle dos botões.
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include "FreeRTOS.h"
#include "queue.h"

#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define DEBOUNCE_TIME_MS 100

void buttons_init(QueueHandle_t queue);
void buttons_task(void *pvParameters);

#endif // BUTTONS_H