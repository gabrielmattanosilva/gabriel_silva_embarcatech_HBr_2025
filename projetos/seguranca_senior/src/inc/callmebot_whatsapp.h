#ifndef CALLMEBOT_WHATSAPP_H
#define CALLMEBOT_WHATSAPP_H

/**
 * @file callmebot_whatsapp.h
 * @brief Biblioteca para envio de mensagens WhatsApp via CallMeBot no Raspberry Pi Pico W.
 *
 * Esta biblioteca permite o envio de mensagens para o WhatsApp utilizando o serviço CallMeBot.
 * Ela gerencia a conexão Wi-Fi, a resolução de DNS e a comunicação HTTP para realizar o envio
 * das mensagens. Ideal para projetos IoT que precisam de notificações via WhatsApp.
 *
 * @author Gabriel Mattano da Silva
 * @date 2025
 */
     
#include "lwip/tcp.h"        
#include "lwip/dns.h"  
#include "pico/stdlib.h"      

// Definições de constantes                   
#define SERVER_HOSTNAME "api.callmebot.com" // Hostname do servidor CallMeBot
#define SERVER_PORT 80                      // Porta do servidor HTTP

// Definição das mensagens
#define MESSAGE_1 "Estou bem, mas gostaria de conversar. Me ligue por favor?"
#define MESSAGE_2 "Estou tendo dificuldades. Por favor, me ajude."
#define MESSAGE_3 "Estou me sentindo um pouco mal. Me ligue por favor?"
#define MESSAGE_4 "SOCORRO! Preciso de ajuda imediata!"

/**
 * @brief Envia uma mensagem via WhatsApp utilizando o serviço CallMeBot.
 *
 * @param message Ponteiro para a string contendo a mensagem a ser enviada.
 * @param phone Ponteiro para a string contendo o número de telefone de destino.
 * @param apikey Ponteiro para a string contendo a chave de API para autenticação.
 * @return true se a mensagem for enviada com sucesso, false caso contrário.
 */
bool send_whatsapp_message(const char *message, const char *phone, const char *apikey);

#endif // CALLMEBOT_WHATSAPP_H
