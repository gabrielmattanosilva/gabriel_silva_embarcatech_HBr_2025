#ifndef WIFI_H
#define WIFI_H

/**
 * @file wifi.h
 * @brief Biblioteca para conexão e configuração Wi-Fi.
 *
 * Esta biblioteca contém as definições de pinos, constantes e as declarações de funções para 
 * gerenciar a conexão com redes Wi-Fi utilizando o módulo CYW43 do Raspberry Pi Pico W.
 * 
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include <stdlib.h>

#include "pico/cyw43_arch.h"
#include "buzzer_led.h"
#include "display_oled.h"
#include "display_text.h"

int wifi_init();

#endif // WIFI_H