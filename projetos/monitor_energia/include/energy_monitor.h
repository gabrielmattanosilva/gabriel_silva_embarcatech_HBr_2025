#ifndef ENERGY_MONITOR_H
#define ENERGY_MONITOR_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Estrutura dos resultados calculados pela task */
typedef struct {
    double   vrms;        /* Tensão RMS [V] */
    double   irms;        /* Corrente RMS [A] */
    double   v_pu;        /* Tensão em PU (base 127 V) [-] */
    double   p_instant;   /* Potência instantânea no último sample [W] */
    uint32_t t_ms;        /* Timestamp (ms desde boot) do último sample usado em p_instant */
} energy_monitor_data_t;

/* A própria task, criada no main com xTaskCreate(energy_monitor_task, ...) */
void energy_monitor_task(void *params);

/* Getter opcional: retorna true se já há dados válidos e copia para *out */
bool energy_monitor_get_last(energy_monitor_data_t *out);

#ifdef __cplusplus
}
#endif

#endif /* ENERGY_MONITOR_H */
