#include "buzzer.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

static uint pwm_slice;

void buzzer_init(void)
{
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(PWM_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, PWM_WRAP);
    pwm_config_set_clkdiv(&config, 1.f); // Sem divisão de clock
    pwm_init(pwm_slice, &config, false);
}

void buzzer_start(void)
{
    pwm_set_enabled(pwm_slice, true);
}

void buzzer_stop(void)
{
    pwm_set_enabled(pwm_slice, false);
}

void buzzer_play_sample(uint16_t sample)
{
    uint16_t pwm_val = (sample * PWM_WRAP) / 4095;
    pwm_set_chan_level(pwm_slice, PWM_CHAN_A, pwm_val);
}

void process_audio_buffer(uint16_t *buffer, uint32_t sample_count)
{
    // Aplica filtro simples de realce
    for (uint32_t i = 1; i < sample_count; i++)
    {
        buffer[i] = buffer[i] + 0.7f * (buffer[i] - buffer[i - 1]);
    }

    // Normalização
    uint16_t max_val = 0;
    for (uint32_t i = 0; i < sample_count; i++)
    {
        if (buffer[i] > max_val)
            max_val = buffer[i];
    }

    float gain = 2.0f;
    if (max_val > 0)
    {
        gain = (4095.0f / max_val) * 1.5f;
    }

    for (uint32_t i = 0; i < sample_count; i++)
    {
        uint16_t amplified = (uint16_t)(buffer[i] * gain);
        if (amplified > 4095)
            amplified = 4095;
        buffer[i] = amplified;
    }
}