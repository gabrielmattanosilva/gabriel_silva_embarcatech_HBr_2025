#include "buttons_led.h"
#include "hardware/gpio.h"

static volatile bool recording_requested = false;
static volatile bool playing_requested = false;

void gpio_callback(uint gpio, uint32_t events)
{
    static uint32_t last_time_record = 0;
    static uint32_t last_time_play = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());

    if (gpio == BTN_RECORD && (now - last_time_record > 200))
    {
        recording_requested = true;
        last_time_record = now;
    }
    else if (gpio == BTN_PLAY && (now - last_time_play > 200))
    {
        playing_requested = true;
        last_time_play = now;
    }
}

void buttons_led_init(void)
{
    // Configuração dos botões
    gpio_init(BTN_RECORD);
    gpio_init(BTN_PLAY);
    gpio_set_dir(BTN_RECORD, GPIO_IN);
    gpio_set_dir(BTN_PLAY, GPIO_IN);
    gpio_pull_up(BTN_RECORD);
    gpio_pull_up(BTN_PLAY);

    // Configura interrupções para os botões
    gpio_set_irq_enabled_with_callback(BTN_RECORD, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BTN_PLAY, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Configuração dos LEDs
    gpio_init(LED_RECORD);
    gpio_init(LED_PLAY);
    gpio_set_dir(LED_RECORD, GPIO_OUT);
    gpio_set_dir(LED_PLAY, GPIO_OUT);
}

void set_led_record(bool state)
{
    gpio_put(LED_RECORD, state);
}

void set_led_play(bool state)
{
    gpio_put(LED_PLAY, state);
}

bool is_recording_requested(void)
{
    return recording_requested;
}

bool is_playing_requested(void)
{
    return playing_requested;
}

void clear_play_request(void)
{
    playing_requested = false;
}

void clear_record_request(void)
{
    recording_requested = false;
}