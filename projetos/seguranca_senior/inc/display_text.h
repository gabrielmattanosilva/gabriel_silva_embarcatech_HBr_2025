#ifndef DISPLAY_TEXT_H
#define DISPLAY_TEXT_H

/**
 * @file display_text.h
 * @brief Definições de mensagens para exibição no display.
 *
 * Este arquivo contém mensagens que serão exibidas no display do dispositivo
 * para informar o status do sistema, como o estado da conexão Wi-Fi e o
 * envio das mensagens.
 * 
 * As mensagens são definidas como arrays de strings, e cada mensagem é um
 * conjunto de strings a ser exibido em duas linhas.
 * 
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

// Mensagens de inicialização do dispositivo
static const char *init[] = {
    "    Iniciando   ",
    "   Dispositivo  ",
    NULL // Finaliza o array de mensagens
};

// Mensagens relacionadas ao status da inicialização do Wi-Fi
static const char *wifi_init_success[] = {
    "     Wi-Fi      ",
    "    iniciado!   ",
    NULL
};

static const char *wifi_init_fail[] = {
    "Falha ao iniciar",
    "    o Wi-Fi!    ",
    NULL
};

// Mensagens relacionadas ao status de conexão Wi-Fi
static const char *wifi_connecting[] = {
    "   Conectando    ",
    "     Wi-Fi       ",
    NULL
};

static const char *wifi_conected[] = {
    "     Wi-Fi      ",
    "   conectado!   ",
    NULL
};

static const char *wifi_not_conected[] = {
    "   Wi-Fi nao    ",
    "   conectado!   ",
    NULL
};

// Mensagem informando que o dispositivo está pronto para uso
static const char *ready_to_use[] = {
    "   Dispositivo  ",
    "pronto para uso!",
    NULL
};

// Mensagens relacionadas ao status de envio da mensagem inicial
static const char *msg_init_success[] = {
    "Mensagem inicial ",
    "   enviada com   ", 
    "    sucesso!     ",
    NULL
};

static const char *msg_init_fail[] = {
    " Falha ao enviar ",
    "mensagem inicial!",
    NULL
};

// Mensagens relacionadas ao status de envio da primeira mensagem
static const char *msg_1_success[] = {
    "   Mensagem 1    ",
    "   enviada com   ", 
    "    sucesso!     ",
    NULL
};

static const char *msg_1_fail[] = {
    " Falha ao enviar ",
    "   mensagem 1!   ",
    NULL
};

// Mensagens relacionadas ao status de envio da segunda mensagem
static const char *msg_2_success[] = {
    "   Mensagem 2    ",
    "   enviada com   ", 
    "    sucesso!     ",
    NULL
};

static const char *msg_2_fail[] = {
    " Falha ao enviar ",
    "   mensagem 2!   ",
    NULL
};

// Mensagens relacionadas ao status de envio da terceira mensagem
static const char *msg_3_success[] = {
    "   Mensagem 3    ",
    "   enviada com   ", 
    "    sucesso!     ",
    NULL
};

static const char *msg_3_fail[] = {
    " Falha ao enviar ",
    "   mensagem 3!   ",
    NULL
};

// Mensagens relacionadas ao status de envio da quarta mensagem
static const char *msg_4_success[] = {
    "   Mensagem 4    ",
    "   enviada com   ", 
    "    sucesso!     ",
    NULL
};

static const char *msg_4_fail[] = {
    " Falha ao enviar ",
    "   mensagem 4!   ",
    NULL
};

#endif // DISPLAY_TEXT_H
