/**
 * @file thingspeak.c
 * @brief Envio de leituras ao ThingSpeak usando TCP bruto (lwIP).
 * @details
 *  Oferece `thingspeak_send()` para montar uma requisição HTTP GET e
 *  `thingspeak_task()` que acumula energia e envia periodicamente.
 */

#include "lib/thingspeak.h"
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "pico/time.h"
#include "lwip/ip_addr.h"
#include "lwip/tcp.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "utils.h"
#include "lib/energy_monitor.h"
#include "lib/wifi_manager.h"
#include "credentials.h"
#include "lib/logger.h"

#define THINGSPEAK_HOST             "api.thingspeak.com"    /**< Host do ThingSpeak. */
#define THINGSPEAK_PORT             80                      /**< Porta HTTP. */
#define THINGSPEAK_TCP_TIMEOUT_MS   7000U                   /**< Timeout de resposta TCP (ms). */

/** @brief Contexto interno para operação HTTP simples sobre TCP. */
typedef struct
{
    struct tcp_pcb *pcb;
    SemaphoreHandle_t sem_done;
    err_t last_err;
    int sent;
    int finished;
} ts_http_ctx_t;

/**
 * @brief Retorna o uptime do sistema em segundos.
 * @return Segundos desde o boot.
 */
static inline uint32_t uptime_s(void)
{
    return (uint32_t)(to_ms_since_boot(get_absolute_time()) / 1000u);
}

/**
 * @brief Zera acumuladores de energia/tempo de 10 min.
 * @param e10_wh Ponteiro para energia acumulada (Wh).
 * @param acc_s Ponteiro para acumulador de tempo (s).
 */
static inline void reset_e10_wh_acc_s(double *e10_wh, uint32_t *acc_s)
{
    *e10_wh = 0.0;
    *acc_s = 0u;
}

/**
 * @brief Callback de recepção TCP (HTTP/1.1 Connection: close).
 */
static err_t thingspeak_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    ts_http_ctx_t *ctx = (ts_http_ctx_t *)arg;
    ctx->last_err = err;

    if (p == NULL)
    {
        ctx->finished = 1;
        tcp_close(tpcb);
        ctx->pcb = NULL;
        xSemaphoreGive(ctx->sem_done);
        return ERR_OK;
    }

    tcp_recved(tpcb, p->len);
    pbuf_free(p);
    return ERR_OK;
}

/**
 * @brief Callback de erro TCP.
 */
static void thingspeak_tcp_err(void *arg, err_t err)
{
    ts_http_ctx_t *ctx = (ts_http_ctx_t *)arg;
    ctx->last_err = err;
    ctx->finished = 1;

    if (ctx->sem_done)
    {
        xSemaphoreGive(ctx->sem_done);
    }
}

/**
 * @brief Callback de confirmação de envio TCP.
 */
static err_t thingspeak_tcp_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    (void)arg;
    (void)tpcb;
    (void)len;
    return ERR_OK;
}

/**
 * @brief Callback de conexão estabelecida TCP.
 */
static err_t thingspeak_tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    ts_http_ctx_t *ctx = (ts_http_ctx_t *)arg;
    ctx->last_err = err;

    if (err != ERR_OK)
    {
        ctx->finished = 1;

        if (ctx->sem_done)
        {
            xSemaphoreGive(ctx->sem_done);
        }

        return err;
    }

    return ERR_OK;
}

/**
 * @brief Envia campos ao ThingSpeak (GET /update) com API key e até 8 campos numéricos.
 * @param api_key Chave de escrita do canal.
 * @param num_fields Quantidade de campos (1..8).
 * @param ... Lista de valores `double` (field1..fieldN).
 * @note NaN/Inf são convertidos para 0.0.
 */
void thingspeak_send(const char *api_key, uint8_t num_fields, ...)
{
    if (!api_key || num_fields <= 0 || num_fields > 8)
    {
        LOG("ThingSpeak", "Parâmetros inválidos (api_key/num_fields)");
        return;
    }

    char qs[256];
    size_t pos = 0;
    pos += (size_t)snprintf(qs + pos, sizeof(qs) - pos, "api_key=%s", api_key);

    va_list ap;
    va_start(ap, num_fields);

    for (int i = 1; i <= num_fields && pos < sizeof(qs); i++)
    {
        double val = va_arg(ap, double);

        if (isnan(val) || isinf(val))
        {
            val = 0.0;
        }

        pos += (size_t)snprintf(qs + pos, sizeof(qs) - pos, "&field%d=%.6f", i, val);
    }

    va_end(ap);

    char req[512];
    int nreq = snprintf(req, sizeof(req),
                        "GET /update?%s HTTP/1.1\r\n"
                        "Host: %s\r\n"
                        "User-Agent: pico-w/rawtcp\r\n"
                        "Connection: close\r\n"
                        "\r\n",
                        qs, THINGSPEAK_HOST);

    if (nreq <= 0 || nreq >= (int)sizeof(req))
    {
        LOG("ThingSpeak", "Requisição muito grande");
        return;
    }

    ip_addr_t ip;

    if (!utils_resolve_dns(THINGSPEAK_HOST, &ip, 5000))
    {
        LOG("ThingSpeak", "DNS falhou para %s", THINGSPEAK_HOST);
        return;
    }

    ts_http_ctx_t ctx = {0};
    ctx.pcb = tcp_new_ip_type(IPADDR_TYPE_V4);

    if (!ctx.pcb)
    {
        LOG("ThingSpeak", "tcp_new falhou");
        return;
    }

    ctx.sem_done = xSemaphoreCreateBinary();

    if (!ctx.sem_done)
    {
        LOG("ThingSpeak", "semáforo falhou");
        tcp_abort(ctx.pcb);
        return;
    }

    tcp_arg(ctx.pcb, &ctx);
    tcp_err(ctx.pcb, thingspeak_tcp_err);
    tcp_recv(ctx.pcb, thingspeak_tcp_recv);
    tcp_sent(ctx.pcb, thingspeak_tcp_sent);

    err_t err = tcp_connect(ctx.pcb, &ip, THINGSPEAK_PORT, thingspeak_tcp_connected);

    if (err != ERR_OK)
    {
        LOG("ThingSpeak", "tcp_connect err=%d", (int)err);
        vSemaphoreDelete(ctx.sem_done);
        tcp_abort(ctx.pcb);
        return;
    }

    vTaskDelay(pdMS_TO_TICKS(30));

    if (!ctx.sent && ctx.pcb)
    {
        err = tcp_write(ctx.pcb, req, (u16_t)nreq, TCP_WRITE_FLAG_COPY);

        if (err == ERR_OK)
        {
            tcp_output(ctx.pcb);
            ctx.sent = 1;
        }
        else
        {
            LOG("ThingSpeak", "tcp_write err=%d", (int)err);
            tcp_abort(ctx.pcb);
            vSemaphoreDelete(ctx.sem_done);
            return;
        }
    }

    if (xSemaphoreTake(ctx.sem_done, pdMS_TO_TICKS(THINGSPEAK_TCP_TIMEOUT_MS)) != pdTRUE)
    {
        LOG("ThingSpeak", "timeout aguardando resposta");

        if (ctx.pcb)
        {
            tcp_abort(ctx.pcb);
        }

        vSemaphoreDelete(ctx.sem_done);
        return;
    }

    if (ctx.pcb)
    {
        tcp_arg(ctx.pcb, NULL);
        tcp_recv(ctx.pcb, NULL);
        tcp_err(ctx.pcb, NULL);
        tcp_sent(ctx.pcb, NULL);
        tcp_close(ctx.pcb);
    }

    vSemaphoreDelete(ctx.sem_done);

    LOG("ThingSpeak", "Enviado para %s (%d bytes)", THINGSPEAK_HOST, nreq);
}

/**
 * @brief Task que acumula energia e envia leituras ao ThingSpeak.
 * @param params Não utilizado.
 * @details
 *  Envia imediatamente após o Wi-Fi ficar UP, depois a cada
 *  `THINGSPEAK_SEND_PERIOD_S` enquanto conectado.
 */
void thingspeak_task(void *params)
{
    (void)params;

    const TickType_t tick_period = pdMS_TO_TICKS(THINGSPEAK_TICK_S * 1000u);
    TickType_t last_wake = xTaskGetTickCount();

    uint64_t last_ms = to_ms_since_boot(get_absolute_time());
    double e10_wh = 0.0;
    uint32_t acc_s = 0u;
    bool was_up = false;
    bool first_send_done = false;

    energy_monitor_data_t em = {0};
    bool have_em = false;

    LOG("ThingSpeak", "Task iniciada: envia no UP e depois a cada %u s.",
        (unsigned)THINGSPEAK_SEND_PERIOD_S);

    for (;;)
    {
        vTaskDelayUntil(&last_wake, tick_period);

        uint64_t now_ms = to_ms_since_boot(get_absolute_time());
        uint32_t dt_ms = (uint32_t)(now_ms - last_ms);
        last_ms = now_ms;

        have_em = energy_monitor_get_last(&em);

        if (have_em)
        {
            double dt_s = (double)dt_ms / 1000.0;
            e10_wh += (em.p_instant * dt_s) / 3600.0;
        }

        acc_s += (dt_ms / 1000u);

        bool up = wifi_manager_is_connected();
        bool just_up = (up && !was_up);

        if (just_up)
        {
            (void)utils_resolve_dns(THINGSPEAK_HOST, NULL, 5000);

            float v = have_em ? (float)em.vrms : 0.0f;
            float i = have_em ? (float)em.irms : 0.0f;
            float p = have_em ? (float)em.p_instant : 0.0f;
            float e = (float)e10_wh;
            float upsecs = (float)uptime_s();

            thingspeak_send(API_KEY, 5, v, i, p, e, upsecs);

            reset_e10_wh_acc_s(&e10_wh, &acc_s);
            first_send_done = true;
        }

        was_up = up;

        if (up && first_send_done && acc_s >= THINGSPEAK_SEND_PERIOD_S)
        {
            float v = have_em ? (float)em.vrms : 0.0f;
            float i = have_em ? (float)em.irms : 0.0f;
            float p = have_em ? (float)em.p_instant : 0.0f;
            float e = (float)e10_wh;
            float upsecs = (float)uptime_s();

            thingspeak_send(API_KEY, 5, v, i, p, e, upsecs);

            reset_e10_wh_acc_s(&e10_wh, &acc_s);
        }
    }
}
