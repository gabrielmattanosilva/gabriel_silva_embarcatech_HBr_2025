#ifndef DISPLAY_H
#define DISPLAY_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306_i2c.h"

// Configuração dos pinos I2C
#define I2C_SDA 14
#define I2C_SCL 15

// Inicializa o display OLED
void display_init();

// Atualiza o display com a forma de onda do áudio
void display_update_waveform(const uint16_t *buffer, uint32_t sample_count);

// Limpa o display
void display_clear();

#endif