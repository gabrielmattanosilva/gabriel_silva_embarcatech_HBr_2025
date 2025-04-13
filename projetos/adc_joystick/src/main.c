#include <stdio.h>
#include "hardware/adc.h"
#include "pico/stdlib.h"

const int X = 26;
const int Y = 27;
const int ADC_CHANNEL_0 = 0;
const int ADC_CHANNEL_1 = 1;

void setup_joystick()
{
    adc_init();
    adc_gpio_init(X);
    adc_gpio_init(Y);
}

void setup()
{
    stdio_init_all();
    setup_joystick();
}

void joystick_read_axis(uint16_t *x_value, uint16_t *y_value)
{
    adc_select_input(ADC_CHANNEL_0);
    sleep_us(2);
    *x_value = adc_read();

    adc_select_input(ADC_CHANNEL_1);
    sleep_us(2);
    *y_value = adc_read();
}

int main()
{
    uint16_t x_value, y_value;
    setup();
    while (1)
    {
        joystick_read_axis(&x_value, &y_value);
        printf("X: %d | Y: %d\n", x_value, y_value);
        sleep_ms(100);
    }
}