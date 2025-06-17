/**
 * @file main.c
 * @brief Arquivo principal do projeto de teste de tempo de reação
 *
 * Contém a função main que inicializa o hardware, cria as tarefas
 * e inicia o escalonador do FreeRTOS.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "buttons.h"
#include "display_oled.h"
#include "reaction_time.h"

int main(void)
{
    /* Inicializações básicas */
    stdio_init_all();
    sleep_ms(1000);

    /* Inicializa o display OLED */
    display_init();

    /* Inicializa o módulo de reaction_time (cria a fila) */
    reaction_time_init();

    /* Inicializa os botões com a fila */
    buttons_init(get_button_queue());

    /* Inicializa o gerador de números pseudoaleatórios */
    srand(to_ms_since_boot(get_absolute_time()));

    /* Criação das tarefas */
    xTaskCreate(reaction_time_task,
                "Reaction Task",
                256,
                NULL,
                2,
                NULL);

    xTaskCreate(buttons_task,
                "Button Task",
                256,
                NULL,
                1,
                NULL);

    /* Inicia o escalonador*/
    vTaskStartScheduler();

    /* Nunca deverá chegar aqui */
    while (true)
    {
        printf("ERROR: Scheduler exited!\n");
        sleep_ms(1000);
    }
}