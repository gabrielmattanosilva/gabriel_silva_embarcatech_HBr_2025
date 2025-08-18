#include "wifi_manager.h"
#include <string.h>
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rtc_ntp.h"
#include "logger.h"

#define TAG "wifi_manager"

#define WIFI_NTP_HOST           "pool.ntp.org"
#define WIFI_NTP_TIMEOUT_MS     5000U
#define WIFI_BACKOFF_MIN_MS     3000U
#define WIFI_BACKOFF_MAX_MS     300000U
#define WIFI_CONNECT_GUARD_MS   12000U
#define WIFI_CONNECT_TIMEOUT_MS 20000U
#define WIFI_TICK_PERIOD_MS     100U

static bool s_ntp_synced_once = false;

static char g_ssid[64];
static char g_pass[64];

static volatile bool g_arch_ok = false;
static volatile bool g_began = false;
static volatile bool g_connected = false;

static uint32_t g_next_try_ms = 0;
static uint32_t g_backoff_ms = 0;
static int g_prev_link = -999;
static uint32_t g_attempt = 0;

static inline uint32_t now_ms(void) { return to_ms_since_boot(get_absolute_time()); }

static const char *link_status_str(int st)
{
    switch (st)
    {
    case CYW43_LINK_DOWN:
        return "DOWN";
    case CYW43_LINK_JOIN:
        return "JOIN";
    case CYW43_LINK_NOIP:
        return "NOIP";
    case CYW43_LINK_UP:
        return "UP";
    case CYW43_LINK_FAIL:
        return "FAIL";
    case CYW43_LINK_NONET:
        return "NONET";
    case CYW43_LINK_BADAUTH:
        return "BADAUTH";
    default:
        return "UNKNOWN";
    }
}

static void log_current_ip(void)
{
    uint8_t *b = (uint8_t *)&cyw43_state.netif[0].ip_addr.addr;
    LOG(TAG, "IP adquirido: %u.%u.%u.%u", b[0], b[1], b[2], b[3]);
}

static void log_rssi_if_available(void)
{
    int rssi = cyw43_wifi_get_rssi(&cyw43_state, CYW43_ITF_STA);

    if (rssi <= -10 && rssi >= -120)
    {
        LOG(TAG, "RSSI: %d dBm", rssi);
    }
}

static void update_link_state(void)
{
    int st = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
    bool up = (st == CYW43_LINK_UP);
    g_connected = up;

    if (st != g_prev_link)
    {
        LOG(TAG, "Link status: %s -> %s", link_status_str(g_prev_link), link_status_str(st));

        if (up && g_prev_link != CYW43_LINK_UP)
        {
            log_current_ip();
            log_rssi_if_available();

            if (!s_ntp_synced_once)
            {
                LOG(TAG, "Sincronizando RTC via NTP (%s)...", WIFI_NTP_HOST);
                if (rtc_ntp_sync(WIFI_NTP_HOST, WIFI_NTP_TIMEOUT_MS))
                {
                    LOG(TAG, "RTC sincronizado via NTP.");
                    s_ntp_synced_once = true;
                }
                else
                {
                    LOG(TAG, "Falha na sincronização NTP (mantendo RTC atual).");
                }
            }

            g_backoff_ms = WIFI_BACKOFF_MIN_MS;
            g_next_try_ms = now_ms() + WIFI_CONNECT_GUARD_MS;
        }

        g_prev_link = st;

        if (!up && g_backoff_ms == 0)
        {
            g_backoff_ms = WIFI_BACKOFF_MIN_MS;
        }
    }
}

static void try_connect_once(void)
{
    if (!g_arch_ok || g_ssid[0] == '\0')
    {
        LOG(TAG, "try_connect_once(): arch_ok=%d, ssid='%s'", (int)g_arch_ok, g_ssid);
        return;
    }

    ++g_attempt;
    LOG(TAG, "Conectando (tentativa #%u) em \"%s\" ...", (unsigned)g_attempt, g_ssid);

    int rc = cyw43_arch_wifi_connect_timeout_ms(
        g_ssid, g_pass, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS);

    if (rc == 0)
    {
        LOG(TAG, "Assoc. confirmada (aguardando DHCP/IP)");
        update_link_state();
    }
    else
    {
        LOG(TAG, "Sem confirmação dentro do timeout (rc=%d). Aguardando transições...", rc);
    }
}

void wifi_manager_init(const char *ssid, const char *pass)
{
    strncpy(g_ssid, ssid ? ssid : "", sizeof(g_ssid) - 1);
    strncpy(g_pass, pass ? pass : "", sizeof(g_pass) - 1);
    g_ssid[sizeof(g_ssid) - 1] = '\0';
    g_pass[sizeof(g_pass) - 1] = '\0';

    g_began = false;
    g_connected = false;
    g_next_try_ms = 0;
    g_backoff_ms = 0;
    g_prev_link = -999;
    g_attempt = 0;

    s_ntp_synced_once = false;

    LOG(TAG, "Inicializando CYW43...");

    if (cyw43_arch_init() != 0)
    {
        LOG(TAG, "Falha em cyw43_arch_init()");
        g_arch_ok = false;
        return;
    }

    g_arch_ok = true;
    cyw43_arch_enable_sta_mode();
    LOG(TAG, "Init OK (STA). SSID=\"%s\" (threadsafe_background)", g_ssid);
}

bool wifi_manager_is_connected(void)
{
    return g_connected;
}

bool wifi_manager_wait_connected(uint32_t timeout_ms)
{
    LOG(TAG, "Esperando conexão por até %u ms...", (unsigned)timeout_ms);
    absolute_time_t deadline = make_timeout_time_ms(timeout_ms);

    while (!wifi_manager_is_connected())
    {
        if (absolute_time_diff_us(get_absolute_time(), deadline) < 0)
        {
            LOG(TAG, "Timeout aguardando conexão.");
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    LOG(TAG, "Conectado! IP=%s", wifi_manager_ip_str());
    return true;
}

void wifi_manager_force_reconnect(void)
{
    LOG(TAG, "force_reconnect(): zerando backoff e reabrindo janela");
    g_began = false;
    g_backoff_ms = 0;
    g_next_try_ms = 0;
}

void wifi_manager_task(void *params)
{
    (void)params;

    if (!g_arch_ok)
    {
        LOG(TAG, "wifi_manager_task(): begin() não chamado? Iniciando com credenciais atuais.");
        wifi_manager_init(g_ssid, g_pass);
    }

    const TickType_t period = pdMS_TO_TICKS(WIFI_TICK_PERIOD_MS);
    TickType_t last = xTaskGetTickCount();

    for (;;)
    {
        update_link_state();
        uint32_t now = now_ms();

        if (wifi_manager_is_connected())
        {
            if (g_backoff_ms == 0)
            {
                g_backoff_ms = WIFI_BACKOFF_MIN_MS;
            }

            if (g_next_try_ms < now + WIFI_CONNECT_GUARD_MS)
            {
                g_next_try_ms = now + WIFI_CONNECT_GUARD_MS;
            }
        }
        else
        {
            if (g_backoff_ms == 0)
            {
                g_backoff_ms = WIFI_BACKOFF_MIN_MS;
            }

            if ((int32_t)(now - g_next_try_ms) >= 0)
            {
                try_connect_once();

                uint32_t base = g_backoff_ms;
                g_backoff_ms = (g_backoff_ms < WIFI_BACKOFF_MAX_MS)
                                   ? (g_backoff_ms * 2U)
                                   : WIFI_BACKOFF_MAX_MS;
                uint32_t jitter = g_backoff_ms / 10U;
                uint32_t j = jitter ? (rand() % jitter) : 0;
                g_next_try_ms = now + WIFI_CONNECT_GUARD_MS + g_backoff_ms + j;

                LOG(TAG, "Backoff: base=%u ms, next=%u ms (+guard=%u, +jitter=%u)",
                    (unsigned)base, (unsigned)g_backoff_ms,
                    (unsigned)WIFI_CONNECT_GUARD_MS, (unsigned)j);
            }
        }

        vTaskDelayUntil(&last, period);
    }
}

const char *wifi_manager_ip_str(void)
{
    static char ipbuf[20];
    uint8_t *b = (uint8_t *)&cyw43_state.netif[0].ip_addr.addr;
    snprintf(ipbuf, sizeof(ipbuf), "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);
    return ipbuf;
}