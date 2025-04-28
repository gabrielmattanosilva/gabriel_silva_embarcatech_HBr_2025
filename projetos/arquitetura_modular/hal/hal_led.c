#include "hal_led.h"
#include "../include/led_embutido.h"

void hal_led_init(void) {
    led_embutido_init();
}

void hal_led_toggle(bool estado) {
    led_embutido_put(estado);
}