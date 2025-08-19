/**
 * @file ads1115_adc.c
 * @brief Wrapper: seleciona implementação REAL ou MOCK via macro local.
 *
 * Troque a linha abaixo para:
 *   #define ADS1115_USE_MOCK 1   // usa o simulador (sem hardware)
 * ou
 *   #define ADS1115_USE_MOCK 0   // usa o driver real I2C (padrão)
 */

#define ADS1115_USE_MOCK 1

#if (ADS1115_USE_MOCK == 1)
  #include "ads1115_adc_mock.c"
#elif (ADS1115_USE_MOCK == 0)
  #include "ads1115_adc.c"
#else
  #error "ADS1115_USE_MOCK deve ser 0 (real) ou 1 (mock)"
#endif
