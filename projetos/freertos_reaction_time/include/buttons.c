#include "buttons.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "reaction_time.h"

static QueueHandle_t button_queue = NULL;

static void button_isr(uint gpio, uint32_t events)
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    static uint32_t last_button_a_time = 0;
    static uint32_t last_button_b_time = 0;

    if (gpio == BUTTON_A_PIN && (current_time - last_button_a_time) > DEBOUNCE_TIME_MS)
    {
        last_button_a_time = current_time;
        button_event_t event = EVENT_BUTTON_A_PRESSED;
        xQueueSendFromISR(button_queue, &event, NULL);
    }
    else if (gpio == BUTTON_B_PIN && (current_time - last_button_b_time) > DEBOUNCE_TIME_MS)
    {
        last_button_b_time = current_time;
        button_event_t event = EVENT_BUTTON_B_PRESSED;
        xQueueSendFromISR(button_queue, &event, NULL);
    }
}

void buttons_init(QueueHandle_t queue)
{
    button_queue = queue;

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, button_isr);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);
}

void buttons_task(void *pvParameters)
{
    button_event_t event;
    while (true)
    {
        if (xQueueReceive(button_queue, &event, portMAX_DELAY))
        {
            process_button_event(event);
        }
    }
}