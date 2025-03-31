/**
 * @file buzzer_led.c
 * @brief Implementação para controle de buzzer e LED via GPIO/PWM.
 *
 * Esta implementação define funções para inicializar, ativar e desativar
 * buzzers e LEDs utilizando GPIO e PWM.
 * 
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include "buzzer_led.h"

/**
 * @brief Inicializa um pino como saída PWM para o buzzer.
 * @param pin Pino GPIO utilizado para o buzzer.
 */
void buzzer_init(uint8_t pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0);
}

/**
 * @brief Gera um tom no buzzer com a frequência especificada.
 * @param pin Pino GPIO do buzzer.
 * @param frequency Frequência do som em Hz.
 */
void tone(uint8_t pin, uint16_t frequency) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_div = clock_get_hz(clk_sys) / (frequency * 4096);
    pwm_set_clkdiv(slice_num, clock_div);
    pwm_set_gpio_level(pin, 2048);  // Define o nível PWM para 50% do ciclo
}

/**
 * @brief Desativa o tom do buzzer.
 * @param pin Pino GPIO do buzzer.
 */
void no_tone(uint8_t pin) {
    pwm_set_gpio_level(pin, 0);
}

/**
 * @brief Liga o buzzer e acende o LED correspondente.
 * @param led Pino GPIO do LED.
 * @param frequency Frequência do buzzer.
 */
void buzzer_led_on(uint8_t led, uint16_t frequency) {
    tone(BUZZER1_PIN, frequency);
    tone(BUZZER2_PIN, frequency);
    gpio_put(led, 1);
}

/**
 * @brief Desliga o buzzer e apaga o LED correspondente.
 * @param led Pino GPIO do LED.
 */
void buzzer_led_off(uint8_t led) {
    no_tone(BUZZER1_PIN);
    no_tone(BUZZER2_PIN);
    gpio_put(led, 0);
}

/**
 * @brief Inicializa os LEDs e os buzzers.
 */
void buzzer_led_init() {
    gpio_init(LED_GREEN);
    gpio_init(LED_RED);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);
    buzzer_init(BUZZER1_PIN);
    buzzer_init(BUZZER2_PIN);
}

/**
 * @brief Indica sucesso piscando o LED verde e emitindo um som curto.
 */
void buzzer_led_init_success() {
    buzzer_led_on(LED_GREEN, 5000);
    busy_wait_ms(150);
    buzzer_led_off(LED_GREEN);
}

/**
 * @brief Indica falha piscando o LED vermelho e emitindo um som longo.
 */
void buzzer_led_fail() {
    buzzer_led_on(LED_RED, 2500);
    busy_wait_ms(500);
    buzzer_led_off(LED_RED);
}

/**
 * @brief Mensagem sonora e visual número 1: Dois sinais curtos.
 */
void buzzer_led_msg_1() {
    for (int i = 0; i < 2; i++) {
        buzzer_led_on(LED_GREEN, 5000);
        busy_wait_ms(250);
        buzzer_led_off(LED_GREEN);
        busy_wait_ms(50);
    }
}

/**
 * @brief Mensagem sonora e visual número 2: Três sinais alternando frequência.
 */
void buzzer_led_msg_2() {
    for (int i = 0; i < 3; i++) {
        buzzer_led_on(LED_GREEN, (i % 2 == 0) ? 2500 : 5000);
        busy_wait_ms(250);
        buzzer_led_off(LED_GREEN);
        busy_wait_ms(50);
    }
}

/**
 * @brief Mensagem sonora e visual número 3: Seis sinais alternando frequência.
 */
void buzzer_led_msg_3() {
    for (int i = 0; i < 6; i++) {
        buzzer_led_on(LED_GREEN, (i % 2 == 0) ? 2500 : 7500);
        busy_wait_ms(250);
        buzzer_led_off(LED_GREEN);
        busy_wait_ms(50);
    }
}

/**
 * @brief Mensagem sonora e visual número 4: Doze sinais alternando frequência.
 */
void buzzer_led_msg_4() {
    for (int i = 0; i < 12; i++) {
        buzzer_led_on(LED_GREEN, (i % 2 == 0) ? 5000 : 10000);
        busy_wait_ms(250);
        buzzer_led_off(LED_GREEN);
        busy_wait_ms(50);
    }
}
