#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

/**
 * @file button_handler.h
 * @brief Biblioteca para manipulação de botões via GPIO.
 *
 * Esta biblioteca contém as definições de pinos e as declarações de funções
 * para lidar com a leitura dos pinos dos botões.
 * 
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "callmebot_whatsapp.h"
#include "buzzer_led.h"
#include "display_oled.h"
#include "display_text.h"

// Definição dos pinos
/*      Botão do            Pino do
        controle    GPIO    receptor*/
#define BUTTON_A    20    // D3
#define BUTTON_B    4     // D2
#define BUTTON_C    9     // D1
#define BUTTON_D    8     // D0 

// Declaração das funções
void button_handler_init();
bool button_check_callback(struct repeating_timer *t);

#endif // BUTTON_HANDLER_H