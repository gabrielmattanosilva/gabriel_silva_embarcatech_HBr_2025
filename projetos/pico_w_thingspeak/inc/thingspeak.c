/**
 * @file thingspeak.c
 * @brief Implementação para enviar dados ao ThingSpeak.
 *
 * Esta implementação contém a funções necessárias para resolver
 * o DNS do ThingSpeak e enviar de dados para a plataforma.
 *
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

#include "thingspeak.h"

static ip_addr_t server_ip;

/**
 * @brief Configura o servidor DNS e resolve o hostname do ThingSpeak.
 *
 * @return bool Retorna true se a resolução for bem-sucedida ou false em caso de falha.
 */
bool resolve_dns()
{
    // Configura o servidor DNS para o Google (8.8.8.8)
    ip_addr_t dns_server;
    IP4_ADDR(&dns_server, 8, 8, 8, 8);
    dns_setserver(0, &dns_server);
    printf("Servidor DNS configurado para 8.8.8.8\n");

    // Tenta resolver o hostname
    printf("Resolvendo hostname api.thingspeak.com...\n");
    err_t err = dns_gethostbyname("api.thingspeak.com", &server_ip, NULL, NULL);

    if (err == ERR_OK)
    {
        printf("Hostname resolvido: %s\n", ip4addr_ntoa(&server_ip));
        return true;
    }
    else if (err == ERR_INPROGRESS)
    {
        printf("Aguardando resolução de DNS...\n");
        for (int i = 0; i < 50; i++)
        {
            err = dns_gethostbyname("api.thingspeak.com", &server_ip, NULL, NULL);
            if (err == ERR_OK)
            {
                printf("Hostname resolvido: %s\n", ip4addr_ntoa(&server_ip));
                return true;
            }
            sleep_ms(100); // Aguarda um intervalo antes de tentar novamente
        }
        printf("Erro: Tempo limite atingido na resolução de DNS!\n");
    }
    else
    {
        printf("Erro ao resolver DNS: %d\n", err);
    }

    return false;
}

/**
 * @brief Callback para receber a resposta do servidor.
 */
static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (p == NULL)
    {
        printf("Conexão fechada pelo servidor.\n");
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Exibe a resposta do servidor
    char *response = (char *)p->payload;
    char *status_line = strtok(response, "\r\n");
    if (status_line)
    {
        printf("Resposta do servidor: %s\n", status_line);
        if (strstr(status_line, "HTTP/1.1 200 OK") != NULL)
        {
            printf("Dados enviados com sucesso!\n");
        }
        else
        {
            printf("Erro ao enviar os dados!\n");
        }
    }
    else
    {
        printf("Falha ao obter resposta do servidor!\n");
    }

    // Libera o buffer
    pbuf_free(p);
    return ERR_OK;
}

/**
 * @brief Envia dados para o ThingSpeak.
 *
 * @param api_key Chave da API do ThingSpeak.
 * @param num_fields Número de fields a serem enviados.
 * @param ... Valores dos fields.
 */
void send_to_thingspeak(const char *api_key, int num_fields, ...)
{
    char body[256]; // Buffer para o corpo da mensagem
    body[0] = '\0'; // Inicializa o buffer como string vazia

    // Variáveis para funções variádicas
    va_list args;
    va_start(args, num_fields);

    // Constrói o corpo da mensagem dinamicamente
    for (int i = 1; i <= num_fields; i++)
    {
        float field_value = (float)va_arg(args, double);
        printf("Enviando para o field %d: %.2f\n", i, field_value);
        char field_str[32];
        snprintf(field_str, sizeof(field_str), "field%d=%.2f", i, field_value);

        if (strlen(body) > 0)
        {
            strcat(body, "&");
        }

        strcat(body, field_str);
    }

    va_end(args); // Finaliza o uso de argumentos variádicos

    // Cria a requisição HTTP POST
    char request[512]; // Buffer para a requisição HTTP
    snprintf(request, sizeof(request),
             "POST /update HTTP/1.1\r\n"
             "Host: api.thingspeak.com\r\n"
             "Connection: close\r\n"
             "X-THINGSPEAKAPIKEY: %s\r\n"
             "Content-Type: application/x-www-form-urlencoded\r\n"
             "Content-Length: %d\r\n\r\n"
             "%s",
             api_key,
             strlen(body),
             body);

    // Cria um novo PCB (Protocol Control Block) para a conexão TCP
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb)
    {
        printf("Falha ao criar PCB!\n");
        return;
    }

    // Configura o callback para receber a resposta do servidor
    tcp_recv(pcb, tcp_recv_callback);

    // Conecta ao servidor do ThingSpeak na porta 80 (HTTP)
    err_t err = tcp_connect(pcb, &server_ip, 80, NULL);
    if (err != ERR_OK)
    {
        printf("Falha na conexão com o servidor: %d\n", err);
        tcp_close(pcb);
        return;
    }

    // Aloca um buffer (pbuf) para enviar a requisição
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, strlen(request), PBUF_RAM);
    if (!p)
    {
        printf("Falha ao alocar buffer de requisição!\n");
        tcp_close(pcb);
        return;
    }

    // Copia a requisição para o buffer
    memcpy(p->payload, request, strlen(request));
    err = tcp_write(pcb, p->payload, p->len, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK)
    {
        printf("Falha ao escrever requisição: %d\n", err);
        pbuf_free(p);
        tcp_close(pcb);
        return;
    }

    // Libera o buffer
    pbuf_free(p);
}