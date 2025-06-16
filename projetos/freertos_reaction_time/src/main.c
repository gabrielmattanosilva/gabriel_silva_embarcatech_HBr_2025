/**
 * @file main.c
 * @brief Arquivo principal do projeto de teste de tempo de reação.
 *
 * Este arquivo implementa um teste de tempo de reação onde:
 * 1. O sistema começa em estado idle
 * 2. Ao pressionar o botão A, inicia um contador aleatório de 2-5 segundos
 * 3. Após o tempo aleatório, acende o LED vermelho
 * 4. O usuário deve pressionar o botão B o mais rápido possível
 * 5. O sistema mede e exibe o tempo de reação
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "buttons.h"
#include "rgb_led.h"

/* Estados do sistema */
typedef enum
{
  STATE_IDLE,
  STATE_WAITING,
  STATE_LED_ON,
  STATE_DONE
} system_state_t;

static system_state_t current_state = STATE_IDLE;
static uint32_t led_on_time = 0;
static uint32_t reaction_time = 0;

/**
 * @brief Tarefa principal do teste de reação.
 *
 * @param pvParameters Parâmetros da tarefa (não utilizado).
 */
void reaction_test_task(void *pvParameters)
{
  rgb_led_init();

  while (true)
  {
    switch (current_state)
    {
    case STATE_IDLE:
      // Nada a fazer, esperando botão A
      vTaskDelay(pdMS_TO_TICKS(100));
      break;

    case STATE_WAITING:
    {
      // Gera um tempo aleatório entre 2000 e 5000 ms
      uint32_t random_delay = (rand() % 3000) + 2000;
      vTaskDelay(pdMS_TO_TICKS(random_delay));

      // Acende o LED vermelho e registra o tempo
      set_led_color(true, false, false);
      led_on_time = to_ms_since_boot(get_absolute_time());
      current_state = STATE_LED_ON;
      printf("LED ON! Press button B as fast as you can!\n");
      break;
    }

    case STATE_LED_ON:
      // Esperando o usuário pressionar o botão B
      vTaskDelay(pdMS_TO_TICKS(10));
      break;

    case STATE_DONE:
      // Mostra o resultado e volta para idle
      printf("Reaction time: %d ms\n", reaction_time);
      set_led_color(false, false, false);
      vTaskDelay(pdMS_TO_TICKS(1000));
      current_state = STATE_IDLE;
      printf("System ready. Press button A to start.\n");
      break;
    }
  }
}

/**
 * @brief Callback para o botão A (inicia o teste).
 */
static void button_a_handler(void)
{
  if (current_state == STATE_IDLE)
  {
    current_state = STATE_WAITING;
    printf("Test started. Waiting for random delay...\n");
  }
}

/**
 * @brief Callback para o botão B (registra tempo de reação).
 */
static void button_b_handler(void)
{
  if (current_state == STATE_LED_ON)
  {
    reaction_time = to_ms_since_boot(get_absolute_time()) - led_on_time;
    set_led_color(false, false, false);
    current_state = STATE_DONE;
  }
}

/**
 * @brief Função principal.
 */
int main(void)
{
  /* Inicializações básicas */
  stdio_init_all();

  /* Configuração dos botões */
  buttons_init();
  register_button_a_callback(button_a_handler);
  register_button_b_callback(button_b_handler);

  /* Inicializa o gerador de números aleatórios */
  srand(to_ms_since_boot(get_absolute_time()));

  /* Criação da tarefa */
  xTaskCreate(reaction_test_task,
              "Reaction Test Task",
              256,
              NULL,
              1,
              NULL);

  /* Mensagem inicial */
  printf("Reaction Time Test\n");
  printf("Press button A to start the test\n");

  /* Inicia o escalonador */
  vTaskStartScheduler();

  /* Nunca deverá chegar aqui */
  while (true)
  {
    printf("ERROR: Scheduler exited!\n");
    sleep_ms(1000);
  }
}