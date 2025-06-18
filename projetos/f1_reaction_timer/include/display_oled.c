/**
 * @file display_oled.c
 * @brief Implementação para controle do display OLED SSD1306 via I2C.
 *
 * Esta implementação define funções para inicializar, limpar e exibir
 * texto no display OLED SSD1306 utilizando comunicasção I2C.
 *
 * @note Esta implementação foi baseada no projeto disponível em:
 *       https://github.com/BitDogLab/BitDogLab-C/blob/main/display_oled
 */

#include "display_oled.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "ssd1306_font.h"
#include "ssd1306_i2c.h"

static uint8_t front_buffer[ssd1306_buffer_length];
static uint8_t back_buffer[ssd1306_buffer_length];
static bool buffer_changed = false;
static bool display_initialized = false;

/**
 * @brief Área de renderização para o quadro completo do display
 */
static struct render_area full_frame = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1,
    .buffer_length = ssd1306_buffer_length};

/**
 * @brief Inicializa o display OLED e a interface I2C
 */
void display_init(void)
{
    if (display_initialized)
        return;

    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();
    memset(front_buffer, 0, ssd1306_buffer_length);
    memset(back_buffer, 0, ssd1306_buffer_length);
    render_on_display(front_buffer, &full_frame);
    display_initialized = true;
}

/**
 * @brief Limpa o buffer traseiro do display
 */
void display_clear(void)
{
    memset(back_buffer, 0, ssd1306_buffer_length);
    buffer_changed = true;
}

/**
 * @brief Desenha texto no buffer traseiro do display
 * @param x Posição horizontal inicial do texto
 * @param y Posição vertical inicial do texto (em páginas de 8 pixels)
 * @param text Texto a ser desenhado
 */
void display_draw_text(int x, int y, const char *text)
{
    ssd1306_draw_string(back_buffer, x, y, text);
    buffer_changed = true;
}

/**
 * @brief Atualiza o display com o conteúdo do buffer traseiro
 * @param force_full Força atualização completa mesmo sem alterações
 */
void display_update(bool force_full)
{
    if (!buffer_changed && !force_full)
        return;

    if (force_full || memcmp(front_buffer, back_buffer, ssd1306_buffer_length) != 0)
    {
        render_on_display(back_buffer, &full_frame);
        memcpy(front_buffer, back_buffer, ssd1306_buffer_length);
    }

    buffer_changed = false;
}

/**
 * @brief Exibe mensagem de estado ocioso no display
 */
void display_show_idle_message(void)
{
    display_clear();
    display_draw_text(10, 10, "TESTE DE TEMPO");
    display_draw_text(10, 20, "DE REACAO - F1");
    display_draw_text(10, 40, "APERTE O BOTAO");
    display_draw_text(10, 50, "A PARA COMECAR");
    display_update(true);
}

/**
 * @brief Exibe mensagem de espera no display
 */
void display_show_waiting_message(void)
{
    display_clear();
    display_draw_text(10, 30, "PREPARE-SE");
    display_update(true);
}

/**
 * @brief Exibe o tempo de reação medido no display
 * @param reaction_time Tempo de reação em milissegundos
 */
void display_show_reaction_time(uint32_t reaction_time)
{
    char time_text[20];
    snprintf(time_text, sizeof(time_text), "TEMPO %d MS", reaction_time);
    display_clear();
    display_draw_text(10, 10, time_text);
    display_draw_text(10, 30, "APERTE O BOTAO");
    display_draw_text(10, 40, "A PARA REPETIR");
    display_update(true);
}