/**
 * @file display_oled.c
 * @brief Implementação para controle do display OLED SSD1306 via I2C.
 *
 * Esta implementação define funções para inicializar, limpar e exibir
 * texto no display OLED SSD1306 utilizando comunicação I2C.
 *
 * @note Esta implementação foi baseada no projeto disponível em:
 *       https://github.com/BitDogLab/BitDogLab-C/blob/main/display_oled
 * 
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "display_oled.h"
#include "display_text.h"
#include "ssd1306.h"

// Variável para verificar se o display já foi inicializado
static bool display_initialized = false;

void display_init(void) {
    if (display_initialized) return; // Evitar inicialização repetida

    // Inicialização do I2C
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicialização do OLED SSD1306
    ssd1306_init();

    // Limpa o display
    display_clear();

    display_initialized = true; // Marcar como inicializado

    // Exibe mensagem inicial
    display_text(init, 3);
    sleep_ms(2000);
}

void display_clear(void) {
    if (!display_initialized) return; // Verificar se o display foi inicializado

    // Definir a área de renderização
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // Zerar o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
}

void display_text(const char *text[], int y) {
    if (!display_initialized) return; // Verificar se o display foi inicializado

    // Definir a área de renderização
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // Zerar o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    // Renderizar o texto linha por linha
    for (int i = 0; text[i] != NULL; i++) { // Itera até encontrar NULL
        ssd1306_draw_string(ssd, 0, y * 8, text[i]);
        y += 1; // Avançar para a próxima linha
    }

    // Atualizar o display com o novo conteúdo
    render_on_display(ssd, &frame_area);
}
