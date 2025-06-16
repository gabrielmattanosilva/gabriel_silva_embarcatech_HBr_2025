/**
 * @file reaction_time.h
 * @brief Definições para o teste de tempo de reação
 *
 * Contém as definições de estados, eventos e protótipos de funções
 * para o teste de tempo de reação.
 */

#ifndef REACTION_TIME_H
#define REACTION_TIME_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

/**
 * @brief Estados possíveis do sistema
 */
typedef enum
{
    STATE_IDLE,
    STATE_WAITING,
    STATE_LED_ON,
    STATE_DONE
} system_state_t;

/**
 * @brief Tipos de eventos de botão
 */
typedef enum
{
    EVENT_NONE,
    EVENT_BUTTON_A_PRESSED,
    EVENT_BUTTON_B_PRESSED
} button_event_t;

void reaction_time_init(void);
void reaction_time_task(void *pvParameters);
void process_button_event(button_event_t event);
uint32_t get_reaction_time(void);
system_state_t get_current_state(void);
QueueHandle_t get_button_queue(void);

#endif // REACTION_TIME_H