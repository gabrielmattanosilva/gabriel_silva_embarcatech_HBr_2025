#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include "hardware/pwm.h"

#define PWM_PIN 10
#define PWM_FREQ 16000                  // Frequência igual à taxa de amostragem
#define PWM_WRAP (125000000 / PWM_FREQ) // Para 16kHz: 7812

void buzzer_init(void);
void buzzer_start(void);
void buzzer_stop(void);
void buzzer_play_sample(uint16_t sample);
void process_audio_buffer(uint16_t *buffer, uint32_t sample_count);

#endif