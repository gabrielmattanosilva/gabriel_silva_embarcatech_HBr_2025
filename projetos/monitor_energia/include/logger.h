#ifndef LOGGER_H
#define LOGGER_H

void logger_init(void);
void logger_log(const char *tag, const char *fmt, ...);

#define LOG(TAG, fmt, ...) logger_log((TAG), (fmt), ##__VA_ARGS__)

#endif /* LOGGER_H */
