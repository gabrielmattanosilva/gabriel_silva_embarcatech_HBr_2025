#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <stdint.h>
#include "hardware/adc.h"
#include "hardware/dma.h"

#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)
#define SAMPLE_RATE 16000 // 16 kHz
#define RECORD_TIME 2     // segundos
#define SAMPLES (SAMPLE_RATE * RECORD_TIME)

void microphone_init(void);
void start_recording(uint16_t *buffer);
uint32_t get_sample_count(void);

#endif