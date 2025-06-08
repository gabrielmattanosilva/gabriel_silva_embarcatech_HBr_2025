#include <stdio.h>
#include "pico/stdlib.h"
#include "buttons_led.h"
#include "microphone.h"
#include "buzzer.h"
#include "display.h"

// Buffer de amostras do ADC
static uint16_t audio_buffer[SAMPLES];

int main()
{
    stdio_init_all();
    buttons_led_init();
    microphone_init();
    buzzer_init();
    display_init(); // Inicializa o display

    sleep_ms(1000);
    printf("Sintetizador de Áudio - Pronto\n");

    while (true)
    {
        if (is_recording_requested())
        {
            printf("Iniciando gravação...\n");
            set_led_record(true);
            set_led_play(false);

            start_recording(audio_buffer);

            // Atualiza o display com a forma de onda gravada
            display_update_waveform(audio_buffer, get_sample_count());

            printf("Gravação concluída!\n");
            set_led_record(false);
            clear_record_request();
        }

        if (is_playing_requested())
        {
            printf("Reproduzindo áudio...\n");
            set_led_record(false);
            set_led_play(true);

            process_audio_buffer(audio_buffer, get_sample_count());

            // Atualiza o display antes de começar a reprodução
            display_update_waveform(audio_buffer, get_sample_count());

            buzzer_start();
            for (uint32_t i = 0; i < get_sample_count(); i++)
            {
                buzzer_play_sample(audio_buffer[i]);
                sleep_us(1000000 / SAMPLE_RATE);

                // Atualiza o display em tempo real durante a reprodução (a cada 128 amostras)
                if (i % (get_sample_count() / ssd1306_width) == 0)
                {
                    display_update_waveform(&audio_buffer[i], get_sample_count() - i);
                }
            }
            buzzer_stop();

            printf("Reprodução concluída!\n");
            set_led_play(false);
            clear_play_request();
        }

        sleep_ms(10);
    }
}