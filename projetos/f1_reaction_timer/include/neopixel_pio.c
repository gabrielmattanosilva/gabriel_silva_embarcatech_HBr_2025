/**
 * @file neopixel_pio.c
 * @brief Driver para controle de LEDs NeoPixel/WS2812B via PIO do Raspberry Pi Pico
 *
 * Implementa controle de matriz de LEDs RGB usando a interface PIO para geração
 * do protocolo de timing preciso requerido pelos LEDs WS2812B.
 */

#include "neopixel_pio.h"
#include "ws2818b.pio.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"

/**
 * @brief Estrutura que representa um pixel RGB
 */
struct pixel_t
{
    uint8_t G, R, B;
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t;

/**
 * @brief Buffer de pixels que formam a matriz de LEDs
 */
npLED_t leds[LED_COUNT];

/**
 * @brief Variáveis para controle da máquina de estados PIO
 */
PIO np_pio;
uint sm;

/**
 * @brief Inicializa a máquina PIO para controle da matriz de LEDs
 */
void npInit()
{
    /* Cria programa PIO */
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;

    /* Toma posse de uma máquina PIO */
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0)
    {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true);
    }

    /* Inicia programa na máquina PIO obtida */
    ws2818b_program_init(np_pio, sm, offset, LED_PIN, 800000.f);

    /* Limpa buffer de pixels */
    for (uint i = 0; i < LED_COUNT; ++i)
    {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}

/**
 * @brief Atribui uma cor RGB a um LED específico
 *
 * @param index Índice do LED na matriz (0 a LED_COUNT-1)
 * @param r Intensidade do componente vermelho (0-255)
 * @param g Intensidade do componente verde (0-255)
 * @param b Intensidade do componente azul (0-255)
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b)
{
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

/**
 * @brief Limpa o buffer de pixels
 */
void npClear()
{
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);
}

/**
 * @brief Escreve os dados do buffer nos LEDs
 */
void npWrite()
{
    /* Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO */
    for (uint i = 0; i < LED_COUNT; ++i)
    {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }

    /* Espera 100us, sinal de RESET do datasheet */
    sleep_us(100);
}

/**
 * @brief Controla um padrão específico de LEDs estilo semáforo F1
 *
 * Acende ou apaga pares específicos de LEDs em sequência, com um
 * delay de 1 segundo entre cada par, simulando as luzes de um
 * semáforo de Fórmula 1.
 *
 * @param state true para acender a sequência, false para apagar todos
 */
void npSetF1Lights(bool state)
{
    npClear();

    const uint8_t ledPairs[][2] = {
        {14, 15},
        {13, 16},
        {12, 17},
        {11, 18},
        {10, 19}};

    const size_t numPairs = sizeof(ledPairs) / sizeof(ledPairs[0]);

    if (state)
    {
        for (size_t i = 0; i < numPairs; i++)
        {
            npSetLED(ledPairs[i][0], 255, 0, 0);
            npSetLED(ledPairs[i][1], 255, 0, 0);
            npWrite();
            sleep_ms(1000);
        }
    }
    else
    {
        for (size_t i = 0; i < numPairs; i++)
        {
            npSetLED(ledPairs[i][0], 0, 0, 0);
            npSetLED(ledPairs[i][1], 0, 0, 0);
        }
        npWrite();
    }
}