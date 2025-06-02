#include <stdio.h>
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

int main()
{
    stdio_init_all();
    cyw43_arch_init();

    while (true) {
        printf("LED ON\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
        sleep_ms(1000);
        printf("LED OFF\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
        sleep_ms(1000);
    }
}
