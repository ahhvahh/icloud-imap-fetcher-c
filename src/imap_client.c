#include <curl/curl.h>
#include <stdio.h>

#include "imap_client.h"
#include "logger.h"

int imap_client_fetch(const AppConfig *config)
{
    CURL *curl;
    CURLcode result;
    char message[512];

    if (config == NULL) {
        logger_error("config is null");
        return -1;
    }

    if (config->username[0] == '\0' || config->app_password[0] == '\0') {
        logger_error("username or app_password not configured");
        return -1;
    }

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        logger_error("curl_global_init failed");
        return -1;
    }

    curl = curl_easy_init();
    if (curl == NULL) {
        logger_error("curl_easy_init failed");
        curl_global_cleanup();
        return -1;
    }

    curl_easy_setopt(curl, CURLOPT_USERNAME, config->username);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, config->app_password);
    curl_easy_setopt(curl, CURLOPT_URL, config->imap_url);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    snprintf(message, sizeof(message), "initializing IMAP session mailbox=%.120s filter=%.120s",
             config->mailbox,
             config->search_filter);
    logger_info(message);

    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        snprintf(message, sizeof(message), "curl_easy_perform failed: %s", curl_easy_strerror(result));
        logger_error(message);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return -1;
    }

    logger_info("IMAP connection established");

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}
