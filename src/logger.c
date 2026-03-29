#include <dirent.h>
#include <errno.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "logger.h"

static FILE *g_log_file = NULL;
static char g_log_dir[PATH_MAX] = {0};
static int g_retention_days = 0;
static int g_debug_enabled = 0;

static int has_log_extension(const char *filename)
{
    const char *dot;

    if (filename == NULL) {
        return 0;
    }

    dot = strrchr(filename, '.');
    return dot != NULL && strcmp(dot, ".log") == 0;
}

static int join_path(char *buffer, size_t size, const char *dir, const char *name)
{
    size_t dir_len;
    size_t name_len;

    if (buffer == NULL || size == 0U || dir == NULL || name == NULL) {
        return -1;
    }

    dir_len = strlen(dir);
    name_len = strlen(name);
    if ((dir_len + 1U + name_len + 1U) > size) {
        return -1;
    }

    memcpy(buffer, dir, dir_len);
    buffer[dir_len] = '/';
    memcpy(buffer + dir_len + 1U, name, name_len);
    buffer[dir_len + 1U + name_len] = '\0';
    return 0;
}

static void prune_old_logs(void)
{
    DIR *directory;
    struct dirent *entry;
    time_t now;
    long max_age_seconds;

    directory = opendir(g_log_dir);
    if (directory == NULL) {
        return;
    }

    now = time(NULL);
    max_age_seconds = (long)g_retention_days * 24L * 60L * 60L;

    while ((entry = readdir(directory)) != NULL) {
        char path[PATH_MAX];
        struct stat info;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (!has_log_extension(entry->d_name)) {
            continue;
        }

        if (join_path(path, sizeof(path), g_log_dir, entry->d_name) != 0) {
            continue;
        }

        if (stat(path, &info) != 0) {
            continue;
        }

        if ((now - info.st_mtime) > max_age_seconds) {
            (void)remove(path);
        }
    }

    closedir(directory);
}

static void logger_write(const char *level, const char *message)
{
    char timestamp[32];
    FILE *stream;
    int is_error;
    time_t now;
    struct tm tm_info;

    if (level == NULL || message == NULL) {
        return;
    }

    now = time(NULL);
#if defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    if (localtime_r(&now, &tm_info) == NULL) {
        return;
    }
#else
    {
        struct tm *tmp = localtime(&now);
        if (tmp == NULL) {
            return;
        }
        tm_info = *tmp;
    }
#endif

    if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_info) == 0U) {
        return;
    }

    is_error = (strcmp(level, "ERROR") == 0);
    stream = is_error ? stderr : stdout;

    fprintf(stream, "%s [%s] %s\n", timestamp, level, message);
    fflush(stream);

    if (g_log_file != NULL) {
        fprintf(g_log_file, "%s [%s] %s\n", timestamp, level, message);
        fflush(g_log_file);
    }
}

int logger_init(const char *log_dir, int retention_days, const char *level)
{
    char file_path[PATH_MAX];
    char file_name[64];
    time_t now;
    struct tm tm_info;

    if (log_dir == NULL || log_dir[0] == '\0') {
        return -1;
    }

    if (retention_days < 1) {
        retention_days = 1;
    }

    (void)setvbuf(stdout, NULL, _IOLBF, 0);
    (void)setvbuf(stderr, NULL, _IOLBF, 0);

    now = time(NULL);
#if defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    if (localtime_r(&now, &tm_info) == NULL) {
        return -1;
    }
#else
    {
        struct tm *tmp = localtime(&now);
        if (tmp == NULL) {
            return -1;
        }
        tm_info = *tmp;
    }
#endif

    if (strftime(file_name, sizeof(file_name), "%Y-%m-%d.log", &tm_info) == 0U) {
        return -1;
    }

    if (snprintf(g_log_dir, sizeof(g_log_dir), "%s", log_dir) < 0) {
        return -1;
    }

    g_retention_days = retention_days;
    g_debug_enabled = (level != NULL && strcmp(level, "debug") == 0) ? 1 : 0;

    if (join_path(file_path, sizeof(file_path), g_log_dir, file_name) != 0) {
        return -1;
    }

    g_log_file = fopen(file_path, "a");
    if (g_log_file == NULL) {
        return -1;
    }

    prune_old_logs();
    return 0;
}

void logger_debug(const char *message)
{
    if (!g_debug_enabled) {
        return;
    }
    logger_write("DEBUG", message);
}

void logger_info(const char *message)
{
    logger_write("INFO", message);
}

void logger_error(const char *message)
{
    logger_write("ERROR", message);
}

int logger_is_debug_enabled(void)
{
    return g_debug_enabled;
}

void logger_close(void)
{
    if (g_log_file != NULL) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
}
