#include "thingspeak.h"
#include "utils.h"
#include "logger.h"

static ip_addr_t server_ip;
#define TAG "ThingSpeak"

static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    (void)arg; (void)tpcb; (void)err;
    if (p == NULL) {
        LOG(TAG, "Conexão fechada pelo servidor.");
        if (tpcb) tcp_close(tpcb);
        return ERR_OK;
    }

    char *response = (char *)p->payload;
    char *status_line = strtok(response, "\r\n");
    if (status_line) {
        LOG(TAG, "Resposta: %s", status_line);
        if (strstr(status_line, "HTTP/1.1 200") != NULL) {
            LOG(TAG, "Dados enviados com sucesso!");
        } else {
            LOG(TAG, "Falha ao enviar dados.");
        }
    } else {
        LOG(TAG, "Resposta inválida do servidor.");
    }

    pbuf_free(p);
    return ERR_OK;
}

void send_to_thingspeak(const char *api_key, int num_fields, ...)
{
    if (!utils_resolve_dns("api.thingspeak.com", &server_ip, 5000)) {
        LOG(TAG, "DNS falhou para api.thingspeak.com");
        return;
    }

    char body[256]; body[0] = '\0';
    va_list args; va_start(args, num_fields);
    for (int i = 1; i <= num_fields; i++) {
        float field_value = (float)va_arg(args, double);
        char field_str[32];
        snprintf(field_str, sizeof(field_str), "field%d=%.2f", i, field_value);
        if (strlen(body) > 0) strcat(body, "&");
        strcat(body, field_str);
        LOG(TAG, "Field %d = %.2f", i, field_value);
    }
    va_end(args);

    char request[512];
    snprintf(request, sizeof(request),
             "POST /update HTTP/1.1\r\n"
             "Host: api.thingspeak.com\r\n"
             "Connection: close\r\n"
             "X-THINGSPEAKAPIKEY: %s\r\n"
             "Content-Type: application/x-www-form-urlencoded\r\n"
             "Content-Length: %d\r\n\r\n"
             "%s",
             api_key, (int)strlen(body), body);

    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        LOG(TAG, "Falha ao criar PCB TCP");
        return;
    }
    tcp_recv(pcb, tcp_recv_callback);

    err_t err = tcp_connect(pcb, &server_ip, 80, NULL);
    if (err != ERR_OK) {
        LOG(TAG, "Falha em tcp_connect (err=%d)", (int)err);
        tcp_close(pcb);
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, strlen(request), PBUF_RAM);
    if (!p) {
        LOG(TAG, "Falha ao alocar pbuf req");
        tcp_close(pcb);
        return;
    }

    memcpy(p->payload, request, strlen(request));
    err = tcp_write(pcb, p->payload, p->len, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        LOG(TAG, "Falha em tcp_write (err=%d)", (int)err);
        pbuf_free(p);
        tcp_close(pcb);
        return;
    }

    pbuf_free(p);
    LOG(TAG, "Requisição enviada para %s", ip4addr_ntoa(&server_ip));
}
