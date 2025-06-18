/**
 * @file neopixel_pio.h
 * @brief Cabeçalho para controle de LEDs NeoPixel/WS2812B via PIO
 *
 * Declarações de interface para o driver que controla matriz de LEDs RGB
 * usando a interface PIO do Raspberry Pi Pico, com suporte a padrões especiais
 * como luzes de semáforo de Fórmula 1.
 */

#ifndef NEOPIXEL_PIO_H
#define NEOPIXEL_PIO_H

#include <stdbool.h>

#define LED_PIN 7
#define LED_COUNT 25

void npInit(void);
void npSetF1Lights(bool state);

#endif // NEOPIXEL_PIO_H