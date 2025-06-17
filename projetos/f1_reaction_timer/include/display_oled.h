/**
 * @file display_oled.h
 * @brief Biblioteca para controle do display OLED SSD1306 via I2C.
 *
 * Esta biblioteca fornece funções para inicializar, limpar e exibir
 * texto no display OLED SSD1306 utilizando comunicação I2C.
 *
 * @note Esta biblioteca foi baseada no projeto disponível em:
 *       https://github.com/BitDogLab/BitDogLab-C/blob/main/display_oled
 */

#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include <stdint.h>
#include <stdbool.h>

#define I2C_SDA 14
#define I2C_SCL 15

void display_init(void);
void display_clear(void);
void display_draw_text(int x, int y, const char *text);
void display_update(bool force_full);
void display_show_idle_message(void);
void display_show_waiting_message(void);
void display_show_reaction_time(uint32_t reaction_time);

#endif // DISPLAY_OLED_H