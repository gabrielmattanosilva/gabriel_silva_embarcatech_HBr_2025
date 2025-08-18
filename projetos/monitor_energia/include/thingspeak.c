/* =========================
 *  HTTP GET -> ThingSpeak
 *  (RAW lwIP, sem sockets)
 * ========================= */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "lwip/ip_addr.h"
#include "lwip/tcp.h"
#include "FreeRTOS.h"
#include "semphr.h"

#include "utils.h"        // utils_resolve_dns()
#include "credentials.h"  // API_KEY
#include "thingspeak.h"   // THINGSPEAK_HOST/PORT
#include "logger.h"
#include "energy_monitor.h"
#include "wifi_manager.h"
#include "pico/time.h"


// Se não estiver em thingspeak.h, deixe aqui:
#ifndef THINGSPEAK_HOST
#define THINGSPEAK_HOST "api.thingspeak.com"
#endif
#ifndef THINGSPEAK_PORT
#define THINGSPEAK_PORT 80
#endif
#ifndef THINGSPEAK_TCP_TIMEOUT_MS
#define THINGSPEAK_TCP_TIMEOUT_MS 7000u
#endif

typedef struct {
    struct tcp_pcb *pcb;
    SemaphoreHandle_t sem_done;
    err_t last_err;
    int   sent;         // já enviou request?
    int   finished;     // recebeu/fechou
} ts_http_ctx_t;

/* ---- Callbacks RAW TCP ---- */
static err_t ts_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    ts_http_ctx_t *ctx = (ts_http_ctx_t *)arg;
    ctx->last_err = err;

    if (p == NULL) {
        // remoto fechou
        ctx->finished = 1;
        tcp_close(tpcb);
        ctx->pcb = NULL;
        xSemaphoreGive(ctx->sem_done);
        return ERR_OK;
    }

    // Consome os dados e avança a janela
    tcp_recved(tpcb, p->len);
    pbuf_free(p);
    return ERR_OK;
}

static void ts_tcp_err(void *arg, err_t err)
{
    ts_http_ctx_t *ctx = (ts_http_ctx_t *)arg;
    ctx->last_err = err;
    ctx->finished = 1;
    if (ctx->sem_done) xSemaphoreGive(ctx->sem_done);
}

static err_t ts_tcp_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    (void)arg; (void)tpcb; (void)len;
    return ERR_OK;
}

static err_t ts_tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    ts_http_ctx_t *ctx = (ts_http_ctx_t *)arg;
    ctx->last_err = err;

    if (err != ERR_OK) {
        ctx->finished = 1;
        if (ctx->sem_done) xSemaphoreGive(ctx->sem_done);
        return err;
    }

    // Se chegou aqui, estamos conectados; o write virá de fora.
    return ERR_OK;
}

void send_to_thingspeak(const char *api_key, int num_fields, ...)
{
    if (!api_key || num_fields <= 0 || num_fields > 8) {
        LOG("ThingSpeak", "Parâmetros inválidos (api_key/num_fields)");
        return;
    }

    // 1) Monta query string field1..fieldN
    char qs[256];
    size_t pos = 0;
    pos += (size_t)snprintf(qs + pos, sizeof(qs) - pos, "api_key=%s", api_key);

    va_list ap;
    va_start(ap, num_fields);
    for (int i = 1; i <= num_fields && pos < sizeof(qs); i++) {
        double val = va_arg(ap, double);  // float -> promovido a double
        if (isnan(val) || isinf(val)) val = 0.0;
        pos += (size_t)snprintf(qs + pos, sizeof(qs) - pos, "&field%d=%.6f", i, val);
    }
    va_end(ap);

    // 2) Monta request HTTP
    char req[512];
    int nreq = snprintf(req, sizeof(req),
                        "GET /update?%s HTTP/1.1\r\n"
                        "Host: %s\r\n"
                        "User-Agent: pico-w/rawtcp\r\n"
                        "Connection: close\r\n"
                        "\r\n",
                        qs, THINGSPEAK_HOST);
    if (nreq <= 0 || nreq >= (int)sizeof(req)) {
        LOG("ThingSpeak", "Requisição muito grande");
        return;
    }

    // 3) DNS
    ip_addr_t ip;
    if (!utils_resolve_dns(THINGSPEAK_HOST, &ip, 5000)) {
        LOG("ThingSpeak", "DNS falhou para %s", THINGSPEAK_HOST);
        return;
    }

    // 4) Cria PCB RAW e conecta
    ts_http_ctx_t ctx = {0};
    ctx.pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!ctx.pcb) {
        LOG("ThingSpeak", "tcp_new falhou");
        return;
    }

    ctx.sem_done = xSemaphoreCreateBinary();
    if (!ctx.sem_done) {
        LOG("ThingSpeak", "semáforo falhou");
        tcp_abort(ctx.pcb);
        return;
    }

    tcp_arg(ctx.pcb, &ctx);
    tcp_err(ctx.pcb, ts_tcp_err);
    tcp_recv(ctx.pcb, ts_tcp_recv);
    tcp_sent(ctx.pcb, ts_tcp_sent);

    err_t err = tcp_connect(ctx.pcb, &ip, THINGSPEAK_PORT, ts_tcp_connected);
    if (err != ERR_OK) {
        LOG("ThingSpeak", "tcp_connect err=%d", (int)err);
        vSemaphoreDelete(ctx.sem_done);
        tcp_abort(ctx.pcb);
        return;
    }

    // 5) Aguarda pequena janela para a conexão completar e envia
    // (tipicamente, a stack chamará ts_tcp_connected quase imediatamente)
    vTaskDelay(pdMS_TO_TICKS(30));

    if (!ctx.sent && ctx.pcb) {
        err = tcp_write(ctx.pcb, req, (u16_t)nreq, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(ctx.pcb);
            ctx.sent = 1;
        } else {
            LOG("ThingSpeak", "tcp_write err=%d", (int)err);
            // Fecha e sai
            tcp_abort(ctx.pcb);
            vSemaphoreDelete(ctx.sem_done);
            return;
        }
    }

    // 6) Espera fechamento remoto (Connection: close) ou timeout
    if (xSemaphoreTake(ctx.sem_done, pdMS_TO_TICKS(THINGSPEAK_TCP_TIMEOUT_MS)) != pdTRUE) {
        LOG("ThingSpeak", "timeout aguardando resposta");
        if (ctx.pcb) tcp_abort(ctx.pcb);
        vSemaphoreDelete(ctx.sem_done);
        return;
    }

    // 7) Limpeza
    if (ctx.pcb) {
        tcp_arg(ctx.pcb, NULL);
        tcp_recv(ctx.pcb, NULL);
        tcp_err(ctx.pcb, NULL);
        tcp_sent(ctx.pcb, NULL);
        tcp_close(ctx.pcb);
    }
    vSemaphoreDelete(ctx.sem_done);

    LOG("ThingSpeak", "Enviado para %s (%d bytes)", THINGSPEAK_HOST, nreq);
}

/* --- Helpers --- */
static inline uint32_t uptime_s(void) {
    return (uint32_t)(to_ms_since_boot(get_absolute_time()) / 1000u);
}

static inline void reset_janela(double *e10_wh, uint32_t *acc_s) {
    *e10_wh = 0.0;
    *acc_s  = 0u;
}

void thingspeak_task(void *params)
{
    (void)params;

    const TickType_t tick_period = pdMS_TO_TICKS(THINGSPEAK_TICK_S * 1000u);
    TickType_t last_wake = xTaskGetTickCount();

    uint64_t last_ms = to_ms_since_boot(get_absolute_time());
    double   e10_wh  = 0.0;   /* energia acumulada [Wh] */
    uint32_t acc_s   = 0u;    /* segundos acumulados na janela */
    bool     was_up  = false;
    bool     first_send_done = false;

    energy_monitor_data_t em = {0};
    bool have_em = false;

    LOG("ThingSpeak", "Task iniciada: envia no UP e depois a cada %u s.",
        (unsigned)THINGSPEAK_SEND_PERIOD_S);

    for (;;)
    {
        vTaskDelayUntil(&last_wake, tick_period);

        /* tempo decorrido real */
        uint64_t now_ms = to_ms_since_boot(get_absolute_time());
        uint32_t dt_ms  = (uint32_t)(now_ms - last_ms);
        last_ms = now_ms;

        /* lê últimos valores de energia */
        have_em = energy_monitor_get_last(&em);

        /* integra energia em Wh */
        if (have_em) {
            double dt_s = (double)dt_ms / 1000.0;
            e10_wh += (em.p_instant * dt_s) / 3600.0;
        }
        acc_s += (dt_ms / 1000u);

        /* estado do Wi-Fi */
        bool up = wifi_manager_is_connected();
        bool just_up = (up && !was_up);

        /* envio imediato ao conectar */
        if (just_up) {
            (void)utils_resolve_dns(THINGSPEAK_HOST, NULL, 5000); // resolve só para garantir

            float v = have_em ? (float)em.vrms      : 0.0f;
            float i = have_em ? (float)em.irms      : 0.0f;
            float p = have_em ? (float)em.p_instant : 0.0f;
            float e = (float)e10_wh;
            float upsecs = (float)uptime_s();

            send_to_thingspeak(API_KEY, 5, v, i, p, e, upsecs);

            reset_janela(&e10_wh, &acc_s);
            first_send_done = true;
        }
        was_up = up;

        /* envio periódico de 10 min */
        if (up && first_send_done && acc_s >= THINGSPEAK_SEND_PERIOD_S) {
            float v = have_em ? (float)em.vrms      : 0.0f;
            float i = have_em ? (float)em.irms      : 0.0f;
            float p = have_em ? (float)em.p_instant : 0.0f;
            float e = (float)e10_wh;
            float upsecs = (float)uptime_s();

            send_to_thingspeak(API_KEY, 5, v, i, p, e, upsecs);

            reset_janela(&e10_wh, &acc_s);
        }
    }
}