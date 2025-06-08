#ifndef BUTTONS_LED_H
#define BUTTONS_LED_H

#include "pico/stdlib.h"

// Definições dos pinos
#define BTN_RECORD 5
#define BTN_PLAY 6
#define LED_RECORD 13
#define LED_PLAY 11

// Protótipos das funções
void buttons_led_init(void);
void set_led_record(bool state);
void set_led_play(bool state);
bool is_recording_requested(void);
bool is_playing_requested(void);
void clear_play_request(void);
void clear_record_request(void);

#endif