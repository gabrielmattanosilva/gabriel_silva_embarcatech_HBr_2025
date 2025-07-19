#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "FreeRTOS.h"
#include "task.h"

#define ADC_PIN 8
#define ADC_INPUT 0
#define SAMPLE_RATE_HZ 1000
#define BUFFER_SIZE 256

uint16_t adc_buffer[BUFFER_SIZE];

volatile bool buffer_ready = false;
int dma_chan;

void dma_handler()
{
    dma_hw->ints0 = 1u << dma_chan; // Clear interrupt
    buffer_ready = true;
}

bool repeating_timer_callback(struct repeating_timer *t)
{
    static uint32_t index = 0;

    if (index < BUFFER_SIZE)
    {
        adc_run(true);
        adc_hw->cs |= ADC_CS_START_ONCE_BITS; // Start single conversion
        index++;
    }
    else
    {
        index = 0; // Reset index for next block
    }

    return true; // Keep repeating
}

void setup_timer_trigger()
{
    static struct repeating_timer timer;
    add_repeating_timer_us(-1000000 / SAMPLE_RATE_HZ, repeating_timer_callback, NULL, &timer);
}

void setup_adc_dma()
{
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_INPUT);
    adc_set_round_robin(1 << ADC_INPUT);

    adc_fifo_setup(true, true, 1, false, false);
    adc_run(false);

    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_ADC);

    dma_channel_configure(
        dma_chan, &c,
        adc_buffer,
        &adc_hw->fifo,
        BUFFER_SIZE,
        false);

    dma_channel_set_irq0_enabled(dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    dma_channel_start(dma_chan);
}

void adc_print_task(void *pvParameters)
{
    static uint32_t sample_counter = 0;

    while (true)
    {
        if (buffer_ready)
        {
            buffer_ready = false;

            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                printf("%lu, %u\n",sample_counter++, adc_buffer[i]);
            }

            dma_channel_configure(
                dma_chan, NULL,
                adc_buffer,
                &adc_hw->fifo,
                BUFFER_SIZE,
                true // Restart DMA
            );
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }

    printf("Inicializando...\n");
    setup_timer_trigger();
    setup_adc_dma();

    xTaskCreate(adc_print_task, "ADC Print", 1024, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }
}
