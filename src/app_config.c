#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app_config.h"

static void trim_in_place(char *text)
{
    char *start;
    char *end;
    size_t len;

    if (text == NULL) {
        return;
    }

    start = text;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }

    if (start != text) {
        memmove(text, start, strlen(start) + 1U);
    }

    len = strlen(text);
    while (len > 0U && isspace((unsigned char)text[len - 1U])) {
        text[len - 1U] = '\0';
        len--;
    }

    end = strchr(text, '#');
    if (end != NULL) {
        *end = '\0';
        trim_in_place(text);
        return;
    }

    end = strchr(text, ';');
    if (end != NULL) {
        *end = '\0';
        trim_in_place(text);
    }
}

static int parse_bool(const char *value)
{
    if (value == NULL) {
        return 0;
    }

    if (strcmp(value, "1") == 0 || strcmp(value, "true") == 0 || strcmp(value, "yes") == 0 || strcmp(value, "on") == 0) {
        return 1;
    }

    return 0;
}

static void set_string(char *destination, size_t destination_size, const char *source)
{
    if (destination == NULL || destination_size == 0U || source == NULL) {
        return;
    }

    snprintf(destination, destination_size, "%s", source);
}

void app_config_set_defaults(AppConfig *config)
{
    if (config == NULL) {
        return;
    }

    memset(config, 0, sizeof(*config));

    set_string(config->imap_url, sizeof(config->imap_url), "imaps://imap.mail.me.com");
    set_string(config->mailbox, sizeof(config->mailbox), "INBOX/Processar");
    set_string(config->search_filter, sizeof(config->search_filter), "UNSEEN");
    set_string(config->attachment_name_prefix, sizeof(config->attachment_name_prefix), "mail_");
    set_string(config->download_dir, sizeof(config->download_dir), "./downloads");
    set_string(config->processed_dir, sizeof(config->processed_dir), "./processed");
    set_string(config->log_dir, sizeof(config->log_dir), "./logs");
    set_string(config->log_level, sizeof(config->log_level), "info");

    config->mark_as_read = 0;
    config->max_messages_per_run = 10;
    config->dry_run = 0;
    config->retention_days = 7;
    config->interval_seconds = 300;
}

int app_config_load(const char *path, AppConfig *config)
{
    FILE *file;
    char line[1024];

    if (path == NULL || config == NULL) {
        return -1;
    }

    app_config_set_defaults(config);

    file = fopen(path, "r");
    if (file == NULL) {
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *equals;
        char *key;
        char *value;

        trim_in_place(line);
        if (line[0] == '\0' || line[0] == '[') {
            continue;
        }

        equals = strchr(line, '=');
        if (equals == NULL) {
            continue;
        }

        *equals = '\0';
        key = line;
        value = equals + 1;

        trim_in_place(key);
        trim_in_place(value);

        if (strcmp(key, "username") == 0) {
            set_string(config->username, sizeof(config->username), value);
        } else if (strcmp(key, "app_password") == 0) {
            set_string(config->app_password, sizeof(config->app_password), value);
        } else if (strcmp(key, "imap_url") == 0) {
            set_string(config->imap_url, sizeof(config->imap_url), value);
        } else if (strcmp(key, "mailbox") == 0) {
            set_string(config->mailbox, sizeof(config->mailbox), value);
        } else if (strcmp(key, "search_filter") == 0) {
            set_string(config->search_filter, sizeof(config->search_filter), value);
        } else if (strcmp(key, "mark_as_read") == 0) {
            config->mark_as_read = parse_bool(value);
        } else if (strcmp(key, "from_contains") == 0) {
            set_string(config->from_contains, sizeof(config->from_contains), value);
        } else if (strcmp(key, "subject_contains") == 0) {
            set_string(config->subject_contains, sizeof(config->subject_contains), value);
        } else if (strcmp(key, "attachment_name_prefix") == 0) {
            set_string(config->attachment_name_prefix, sizeof(config->attachment_name_prefix), value);
        } else if (strcmp(key, "max_messages_per_run") == 0) {
            config->max_messages_per_run = atoi(value);
        } else if (strcmp(key, "dry_run") == 0) {
            config->dry_run = parse_bool(value);
        } else if (strcmp(key, "download_dir") == 0) {
            set_string(config->download_dir, sizeof(config->download_dir), value);
        } else if (strcmp(key, "processed_dir") == 0) {
            set_string(config->processed_dir, sizeof(config->processed_dir), value);
        } else if (strcmp(key, "log_dir") == 0) {
            set_string(config->log_dir, sizeof(config->log_dir), value);
        } else if (strcmp(key, "level") == 0) {
            set_string(config->log_level, sizeof(config->log_level), value);
        } else if (strcmp(key, "retention_days") == 0) {
            config->retention_days = atoi(value);
        } else if (strcmp(key, "interval_seconds") == 0) {
            config->interval_seconds = atoi(value);
        }
    }

    fclose(file);
    return 0;
}

void app_config_print(const AppConfig *config)
{
    if (config == NULL) {
        return;
    }

    printf("username=%s\n", config->username);
    printf("imap_url=%s\n", config->imap_url);
    printf("mailbox=%s\n", config->mailbox);
    printf("search_filter=%s\n", config->search_filter);
    printf("mark_as_read=%d\n", config->mark_as_read);
    printf("from_contains=%s\n", config->from_contains);
    printf("subject_contains=%s\n", config->subject_contains);
    printf("attachment_name_prefix=%s\n", config->attachment_name_prefix);
    printf("max_messages_per_run=%d\n", config->max_messages_per_run);
    printf("dry_run=%d\n", config->dry_run);
    printf("download_dir=%s\n", config->download_dir);
    printf("processed_dir=%s\n", config->processed_dir);
    printf("log_dir=%s\n", config->log_dir);
    printf("log_level=%s\n", config->log_level);
    printf("retention_days=%d\n", config->retention_days);
    printf("interval_seconds=%d\n", config->interval_seconds);
}
