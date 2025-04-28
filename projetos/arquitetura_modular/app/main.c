#include "hal_led.h"
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    hal_led_init();

    while (true) {
        hal_led_toggle(true);
        sleep_ms(500);
        hal_led_toggle(false);
        sleep_ms(500);
    }
}