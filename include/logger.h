#ifndef LOGGER_H
#define LOGGER_H

int logger_init(const char *log_dir, int retention_days, const char *level);
void logger_debug(const char *message);
void logger_info(const char *message);
void logger_error(const char *message);
int logger_is_debug_enabled(void);
void logger_close(void);

#endif
