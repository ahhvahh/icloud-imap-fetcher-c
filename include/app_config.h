#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define PATH_MAX_LEN 512
#define STR_MAX_LEN 256

typedef struct AppConfig {
    char username[STR_MAX_LEN];
    char app_password[STR_MAX_LEN];
    char imap_url[STR_MAX_LEN];
    char mailbox[STR_MAX_LEN];
    char search_filter[STR_MAX_LEN];
    int mark_as_read;

    char from_contains[STR_MAX_LEN];
    char subject_contains[STR_MAX_LEN];
    char attachment_name_prefix[STR_MAX_LEN];
    int max_messages_per_run;
    int dry_run;

    char download_dir[PATH_MAX_LEN];
    char processed_dir[PATH_MAX_LEN];

    char log_dir[PATH_MAX_LEN];
    char log_level[STR_MAX_LEN];
    int retention_days;

    int interval_seconds;
} AppConfig;

void app_config_set_defaults(AppConfig *config);
int app_config_load(const char *path, AppConfig *config);
void app_config_print(const AppConfig *config);

#endif
