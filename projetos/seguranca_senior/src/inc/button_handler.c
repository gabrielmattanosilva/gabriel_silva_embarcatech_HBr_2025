/**
 * @file button_handler.c
 * @brief Implementação da biblioteca para manipulação de botões via GPIO.
 *
 * Esta implementação contém as funções responsáveis pela inicialização e
 * leitura do estado dos pinos GPIO do Raspberry Pi Pico nos quais o receptor 
 * do controle RF está conectado. Além disso, realiza o envio de mensagens via
 * WhatsApp ao detectar eventos de pressão nos botões.
 *
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include "button_handler.h"
#include "credentials.h"

// Variáveis globais para controle de envio de mensagens
volatile bool msg_1_sent = false;
volatile bool msg_2_sent = false;
volatile bool msg_3_sent = false;
volatile bool msg_4_sent = false;

/**
 * @brief Inicializa os pinos dos botões
 */
void button_handler_init()
{
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_init(BUTTON_C);
    gpio_init(BUTTON_D);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_set_dir(BUTTON_C, GPIO_IN);
    gpio_set_dir(BUTTON_D, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);
    gpio_pull_up(BUTTON_C);
    gpio_pull_up(BUTTON_D);
}

/**
 * @brief Callback do temporizador para checagem do estado dos botões
 * 
 * Essa função verifica periodicamente o estado dos pinos nos quais o 
 * receptor RF está conectado, aplicando um debounce e enviando uma 
 * mensagem via WhatsApp quando um botão é pressionado no controle.
 * 
 * @param t Ponteiro para a estrutura do temporizador.
 * @return true Para manter o temporizador ativo.
 */
bool button_check_callback(struct repeating_timer *t)
{
    static absolute_time_t last_press_time_A;
    static absolute_time_t last_press_time_B;
    static absolute_time_t last_press_time_C;
    static absolute_time_t last_press_time_D;
    static bool button_last_state_A = false;
    static bool button_last_state_B = false;
    static bool button_last_state_C = false;
    static bool button_last_state_D = false;

    // Verifica o pino correspondente ao botão A
    bool button_pressed_A = gpio_get(BUTTON_A);
    if (is_nil_time(last_press_time_A))
    {
        last_press_time_A = get_absolute_time();
    }

    if (button_pressed_A && !button_last_state_A &&
        absolute_time_diff_us(last_press_time_A, get_absolute_time()) > 200000)
    {
        last_press_time_A = get_absolute_time();
        button_last_state_A = true;

        printf("Botão A pressionado! Enviando mensagem...\n");
        if (send_whatsapp_message(MESSAGE_4, PHONE_NUMBER, API_KEY))
        {
            printf("Mensagem 4 enviada com sucesso!\n");
            msg_4_sent = true;
            display_text(msg_4_success, 3);
            buzzer_led_msg_4();
        }
        else
        {
            printf("Falha ao enviar mensagem 4!\n");
            display_text(msg_4_fail, 3);
            buzzer_led_fail();
        }
    }
    else if (!button_pressed_A)
    {
        button_last_state_A = false;
    }

    // Verifica o pino correspondente ao botão B
    bool button_pressed_B = gpio_get(BUTTON_B);
    if (is_nil_time(last_press_time_B))
    {
        last_press_time_B = get_absolute_time();
    }

    if (button_pressed_B && !button_last_state_B &&
        absolute_time_diff_us(last_press_time_B, get_absolute_time()) > 200000)
    {
        last_press_time_B = get_absolute_time();
        button_last_state_B = true;

        printf("Botão B pressionado! Enviando mensagem...\n");
        if (send_whatsapp_message(MESSAGE_3, PHONE_NUMBER, API_KEY))
        {
            printf("Mensagem 3 enviada com sucesso!\n");
            msg_3_sent = true;
            display_text(msg_3_success, 3);
            buzzer_led_msg_3();
        }
        else
        {
            printf("Falha ao enviar mensagem 3.\n");
            display_text(msg_3_fail, 3);
            buzzer_led_fail();
        }
    }
    else if (!button_pressed_B)
    {
        button_last_state_B = false;
    }

    // Verifica o pino correspondente ao botão C
    bool button_pressed_C = gpio_get(BUTTON_C);
    if (is_nil_time(last_press_time_C))
    {
        last_press_time_C = get_absolute_time();
    }

    if (button_pressed_C && !button_last_state_C &&
        absolute_time_diff_us(last_press_time_C, get_absolute_time()) > 200000)
    {
        last_press_time_C = get_absolute_time();
        button_last_state_C = true;

        printf("Botão C pressionado! Enviando mensagem...\n");
        if (send_whatsapp_message(MESSAGE_2, PHONE_NUMBER, API_KEY))
        {
            printf("Mensagem 2 enviada com sucesso!\n");
            msg_2_sent = true;
            display_text(msg_2_success, 3);
            buzzer_led_msg_2();
        }
        else
        {
            printf("Falha ao enviar mensagem 2.\n");
            display_text(msg_2_fail, 3);
            buzzer_led_fail();
        }
    }
    else if (!button_pressed_C)
    {
        button_last_state_C = false;
    }

    // Verifica o pino correspondente ao botão D
    bool button_pressed_D = gpio_get(BUTTON_D);
    if (is_nil_time(last_press_time_D))
    {
        last_press_time_D = get_absolute_time();
    }

    if (button_pressed_D && !button_last_state_D &&
        absolute_time_diff_us(last_press_time_D, get_absolute_time()) > 200000)
    {
        last_press_time_D = get_absolute_time();
        button_last_state_D = true;

        printf("Botão D pressionado! Enviando mensagem...\n");
        if (send_whatsapp_message(MESSAGE_1, PHONE_NUMBER, API_KEY))
        {
            printf("Mensagem 1 enviada com sucesso!\n");
            msg_1_sent = true;
            display_text(msg_1_success, 3);
            buzzer_led_msg_1();
        }
        else
        {
            printf("Falha ao enviar mensagem 1.\n");
            display_text(msg_1_fail, 3);
            buzzer_led_fail();
        }
    }
    else if (!button_pressed_D)
    {
        button_last_state_D = false;
    }

    return true;
}