#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

#include "imap_client.h"
#include "logger.h"

static int curl_trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr)
{
    char buffer[1024];
    size_t copy_size;

    (void)handle;
    (void)userptr;

    if (!logger_is_debug_enabled() || data == NULL || size == 0U) {
        return 0;
    }

    copy_size = size;
    if (copy_size > sizeof(buffer) - 1U) {
        copy_size = sizeof(buffer) - 1U;
    }

    memcpy(buffer, data, copy_size);
    buffer[copy_size] = '\0';

    if (type == CURLINFO_TEXT) {
        logger_debug(buffer);
    } else if (type == CURLINFO_HEADER_IN) {
        logger_debug("<<< IMAP IN");
        logger_debug(buffer);
    } else if (type == CURLINFO_HEADER_OUT) {
        logger_debug(">>> IMAP OUT");
        logger_debug(buffer);
    } else if (type == CURLINFO_DATA_IN) {
        logger_debug("<<< DATA IN");
        logger_debug(buffer);
    } else if (type == CURLINFO_DATA_OUT) {
        logger_debug(">>> DATA OUT");
        logger_debug(buffer);
    }

    return 0;
}

static int perform_mailbox_search(CURL *curl,
                                  const char *base_url,
                                  const char *mailbox,
                                  const char *search_filter)
{
    CURLcode result;
    char effective_url[512];
    char command[256];
    char error_buffer[CURL_ERROR_SIZE];
    char message[1024];

    if (curl == NULL || base_url == NULL || mailbox == NULL || search_filter == NULL) {
        logger_error("perform_mailbox_search received invalid input");
        return -1;
    }

    if (snprintf(effective_url, sizeof(effective_url), "%s/%s", base_url, mailbox) < 0) {
        logger_error("failed to build effective IMAP URL");
        return -1;
    }

    if (snprintf(command, sizeof(command), "SEARCH %s", search_filter) < 0) {
        logger_error("failed to build IMAP SEARCH command");
        return -1;
    }

    error_buffer[0] = '\0';
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
    curl_easy_setopt(curl, CURLOPT_URL, effective_url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, command);

    snprintf(message, sizeof(message), "effective imap_url=%s mailbox=%s command=%s", effective_url, mailbox, command);
    logger_info(message);

    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        if (error_buffer[0] != '\0') {
            snprintf(message, sizeof(message),
                     "curl_easy_perform failed mailbox=%s code=%d error=%s details=%s",
                     mailbox,
                     (int)result,
                     curl_easy_strerror(result),
                     error_buffer);
        } else {
            snprintf(message, sizeof(message),
                     "curl_easy_perform failed mailbox=%s code=%d error=%s",
                     mailbox,
                     (int)result,
                     curl_easy_strerror(result));
        }
        logger_error(message);
        return -1;
    }

    snprintf(message, sizeof(message), "IMAP SEARCH completed mailbox=%s", mailbox);
    logger_info(message);
    return 0;
}

int imap_client_fetch(const AppConfig *config)
{
    CURL *curl;
    int index;

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
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);

    if (logger_is_debug_enabled()) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curl_trace);
    }

    if (config->mapping_count > 0) {
        for (index = 0; index < config->mapping_count; index++) {
            if (perform_mailbox_search(curl,
                                       config->imap_url,
                                       config->mappings[index].mailbox,
                                       config->mappings[index].search_filter) != 0) {
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                return -1;
            }
        }
    } else {
        if (perform_mailbox_search(curl, config->imap_url, config->mailbox, config->search_filter) != 0) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return -1;
        }
    }

    logger_info("IMAP fetch routine completed");

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}
