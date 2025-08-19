/**
 * @file ads1115_adc_mock.c
 * @brief Implementação MOCK do ADS1115 (simulador por software).
 *
 * Mantém a mesma API da lib real (ads1115_adc.h) para ser intercambiável.
 * Gera senoides de 60 Hz com offsets DC e ruído leve, e respeita um tempo
 * de conversão ~1160 us (860 SPS).
 */

#include "lib/ads1115_adc.h"
#include <math.h>
#include "pico/time.h"

#define M_PI 3.14159265358979323846 /**< Constante PI para cálculos trigonométricos. */

#define F_LINE_HZ   60.0f                                   /**< Frequência da senoide simulada (Hz). */
#define DR_SPS      860.0f                                  /**< Taxa de amostragem simulada (Samples per second). */
#define T_CONV_US   (uint64_t)(1000000.0 / DR_SPS + 0.5)    /**< Tempo de conversão simulado em microssegundos. */
#define LSB_4_096V  (4.096f / 32768.0f)                     /**< Resolução (LSB) para faixa ±4.096 V. */

#define VOLT_DC_OFFSET      1.50f   /**< Offset DC aplicado ao canal de tensão (V). */
#define CURR_DC_OFFSET      1.65f   /**< Offset DC aplicado ao canal de corrente (V). */
#define VOLT_CONV_FACTOR    301.15f /**< Fator de conversão de tensão (Vadc -> Vreal). */
#define CURR_CONV_FACTOR    54.87f  /**< Fator de conversão de corrente (Vadc -> Ireal). */

#define MOCK_TARGET_VRMS 127.0f /**< Valor RMS alvo da tensão simulada (V). */
#define MOCK_TARGET_IRMS 5.0f   /**< Valor RMS alvo da corrente simulada (A). */

/** @brief Timestamp da última conversão simulada (us desde boot). */
static uint64_t s_last_conv_start_us = 0;

/** @brief Código do último canal configurado (MUX). */
static uint8_t s_last_mux_code = 0x4;

/** @brief Semente para gerador pseudoaleatório interno (ruído). */
static uint32_t s_seed = 0xABCDEF01;

/**
 * @brief Gera número pseudoaleatório (LCG).
 * @return Valor inteiro de 32 bits pseudoaleatório.
 */
static inline uint32_t lcg(void)
{
    s_seed = 1664525u * s_seed + 1013904223u;
    return s_seed;
}

/**
 * @brief Obtém tempo atual em segundos desde o boot.
 * @return Tempo em segundos (float).
 */
static inline float now_s(void)
{
    return (float)(time_us_64() / 1000000.0);
}

/**
 * @brief Converte tensão analógica em código de 16 bits (ADS1115).
 * @param v Tensão em Volts.
 * @return Código assinado de 16 bits proporcional à tensão.
 */
static inline int16_t volts_to_code(float v)
{
    if (v > 4.096f)
    {
        v = 4.096f;
    }

    if (v < -4.096f)
    {
        v = -4.096f;
    }

    float code = v / LSB_4_096V;

    if (code > 32767.0f)
    {
        code = 32767.0f;
    }

    if (code < -32768.0f)
    {
        code = -32768.0f;
    }

    return (int16_t)lrintf(code);
}

/**
 * @brief Gera amostra simulada para canal de tensão (AIN0).
 * @return Código de 16 bits simulando leitura do ADS1115.
 */
static int16_t gen_sample_ch0(void)
{
    const float rms_adc = (MOCK_TARGET_VRMS / VOLT_CONV_FACTOR);
    const float amp_adc = rms_adc * 1.41421356f;
    const float t = now_s();
    const float noise = ((int32_t)(lcg() & 0xFFFF) - 32768) / 32768.0f * 0.003f;
    const float v = VOLT_DC_OFFSET + amp_adc * sinf(2.0f * (float)M_PI * F_LINE_HZ * t) + noise;
    return volts_to_code(v);
}

/**
 * @brief Gera amostra simulada para canal de corrente (AIN1).
 * @return Código de 16 bits simulando leitura do ADS1115.
 */
static int16_t gen_sample_ch1(void)
{
    const float rms_adc = (MOCK_TARGET_IRMS / CURR_CONV_FACTOR);
    const float amp_adc = rms_adc * 1.41421356f;
    const float t = now_s();
    const float noise = ((int32_t)(lcg() & 0xFFFF) - 32768) / 32768.0f * 0.006f; /* ±6 mV */
    const float v = CURR_DC_OFFSET + amp_adc * sinf(2.0f * (float)M_PI * F_LINE_HZ * t) + noise;
    return volts_to_code(v);
}

/**
 * @brief Inicializa o mock do ADS1115.
 * @note Apenas registra o timestamp inicial.
 */
void ads1115_init(void)
{
    s_last_conv_start_us = time_us_64();
}

/**
 * @brief Escreve valor em registrador (mock).
 * @param reg Endereço do registrador.
 * @param value Valor de 16 bits escrito.
 * @note No mock, apenas atualiza o MUX e reinicia conversão simulada.
 */
void ads1115_write(uint8_t reg, uint16_t value)
{
    if (reg == 0x01)
    {
        s_last_mux_code = (uint8_t)((value >> 12) & 0x7);
        s_last_conv_start_us = time_us_64();
    }
}

/**
 * @brief Lê valor de conversão do mock.
 * @return Amostra de 16 bits simulada do canal configurado.
 */
int16_t ads1115_read_conversion(void)
{
    switch (s_last_mux_code)
    {
    case 0x4:
        return gen_sample_ch0();
    case 0x5:
        return gen_sample_ch1();
    default:
        return 0;
    }
}

/**
 * @brief Verifica se conversão simulada está pronta.
 * @return true se tempo de conversão decorrido; false caso contrário.
 */
bool ads1115_conversion_ready(void)
{
    return (time_us_64() - s_last_conv_start_us) >= T_CONV_US;
}
