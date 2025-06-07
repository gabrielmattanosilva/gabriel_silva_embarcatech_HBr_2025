#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

// Configurações do microfone
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)
#define SAMPLE_RATE 16000 // 16 kHz
#define RECORD_TIME 2     // segundos
#define SAMPLES (SAMPLE_RATE * RECORD_TIME)

// Configurações do PWM (reprodução)
#define PWM_PIN 10
#define PWM_FREQ SAMPLE_RATE            // Frequência igual à taxa de amostragem
#define PWM_WRAP (125000000 / PWM_FREQ) // Para 16kHz: 7812

// Botões
#define BTN_RECORD 5
#define BTN_PLAY 6

// LEDs
#define LED_RECORD 13
#define LED_PLAY 11

// Buffer de amostras do ADC
uint16_t audio_buffer[SAMPLES];
volatile bool recording = false;
volatile bool playing = false;
volatile uint32_t sample_pos = 0;

// Configurações do DMA
uint dma_channel;
dma_channel_config dma_cfg;

// Configurações do PWM
uint pwm_slice;

void init_hardware();
void start_recording();
void play_audio();
void gpio_callback();

int main()
{
    stdio_init_all();
    init_hardware();

    sleep_ms(1000);

    printf("Sintetizador de Áudio - Pronto\n");

    while (true)
    {
        if (recording)
        {
            start_recording();
            recording = false;
        }

        if (playing)
        {
            play_audio();
            playing = false;
        }

        sleep_ms(10);
    }
}

void init_hardware()
{
    // Configuração do ADC
    adc_gpio_init(MIC_PIN);
    adc_init();
    adc_select_input(MIC_CHANNEL);
    adc_fifo_setup(true, true, 1, false, false);
    adc_set_clkdiv(48000000.0f / SAMPLE_RATE - 1); // Configura taxa de amostragem

    // Configuração do DMA
    dma_channel = dma_claim_unused_channel(true);
    dma_cfg = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&dma_cfg, false);
    channel_config_set_write_increment(&dma_cfg, true);
    channel_config_set_dreq(&dma_cfg, DREQ_ADC);

    // Configuração do PWM
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(PWM_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, PWM_WRAP);
    pwm_config_set_clkdiv(&config, 1.f); // Sem divisão de clock
    pwm_init(pwm_slice, &config, false);

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

void start_recording()
{
    printf("Iniciando gravação...\n");
    gpio_put(LED_RECORD, 1);
    gpio_put(LED_PLAY, 0);

    adc_fifo_drain();
    adc_run(false);

    dma_channel_configure(dma_channel, &dma_cfg,
                          audio_buffer,
                          &(adc_hw->fifo),
                          SAMPLES,
                          true);

    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_channel);
    adc_run(false);

    printf("Gravação concluída!\n");
    gpio_put(LED_RECORD, 0);
}

void play_audio()
{
    printf("Reproduzindo áudio...\n");
    gpio_put(LED_RECORD, 0);
    gpio_put(LED_PLAY, 1);

    for (uint32_t i = 1; i < SAMPLES; i++)
    {
        audio_buffer[i] = audio_buffer[i] + 0.7f * (audio_buffer[i] - audio_buffer[i - 1]);
    }

    // Encontra o valor máximo no buffer para normalização
    uint16_t max_val = 0;
    for (uint32_t i = 0; i < SAMPLES; i++)
    {
        if (audio_buffer[i] > max_val)
        {
            max_val = audio_buffer[i];
        }
    }

    // Fator de amplificação (ajuste conforme necessário)
    float gain = 2.0f; // Aumenta o ganho em 2x
    if (max_val > 0)
    {
        gain = (4095.0f / max_val) * 1.5f; // Auto-ajuste com margem de 1.5x
    }

    printf("Ganho aplicado: %.2f\n", gain);

    pwm_set_enabled(pwm_slice, true);

    for (uint32_t i = 0; i < SAMPLES; i++)
    {
        // Aplica ganho e limita ao máximo de 12 bits
        uint16_t amplified = (uint16_t)(audio_buffer[i] * gain);
        if (amplified > 4095)
            amplified = 4095;

        // Converte para valor PWM (12 bits -> 16 bits, usando todo o range do PWM)
        uint16_t pwm_val = (amplified * PWM_WRAP) / 4095;
        pwm_set_chan_level(pwm_slice, PWM_CHAN_A, pwm_val);
        sleep_us(1000000 / SAMPLE_RATE);
    }

    pwm_set_enabled(pwm_slice, false);
    printf("Reprodução concluída!\n");
    gpio_put(LED_PLAY, 0);
}

void gpio_callback(uint gpio, uint32_t events)
{
    static uint32_t last_time_record = 0;
    static uint32_t last_time_play = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());

    // Debounce simples
    if (gpio == BTN_RECORD && (now - last_time_record > 200))
    {
        recording = true;
        last_time_record = now;
        printf("Botao RECORD pressionado\n");
    }
    else if (gpio == BTN_PLAY && (now - last_time_play > 200))
    {
        playing = true;
        last_time_play = now;
        printf("Botao PLAY pressionado\n");
    }
}