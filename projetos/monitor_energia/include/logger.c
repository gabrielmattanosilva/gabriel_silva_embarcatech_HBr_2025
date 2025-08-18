#include "logger.h"

#include <stdio.h>
#include <stdarg.h>
#include "pico/stdlib.h"           // time_us_64()
#include "hardware/rtc.h"          // rtc_get_datetime
#include "pico/util/datetime.h"    // datetime_t

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

static SemaphoreHandle_t s_mutex = NULL;

static inline int scheduler_started(void) {
    return xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED;
}

/* yyyy/mm/dd hh:mm:ss.mmm */
static void format_timestamp(char *buf, size_t buflen)
{
    datetime_t dt;
    rtc_get_datetime(&dt);
    uint32_t msec = (uint32_t)((time_us_64() / 1000ULL) % 1000ULL);

    // assume RTC já ajustado (ex.: via NTP)
    snprintf(buf, buflen, "%04d/%02d/%02d %02d:%02d:%02d.%03u",
             dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec, msec);
}

void logger_init(void)
{
    if (!s_mutex) {
        s_mutex = xSemaphoreCreateMutex();
    }
}

void logger_log(const char *tag, const char *fmt, ...)
{
    char ts[32];
    format_timestamp(ts, sizeof(ts));

    int locked = 0;
    if (s_mutex && scheduler_started()) {
        if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(50)) == pdTRUE) locked = 1;
    }

    // Cabeçalho fixo + mensagem
    printf("%s %s: ", ts, tag ? tag : "LOG");
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");

    if (locked) xSemaphoreGive(s_mutex);
}
