#ifndef HAL_LED_H
#define HAL_LED_H

#include <stdbool.h>

void hal_led_init(void);
void hal_led_toggle(bool estado);

#endif