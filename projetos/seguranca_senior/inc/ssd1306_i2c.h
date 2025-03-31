#ifndef SSD1306_I2C_H
#define SSD1306_I2C_H

/**
 * @file ssd1306_i2c.h
 * @brief Definições e estrutura para controle do display OLED SSD1306 via I2C no Raspberry Pi Pico W.
 *
 * Este arquivo contém as definições de macros, estruturas e constantes utilizadas na comunicação
 * com o display SSD1306. Ele define:
 * - Configurações do display (largura, altura, endereço I2C, clock).
 * - Comandos do controlador SSD1306.
 * - Estruturas para armazenamento de dados do display e área de renderização.
 *
 * @note Este cabeçalho complementa a implementação disponível em 'ssd1306_i2c.c'.
 *       Este arquivo está disponível em:
 *       https://github.com/BitDogLab/BitDogLab-C/blob/main/display_oled
 *
 * @author Timóteo Altoé
 * @date 2025
 */

#include <stdlib.h>

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define ssd1306_height 64 // Define a altura do display (32 pixels)
#define ssd1306_width 128 // Define a largura do display (128 pixels)

#define ssd1306_i2c_address _u(0x3C) // Define o endereço do i2c do display

#define ssd1306_i2c_clock 400 // Define o tempo do clock (pode ser aumentado)

// Comandos de configuração (endereços)
#define ssd1306_set_memory_mode _u(0x20)
#define ssd1306_set_column_address _u(0x21)
#define ssd1306_set_page_address _u(0x22)
#define ssd1306_set_horizontal_scroll _u(0x26)
#define ssd1306_set_scroll _u(0x2E)

#define ssd1306_set_display_start_line _u(0x40)

#define ssd1306_set_contrast _u(0x81)
#define ssd1306_set_charge_pump _u(0x8D)

#define ssd1306_set_segment_remap _u(0xA0)
#define ssd1306_set_entire_on _u(0xA4)
#define ssd1306_set_all_on _u(0xA5)
#define ssd1306_set_normal_display _u(0xA6)
#define ssd1306_set_inverse_display _u(0xA7)
#define ssd1306_set_mux_ratio _u(0xA8)
#define ssd1306_set_display _u(0xAE)
#define ssd1306_set_common_output_direction _u(0xC0)
#define ssd1306_set_common_output_direction_flip _u(0xC0)

#define ssd1306_set_display_offset _u(0xD3)
#define ssd1306_set_display_clock_divide_ratio _u(0xD5)
#define ssd1306_set_precharge _u(0xD9)
#define ssd1306_set_common_pin_configuration _u(0xDA)
#define ssd1306_set_vcomh_deselect_level _u(0xDB)

#define ssd1306_page_height _u(8)
#define ssd1306_n_pages (ssd1306_height / ssd1306_page_height)
#define ssd1306_buffer_length (ssd1306_n_pages * ssd1306_width)

#define ssd1306_write_mode _u(0xFE)
#define ssd1306_read_mode _u(0xFF)

struct render_area {
  uint8_t start_column;
  uint8_t end_column;
  uint8_t start_page;
  uint8_t end_page;

  int buffer_length;
};

typedef struct {
  uint8_t width, height, pages, address;
  i2c_inst_t * i2c_port;
  bool external_vcc;
  uint8_t *ram_buffer;
  size_t bufsize;
  uint8_t port_buffer[2];
} ssd1306_t;

#endif // SSD1306_I2C_H