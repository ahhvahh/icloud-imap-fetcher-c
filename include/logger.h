#ifndef LOGGER_H
#define LOGGER_H

int logger_init(const char *log_dir, int retention_days);
void logger_info(const char *message);
void logger_error(const char *message);
void logger_close(void);

#endif
