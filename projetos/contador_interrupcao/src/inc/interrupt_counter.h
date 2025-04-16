#ifndef INTERRUPT_COUNTER_H
#define INTERRUPT_COUNTER_H

#include "pico/stdlib.h"
#include "ssd1306.h"

// Definições de pinos e constantes
#define BUTTON_A 5                   // Pino do botão A
#define BUTTON_B 6                   // Pino do botão B
#define SDA_PIN 14                   // Pino I2C SDA
#define SCL_PIN 15                   // Pino I2C SCL
#define I2C_PORT i2c1                // Porta I2C utilizada
#define BUTTON_DEBOUNCE (200 * 1000) // Tempo de debounce 200ms em microssegundos

/**
 * @brief Estrutura que armazena o estado da aplicação
 *
 * Utiliza volatile para garantir acesso seguro entre main loop e interrupções
 */
typedef struct
{
    volatile int counter;         // Contador decrescente
    volatile int button_b_clicks; // Contador de cliques no botão B
    volatile bool timer_running;  // Estado do timer
    volatile bool update_display; // Flag para atualização do display
} AppState;

// Declarações de variáveis globais
extern AppState app_state;
extern uint8_t display_buffer[ssd1306_buffer_length];
extern struct render_area display_area;
extern absolute_time_t last_button_a_time;
extern absolute_time_t last_button_b_time;

/**
 * @brief Inicializa os botões com interrupções
 */
void init_buttons(void);

/**
 * @brief Inicializa o display OLED
 */
void init_display(void);

/**
 * @brief Atualiza o conteúdo exibido no display OLED
 *
 * @param state Ponteiro para o estado atual da aplicação
 */
void update_display(AppState *state);

/**
 * @brief Callback para o botão A (reset)
 *
 * @param gpio Número do pino GPIO
 * @param events Eventos que triggeraram o callback
 */
static void button_a_callback(uint gpio, uint32_t events);

/**
 * @brief Callback para o botão B (contagem de cliques)
 *
 * @param gpio Número do pino GPIO
 * @param events Eventos que triggeraram o callback
 */
static void button_b_callback(uint gpio, uint32_t events);

/**
 * @brief Dispatcher de interrupções GPIO
 *
 * @param gpio Número do pino GPIO
 * @param events Eventos que triggeraram a interrupção
 */
void gpio_callback_dispatcher(uint gpio, uint32_t events);

/**
 * @brief Callback do timer periódico
 *
 * @param rt Ponteiro para a estrutura do timer
 * @return true Para continuar o timer
 * @return false Para parar o timer
 */
bool timer_callback(repeating_timer_t *rt);

#endif // INTERRUPT_COUNTER_H