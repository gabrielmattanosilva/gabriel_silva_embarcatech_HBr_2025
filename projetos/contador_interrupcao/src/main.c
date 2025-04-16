#include <stdio.h>
#include "interrupt_counter.h"
#include "pico/stdlib.h"

int main()
{
    // Inicializa comunicação serial e periféricos
    stdio_init_all();
    init_display();
    init_buttons();

    // Configura timer para callback periódico
    repeating_timer_t timer;
    add_repeating_timer_ms(1000, timer_callback, NULL, &timer);

    // Loop principal
    while (true)
    {
        // Verifica se é necessário atualizar o display
        if (app_state.update_display)
        {
            update_display(&app_state);
            app_state.update_display = false;
        }
        sleep_ms(50); // Pequeno delay para reduzir consumo de CPU
    }

    return 0;
}