#include "display.h"
#include <string.h>

// Buffer para o display
static uint8_t ssd[ssd1306_buffer_length];

// Área de renderização
static struct render_area frame_area = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1};

void display_init()
{
    // Inicialização do I2C
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display
    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
    display_clear();
}

void display_clear()
{
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
}

void display_update_waveform(const uint16_t *buffer, uint32_t sample_count)
{
    // Limpa o buffer
    memset(ssd, 0, ssd1306_buffer_length);

    // Calcula quantas amostras por pixel (para caber no display)
    uint32_t samples_per_pixel = sample_count / ssd1306_width;
    if (samples_per_pixel < 1)
        samples_per_pixel = 1;

    // Desenha a forma de onda
    for (uint32_t x = 0; x < ssd1306_width; x++)
    {
        // Calcula a média das amostras para este pixel
        uint32_t start_sample = x * samples_per_pixel;
        uint32_t end_sample = start_sample + samples_per_pixel;
        if (end_sample > sample_count)
            end_sample = sample_count;

        uint32_t sum = 0;
        for (uint32_t i = start_sample; i < end_sample; i++)
        {
            sum += buffer[i];
        }
        uint16_t avg = sum / (end_sample - start_sample);

        // Mapeia o valor médio para a altura do display (0-63)
        uint8_t height = (avg * ssd1306_height) / 4096;

        // Desenha a coluna (de baixo para cima)
        for (uint8_t y = 0; y < height && y < ssd1306_height; y++)
        {
            // Calcula a posição vertical invertida (o display tem (0,0) no canto superior esquerdo)
            uint8_t display_y = ssd1306_height - 1 - y;

            // Determina a página e o bit dentro da página
            uint8_t page = display_y / 8;
            uint8_t bit = display_y % 8;

            // Ativa o bit correspondente
            ssd[x + page * ssd1306_width] |= (1 << bit);
        }
    }

    // Atualiza o display
    render_on_display(ssd, &frame_area);
}