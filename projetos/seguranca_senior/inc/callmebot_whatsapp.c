/**
 * @file callmebot_whatsapp.c
 * @brief Implementação da biblioteca para envio de mensagens via WhatsApp usando CallMeBot.
 *
 * Esta implementação permite que um Raspberry Pi Pico W envie mensagens de WhatsApp
 * utilizando o serviço CallMeBot. A comunicação é feita via TCP/IP, resolvendo o 
 * DNS do servidor CallMeBot e enviando requisições HTTP.
 *
 * Recursos principais:
 * - Configuração do servidor DNS para resolução do CallMeBot.
 * - Resolução de hostname para endereço IP.
 * - Codificação de strings para formato URL.
 * - Comunicação via TCP para envio das mensagens.
 * - Processamento da resposta do servidor para validar o envio.
 *
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "callmebot_whatsapp.h"

// Variáveis globais
static ip4_addr_t server_ip;      // Armazena o IP do servidor CallMeBot
static int dns_resolved = 0;      // Flag para indicar se o DNS já foi resolvido
static bool message_sent = false; // Status do envio da mensagem

/**
 * @brief Configura o servidor DNS para o Google (8.8.8.8)
 */
static void set_dns_server()
{
    ip4_addr_t dns_server;
    IP4_ADDR(&dns_server, 8, 8, 8, 8);
    dns_setserver(0, &dns_server);
    printf("Servidor DNS configurado para 8.8.8.8\n");
}

/**
 * @brief Resolve o hostname do servidor CallMeBot para um endereço IP.
 *
 * @param hostname Nome do host a ser resolvido.
 * @param ip_addr Ponteiro para armazenar o endereço IP resolvido.
 * @return err_t Código de erro da resolução DNS.
 */
static err_t resolve_dns(const char *hostname, ip4_addr_t *ip_addr)
{
    if (dns_resolved)
    {
        *ip_addr = server_ip;
        printf("Usando IP já resolvido: %s\n", ip4addr_ntoa(ip_addr));
        return ERR_OK;
    }

    struct ip4_addr temp_ip;
    err_t err;

    printf("Resolvendo hostname: %s\n", hostname);
    err = dns_gethostbyname(hostname, &temp_ip, NULL, NULL);

    if (err == ERR_OK)
    {
        *ip_addr = temp_ip;
        server_ip = temp_ip;
        dns_resolved = 1;
        printf("Hostname resolvido: %s -> %s\n", hostname, ip4addr_ntoa(&temp_ip));
        return ERR_OK;
    }
    else if (err == ERR_INPROGRESS)
    {
        printf("Aguardando resolução de DNS...\n");
        for (int i = 0; i < 50; i++) // Tenta resolver o DNS por um tempo limitado
        {
            err = dns_gethostbyname(hostname, &temp_ip, NULL, NULL);
            if (err == ERR_OK)
            {
                *ip_addr = temp_ip;
                server_ip = temp_ip;
                dns_resolved = 1;
                printf("Hostname resolvido: %s -> %s\n", hostname, ip4addr_ntoa(&temp_ip));
                return ERR_OK;
            }
            sleep_ms(100); // Aguarda um intervalo antes de tentar novamente
        }
        printf("Erro: Tempo limite atingido na resolução de DNS\n");
    }
    else
    {
        printf("Erro ao resolver DNS: %d\n", err);
    }

    return err;
}

/**
 * @brief Codifica uma string no formato URL.
 *
 * @param input String de entrada.
 * @param output Buffer para armazenar a string codificada.
 * @param output_size Tamanho máximo do buffer de saída.
 */
void url_encode(const char *input, char *output, int output_size)
{
    int i, j = 0;

    for (i = 0; input[i] != '\0' && j < output_size - 1; i++)
    {
        if (isalnum((unsigned char)input[i]) || input[i] == '-' || input[i] == '_' || input[i] == '.' || input[i] == '~')
        {
            output[j++] = input[i];
        }
        else if (input[i] == ' ')
        {
            output[j++] = '+'; // Substitui espaço por '+'
        }
        else
        {
            if (j + 3 < output_size - 1) // Verifica se há espaço suficiente para codificar o caractere
            {
                snprintf(&output[j], 4, "%%%02X", (unsigned char)input[i]);
                j += 3; // Adiciona a representação codificada ao buffer
            }
            else
            {
                break; // Interrompe se o buffer está cheio
            }
        }
    }
    
    output[j] = '\0'; // Garante que a string esteja corretamente terminada
}

/**
 * @brief Callback para processar a resposta do servidor após o envio da mensagem.
 */
static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (p == NULL)
    {
        tcp_close(tpcb); // Fecha a conexão se não houver dados recebidos
        return ERR_OK;
    }

    char *response = (char *)p->payload;
    printf("Resposta da API: %s\n", strtok(response, "\r\n")); // Exibe a primeira linha da resposta

    if (strstr(response, "HTTP/1.1 200 OK") != NULL)
    {
        message_sent = true; // Marca como enviado se o código de status for 200
        printf("Mensagem enviada com sucesso (Código 200)\n");
    }
    else
    {
        message_sent = false;
        printf("Erro ao enviar mensagem\n");
    }

    pbuf_free(p); // Libera a memória usada pelo buffer
    tcp_close(tpcb); // Fecha a conexão TCP
    printf("Conexão fechada após resposta recebida.\n");
    return ERR_OK;
}

/**
 * @brief Envia uma mensagem via WhatsApp usando a API CallMeBot.
 *
 * @param message Texto da mensagem.
 * @param phone Número de telefone do destinatário.
 * @param apikey Chave da API CallMeBot.
 * @return true se a mensagem foi enviada com sucesso, false caso contrário.
 */
bool send_whatsapp_message(const char *message, const char *phone, const char *apikey)
{
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb)
    {
        printf("Erro ao criar PCB\n");
        return false;
    }

    set_dns_server(); // Configura o servidor DNS
    if (resolve_dns(SERVER_HOSTNAME, &server_ip) != ERR_OK) // Resolve o hostname
    {
        printf("Erro ao resolver DNS\n");
        return false;
    }

    if (tcp_connect(pcb, &server_ip, SERVER_PORT, NULL) != ERR_OK) // Estabelece a conexão TCP
    {
        printf("Erro ao conectar ao servidor\n");
        return false;
    }

    printf("Conectado ao CallMeBot. Enviando mensagem...\n");

    char encoded_message[1024];
    url_encode(message, encoded_message, sizeof(encoded_message)); // Codifica a mensagem

    char request[1024];
    snprintf(request, sizeof(request),
            "GET /whatsapp.php?phone=%s&text=%s&apikey=%s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Connection: close\r\n"
            "User-Agent: Mozilla/5.0\r\n"
            "Accept: */*\r\n\r\n",
            phone, encoded_message, apikey, SERVER_HOSTNAME); // Monta a requisição HTTP

    err_t err = tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY); // Envia a requisição
    if (err != ERR_OK)
    {
        printf("Erro ao enviar requisição, código: %d\n", err);
        return false;
    }

    tcp_recv(pcb, recv_callback); // Define o callback para processar a resposta

    absolute_time_t start_time = get_absolute_time();
    while (!message_sent && absolute_time_diff_us(start_time, get_absolute_time()) < 10000000)
    {
        sleep_ms(100); // Aguarda a resposta do servidor por um tempo limite
    }

    if (!message_sent)
    {
        printf("Erro: Timeout ao aguardar resposta do servidor\n");
        tcp_close(pcb); // Fecha a conexão após o timeout
        return false;
    }

    return true;
}
