#include <stdio.h>
#include <string.h>
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "interrupt_counter.h"
#include "pico/time.h"

// Estado inicial da aplicação
AppState app_state = {
    .counter = 9,           // Contador inicia em 9
    .button_b_clicks = 0,   // Contador de cliques inicia em 0
    .timer_running = false, // Timer inicia desativado
    .update_display = false // Flag para atualização do display
};

// Buffer para o display OLED
uint8_t display_buffer[ssd1306_buffer_length];

// Área de renderização do display
struct render_area display_area = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1};

// Tempos dos últimos acionamentos para debounce
absolute_time_t last_button_a_time = {0};
absolute_time_t last_button_b_time = {0};

void init_buttons()
{
    // Configura botão A
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    // Configura botão B
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    // Habilita interrupções com callback
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback_dispatcher);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);
}

void init_display()
{
    // Configura I2C
    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Inicializa display e mostra estado inicial
    ssd1306_init();
    update_display(&app_state);
}

void update_display(AppState *state)
{
    // Limpa o buffer do display
    memset(display_buffer, 0, sizeof(display_buffer));

    char line_buffer[16]; // Buffer para as linhas de texto

    // Desenha as strings no buffer do display
    sprintf(line_buffer, "APERTE O BOTAO A");
    ssd1306_draw_string(display_buffer, 0, 8, line_buffer);

    sprintf(line_buffer, "  PARA RESETAR  ");
    ssd1306_draw_string(display_buffer, 0, 16, line_buffer);

    sprintf(line_buffer, "    TIMER %d    ", state->counter);
    ssd1306_draw_string(display_buffer, 0, 40, line_buffer);

    sprintf(line_buffer, "   CLIQUES %d   ", state->button_b_clicks);
    ssd1306_draw_string(display_buffer, 0, 48, line_buffer);

    // Renderiza o buffer no display
    calculate_render_area_buffer_length(&display_area);
    render_on_display(display_buffer, &display_area);
}

static void button_a_callback(uint gpio, uint32_t events)
{
    absolute_time_t now = get_absolute_time();

    // Verifica debounce
    if (absolute_time_diff_us(last_button_a_time, now) > BUTTON_DEBOUNCE)
    {
        // Reseta os contadores e inicia o timer
        app_state.counter = 9;
        app_state.button_b_clicks = 0;
        app_state.timer_running = true;
        app_state.update_display = true;
        last_button_a_time = now;
        printf("Botão A pressionado: contador reiniciado.\n");
    }
}

static void button_b_callback(uint gpio, uint32_t events)
{
    absolute_time_t now = get_absolute_time();

    // Só conta cliques se o timer estiver ativo e após debounce
    if (app_state.timer_running && absolute_time_diff_us(last_button_b_time, now) > BUTTON_DEBOUNCE)
    {
        app_state.button_b_clicks++;
        app_state.update_display = true;
        last_button_b_time = now;
        printf("Botão B pressionado: total de cliques = %d\n", app_state.button_b_clicks);
    }
}

void gpio_callback_dispatcher(uint gpio, uint32_t events)
{
    if (gpio == BUTTON_A && (events & GPIO_IRQ_EDGE_FALL))
    {
        button_a_callback(gpio, events);
    }
    else if (gpio == BUTTON_B && (events & GPIO_IRQ_EDGE_FALL))
    {
        button_b_callback(gpio, events);
    }
}

bool timer_callback(repeating_timer_t *rt)
{
    if (app_state.timer_running)
    {
        if (app_state.counter > 0)
        {
            app_state.counter--;
            printf("Contador: %d\n", app_state.counter);
        }
        else
        {
            app_state.timer_running = false;
            printf("Contagem encerrada.\n");
        }
        app_state.update_display = true;
    }
    return true; // Continua o timer
}
