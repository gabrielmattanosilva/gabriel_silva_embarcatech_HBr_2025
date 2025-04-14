#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

volatile int counter = 0;
volatile int button_b_presses = 0;
volatile bool counting_active = false;
volatile bool button_b_pressed = false;

// Função de callback para a interrupção do Botão A
void button_a_callback(uint gpio, uint32_t events)
{
    if (gpio == BUTTON_A_PIN)
    {
        counter = 9;
        button_b_presses = 0;
        counting_active = true;
    }
}

// Função de callback para a interrupção do Botão B
void button_b_callback(uint gpio, uint32_t events)
{
    if (gpio == BUTTON_B_PIN && counting_active)
    {
        button_b_presses++;
        button_b_pressed = true;
    }
}

int main()
{
    // Inicializa a comunicação serial
    stdio_init_all();

    // Configura os pinos dos botões como entrada com pull-up
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Configura as interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_a_callback);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_add_raw_irq_handler(BUTTON_B_PIN, &button_b_callback);

    // Habilita interrupções globais
    irq_set_enabled(IO_IRQ_BANK0, true);

    printf("Sistema iniciado. Pressione o Botão A (GPIO5) para começar.\n");

    absolute_time_t last_count_time = get_absolute_time();

    while (true)
    {
        if (counting_active)
        {
            // Verifica se passou 1 segundo desde o último decremento
            absolute_time_t current_time = get_absolute_time();
            int64_t time_diff = absolute_time_diff_us(last_count_time, current_time) / 1000;

            if (time_diff >= 1000)
            {
                if (counter > 0)
                {
                    counter--;
                    last_count_time = current_time;
                }
                else
                {
                    counting_active = false;
                }
            }

            // Atualiza a saída serial
            printf("Contador: %d | Cliques no Botão B: %d\n", counter, button_b_presses);

            // Se o botão B foi pressionado, atualiza imediatamente
            if (button_b_pressed)
            {
                printf("Contador: %d | Cliques no Botão B: %d\n", counter, button_b_presses);
                button_b_pressed = false;
            }

            // Pequena pausa para evitar sobrecarga da saída serial
            sleep_ms(100);
        }
        else if (counter == 0)
        {
            // Estado congelado após contagem
            printf("Sistema congelado. Contador: 0 | Total de cliques no Botão B: %d\n", button_b_presses);
            printf("Pressione o Botão A (GPIO5) para reiniciar.\n");
            sleep_ms(1000);
        }
    }

    return 0;
}