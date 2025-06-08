#include "microphone.h"

static uint dma_channel;
static dma_channel_config dma_cfg;

void microphone_init(void)
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
}

void start_recording(uint16_t *buffer)
{
    adc_fifo_drain();
    adc_run(false);

    dma_channel_configure(dma_channel, &dma_cfg,
                          buffer,
                          &(adc_hw->fifo),
                          SAMPLES,
                          true);

    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_channel);
    adc_run(false);
}

uint32_t get_sample_count(void)
{
    return SAMPLES;
}