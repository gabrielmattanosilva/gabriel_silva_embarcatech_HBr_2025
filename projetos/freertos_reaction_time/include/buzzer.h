/**
 * @file buzzer.h
 * @brief Definições para o controle do buzzer.
 *
 * Este arquivo contém as definições para inicialização e controle do buzzer,
 * incluindo suspensão e retomada da tarefa do buzzer.
 */

#ifndef BUZZER_H
#define BUZZER_H

#include "FreeRTOS.h"
#include "task.h"

#define BUZZER_PIN 21

extern TaskHandle_t buzzer_task_handle;

void buzzer_task(void *pvParameters);
void suspend_buzzer_task(void);
void resume_buzzer_task(void);

#endif // BUZZER_H