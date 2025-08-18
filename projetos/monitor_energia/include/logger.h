#ifndef LOGGER_H
#define LOGGER_H

/* Inicializa o logger (cria mutex). Chame no início do main(). */
void logger_init(void);

/* Função básica de log (use o macro LOG abaixo). */
void logger_log(const char *tag, const char *fmt, ...);

/* Macro único: sempre imprime uma linha com timestamp + TAG. */
#define LOG(TAG, fmt, ...) logger_log((TAG), (fmt), ##__VA_ARGS__)

#endif /* LOGGER_H */
