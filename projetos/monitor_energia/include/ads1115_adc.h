#ifndef ADS1115_ADC_H
#define ADS1115_ADC_H

#include <stdint.h>
#include <stdbool.h>

#define CONFIG_OS_SINGLE    (1 << 15)
#define CONFIG_MUX_AIN0     (0x4 << 12)
#define CONFIG_MUX_AIN1     (0x5 << 12)
#define CONFIG_PGA_4_096V   (0x1 << 9)
#define CONFIG_MODE_SINGLE  (1 << 8)
#define CONFIG_DR_860SPS    (0x7 << 5)
#define CONFIG_DEFAULT      (CONFIG_OS_SINGLE | CONFIG_PGA_4_096V | CONFIG_MODE_SINGLE | CONFIG_DR_860SPS)

void ads1115_init(void);
void ads1115_write(uint8_t reg, uint16_t value);
int16_t ads1115_read_conversion(void);
bool ads1115_conversion_ready(void);

#endif /* ADS1115_ADC_H */
