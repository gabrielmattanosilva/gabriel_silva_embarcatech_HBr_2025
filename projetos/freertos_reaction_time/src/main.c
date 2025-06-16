#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "buttons.h"
#include "rgb_led.h"
#include "reaction_time.h"

/**
 * @brief Callback para o botão A
 */
static void button_a_handler(void)
{
  button_event_t event = EVENT_BUTTON_A_PRESSED;
  xQueueSendFromISR(get_button_queue(), &event, NULL);
}

/**
 * @brief Callback para o botão B
 */
static void button_b_handler(void)
{
  button_event_t event = EVENT_BUTTON_B_PRESSED;
  xQueueSendFromISR(get_button_queue(), &event, NULL);
}

/**
 * @brief Tarefa de processamento de botões
 */
void button_task(void *pvParameters)
{
  button_event_t event;
  while (true)
  {
    if (xQueueReceive(get_button_queue(), &event, portMAX_DELAY))
    {
      process_button_event(event);
    }
  }
}

int main(void)
{
  stdio_init_all();

  /* Inicialização do módulo */
  reaction_time_init();

  /* Configuração dos botões */
  buttons_init();
  register_button_a_callback(button_a_handler);
  register_button_b_callback(button_b_handler);

  /* Inicialização do RNG */
  srand(to_ms_since_boot(get_absolute_time()));

  /* Criação das tarefas */
  xTaskCreate(reaction_time_task, "Reaction Task", 256, NULL, 2, NULL);
  xTaskCreate(button_task, "Button Task", 256, NULL, 1, NULL);

  printf("System Ready\n");
  vTaskStartScheduler();

  while (true)
  {
    printf("ERROR: Scheduler exited!\n");
    sleep_ms(1000);
  }
}