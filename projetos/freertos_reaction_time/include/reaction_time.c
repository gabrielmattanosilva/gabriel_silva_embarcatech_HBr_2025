/**
 * @file reaction_time.c
 * @brief Implementação da lógica principal do teste de tempo de reação
 *
 * Contém a implementação da máquina de estados que controla o teste
 * de tempo de reação, incluindo geração de atrasos aleatórios e
 * cálculo do tempo de resposta.
 */

#include "reaction_time.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "rgb_led.h"

static volatile system_state_t current_state = STATE_IDLE;
static uint32_t led_on_time = 0;
static uint32_t reaction_time = 0;
static QueueHandle_t button_queue = NULL;

/**
 * @brief Inicializa o módulo de tempo de reação
 */
void reaction_time_init(void)
{
    button_queue = xQueueCreate(10, sizeof(button_event_t));
    rgb_led_init();
    printf("Aperte o botao A para iniciar.\n");
}

/**
 * @brief Obtém a fila de eventos de botão
 * @return QueueHandle_t Ponteiro para a fila de eventos
 */
QueueHandle_t get_button_queue(void)
{
    return button_queue;
}

/**
 * @brief Obtém o estado atual do sistema
 * @return system_state_t Estado atual
 */
system_state_t get_current_state(void)
{
    return current_state;
}

/**
 * @brief Obtém o último tempo de reação medido
 * @return uint32_t Tempo de reação em ms
 */
uint32_t get_reaction_time(void)
{
    return reaction_time;
}

/**
 * @brief Tarefa principal do teste de tempo de reação
 * @param pvParameters Parâmetros da tarefa (não utilizado)
 */
void reaction_time_task(void *pvParameters)
{
    while (true)
    {
        switch (current_state)
        {
        case STATE_IDLE:
            set_led_color(false, false, false);
            vTaskDelay(pdMS_TO_TICKS(100));
            break;

        case STATE_WAITING:
        {
            printf("Espere pelo LED vermelho e aperte o botão B.\n");
            /* Gera um delay aleatório de 2 a 5 segundos */
            uint32_t random_delay = (rand() % 3000) + 2000;
            vTaskDelay(pdMS_TO_TICKS(random_delay));
            set_led_color(true, false, false);
            led_on_time = to_ms_since_boot(get_absolute_time());
            current_state = STATE_LED_ON;
            break;
        }

        case STATE_LED_ON:
            vTaskDelay(pdMS_TO_TICKS(10));
            break;

        case STATE_DONE:
            printf("Tempo de reacao: %d ms\n", reaction_time);
            set_led_color(false, false, false);
            vTaskDelay(pdMS_TO_TICKS(1000));
            current_state = STATE_IDLE;
            printf("Aperte o botao A para reiniciar.\n");
            break;
        }
    }
}

/**
 * @brief Processa eventos de botão de acordo com o estado atual
 * @param event Tipo de evento de botão recebido
 */
void process_button_event(button_event_t event)
{
    switch (event)
    {
    case EVENT_BUTTON_A_PRESSED:
        if (current_state == STATE_IDLE)
        {
            current_state = STATE_WAITING;
        }
        break;

    case EVENT_BUTTON_B_PRESSED:
        if (current_state == STATE_LED_ON)
        {
            reaction_time = to_ms_since_boot(get_absolute_time()) - led_on_time;
            current_state = STATE_DONE;
        }
        break;

    default:
        break;
    }
}