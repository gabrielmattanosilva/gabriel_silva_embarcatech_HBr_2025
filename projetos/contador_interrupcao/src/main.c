#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "inc/ssd1306.h"
#include <string.h>

#define BUTTON_A 5
#define BUTTON_B 6

#define I2C_PORT i2c1
#define SDA_PIN 14
#define SCL_PIN 15

#define DEBOUNCE_TIME_MS 300 * 1000 // Transforma ms para us

volatile int counter = 9;
volatile int button_b_clicks = 0;
volatile bool active = false;
volatile bool update_display = false;

uint8_t oled_buffer[ssd1306_buffer_length];
struct render_area oled_area = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1
};

absolute_time_t last_button_a_time = { 0 };
absolute_time_t last_button_b_time = { 0 };

void update_oled() {
    memset(oled_buffer, 0, sizeof(oled_buffer));
    char msg[40];
    sprintf(msg, "Counter: %d", counter);
    ssd1306_draw_string(oled_buffer, 5, 10, msg);
    sprintf(msg, "Clicks B: %d", button_b_clicks);
    ssd1306_draw_string(oled_buffer, 5, 30, msg);
    sprintf(msg, "restart A");
    ssd1306_draw_string(oled_buffer, 5, 50, msg);
    calculate_render_area_buffer_length(&oled_area);
    render_on_display(oled_buffer, &oled_area);
}

// Função separada para tratar o botão A
void button_a_callback(uint gpio, uint32_t events) {
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_button_a_time, now) > DEBOUNCE_TIME_MS) {
        counter = 9;
        button_b_clicks = 0;
        active = true;
        update_display = true;
        last_button_a_time = now;

        printf("[EVENT] Botão A pressionado: contador reiniciado para 9.\n");
    }
}

// Função separada para tratar o botão B
void button_b_callback(uint gpio, uint32_t events) {
    absolute_time_t now = get_absolute_time();
    if (active && absolute_time_diff_us(last_button_b_time, now) > DEBOUNCE_TIME_MS) {
        button_b_clicks++;
        update_display = true;
        last_button_b_time = now;

        printf("[EVENT] Botão B pressionado: total de cliques = %d\n", button_b_clicks);
    }
}

// Dispatcher geral para GPIO — único callback registrado no SDK
void gpio_callback_dispatcher(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A && (events & GPIO_IRQ_EDGE_FALL)) {
        button_a_callback(gpio, events);
    } else if (gpio == BUTTON_B && (events & GPIO_IRQ_EDGE_FALL)) {
        button_b_callback(gpio, events);
    }
}

bool timer_callback(repeating_timer_t* rt) {
    if (active) {
        if (counter > 0) {
            counter--;
            printf("[TIMER] Contador = %d\n", counter);
        } else {
            active = false;
            printf("[END] Contagem encerrada.\n");
        }
        update_display = true;
    }
    return true;
}

int main() {
    stdio_init_all();
    sleep_ms(500);  // Delay para garantir que o terminal USB esteja pronto

    printf("=== Sistema Iniciado: Timer com Interrupção ===\n");

    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    // REGISTRA UM ÚNICO CALLBACK GLOBAL para GPIOs
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback_dispatcher);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    ssd1306_init();
    update_oled();

    repeating_timer_t timer;
    add_repeating_timer_ms(1000, timer_callback, NULL, &timer);

    while (true) {
        if (update_display) {
            update_oled();
            update_display = false;
        }
        sleep_ms(50);
    }

    return 0;
}
