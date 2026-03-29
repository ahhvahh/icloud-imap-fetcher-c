#include <stdio.h>
#include <string.h>

#include "app_config.h"
#include "attachment_saver.h"
#include "imap_client.h"
#include "logger.h"

static void print_help(const char *program_name)
{
    printf("Usage: %s [--config <file>] [--help]\n", program_name);
}

static const char *get_config_path(int argc, char **argv)
{
    int index;

    for (index = 1; index < argc; index++) {
        if (strcmp(argv[index], "--help") == 0) {
            print_help(argv[0]);
            return NULL;
        }

        if (strcmp(argv[index], "--config") == 0 && (index + 1) < argc) {
            return argv[index + 1];
        }
    }

    return "./config/icloud-imap-fetcher.example.conf";
}

int main(int argc, char **argv)
{
    AppConfig config;
    const char *config_path = get_config_path(argc, argv);

    if (config_path == NULL) {
        return 0;
    }

    if (app_config_load(config_path, &config) != 0) {
        fprintf(stderr, "failed to load config: %s\n", config_path);
        return 1;
    }

    if (attachment_saver_prepare_directory(config.download_dir) != 0) {
        fprintf(stderr, "failed to prepare download_dir\n");
        return 1;
    }

    if (attachment_saver_prepare_directory(config.processed_dir) != 0) {
        fprintf(stderr, "failed to prepare processed_dir\n");
        return 1;
    }

    if (logger_init(config.log_dir, config.retention_days) != 0) {
        fprintf(stderr, "failed to init logger\n");
        return 1;
    }

    logger_info("application started");
    app_config_print(&config);

    if (imap_client_fetch(&config) != 0) {
        logger_error("fetch routine failed");
        logger_close();
        return 1;
    }

    logger_info("application finished successfully");
    logger_close();
    return 0;
}
