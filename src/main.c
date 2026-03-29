#include <stdio.h>
#include <string.h>

#include "app_config.h"
#include "attachment_saver.h"
#include "imap_client.h"
#include "logger.h"
#include "log_http_server.h"

static void print_help(const char *program_name)
{
    printf("Usage: %s [--config <file>] [--serve-logs] [--help]\n", program_name);
}

static const char *get_config_path(int argc, char **argv, int *serve_logs)
{
    int index;
    const char *config_path = "./config/icloud-imap-fetcher.example.conf";

    if (serve_logs != NULL) {
        *serve_logs = 0;
    }

    for (index = 1; index < argc; index++) {
        if (strcmp(argv[index], "--help") == 0) {
            print_help(argv[0]);
            return NULL;
        }

        if (strcmp(argv[index], "--serve-logs") == 0 && serve_logs != NULL) {
            *serve_logs = 1;
        }

        if (strcmp(argv[index], "--config") == 0 && (index + 1) < argc) {
            config_path = argv[index + 1];
            index++;
        }
    }

    return config_path;
}

int main(int argc, char **argv)
{
    AppConfig config;
    int serve_logs = 0;
    const char *config_path = get_config_path(argc, argv, &serve_logs);
    int index;

    if (config_path == NULL) {
        return 0;
    }

    if (app_config_load(config_path, &config) != 0) {
        fprintf(stderr, "failed to load config: %s\n", config_path);
        return 1;
    }

    if (config.mapping_count > 0) {
        for (index = 0; index < config.mapping_count; index++) {
            if (attachment_saver_prepare_directory(config.mappings[index].attachments_dir) != 0) {
                fprintf(stderr, "failed to prepare mapping[%d].attachments_dir\n", index);
                return 1;
            }
            if (attachment_saver_prepare_directory(config.mappings[index].contents_dir) != 0) {
                fprintf(stderr, "failed to prepare mapping[%d].contents_dir\n", index);
                return 1;
            }
        }
    } else {
        if (attachment_saver_prepare_directory(config.download_dir) != 0) {
            fprintf(stderr, "failed to prepare download_dir\n");
            return 1;
        }

        if (attachment_saver_prepare_directory(config.processed_dir) != 0) {
            fprintf(stderr, "failed to prepare processed_dir\n");
            return 1;
        }
    }

    if (attachment_saver_prepare_directory(config.log_dir) != 0) {
        fprintf(stderr, "failed to prepare log_dir\n");
        return 1;
    }

    if (logger_init(config.log_dir, config.retention_days) != 0) {
        fprintf(stderr, "failed to init logger\n");
        return 1;
    }

    logger_info("application started");
    app_config_print(&config);

    if (serve_logs) {
        char message[128];
        if (config.log_http_port <= 0) {
            fprintf(stderr, "log_http_port must be configured (>0) to use --serve-logs\n");
            logger_close();
            return 1;
        }

        snprintf(message, sizeof(message), "starting log HTTP server on port %d", config.log_http_port);
        logger_info(message);
        (void)log_http_server_run(config.log_dir, config.log_http_port);
        logger_close();
        return 0;
    }

    if (imap_client_fetch(&config) != 0) {
        logger_error("fetch routine failed");
        logger_close();
        return 1;
    }

    logger_info("application finished successfully");
    logger_close();
    return 0;
}
