#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "log_http_server.h"

static int make_log_path(char *buffer, size_t size, const char *log_dir)
{
    char file_name[64];
    time_t now;
    struct tm tm_info;

    if (buffer == NULL || size == 0U || log_dir == NULL || log_dir[0] == '\0') {
        return -1;
    }

    now = time(NULL);
    {
        struct tm *tmp = localtime(&now);
        if (tmp == NULL) {
            return -1;
        }
        tm_info = *tmp;
    }

    if (strftime(file_name, sizeof(file_name), "%Y-%m-%d.log", &tm_info) == 0U) {
        return -1;
    }

    if (snprintf(buffer, size, "%s/%s", log_dir, file_name) < 0) {
        return -1;
    }

    return 0;
}

static void send_response(int client_fd, int code, const char *status, const char *content_type, const char *body)
{
    char header[512];
    int body_len;

    if (body == NULL) {
        body = "";
    }

    body_len = (int)strlen(body);
    snprintf(header,
             sizeof(header),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n\r\n",
             code,
             status,
             content_type,
             body_len);

    (void)send(client_fd, header, strlen(header), 0);
    if (body_len > 0) {
        (void)send(client_fd, body, body_len, 0);
    }
}

static void handle_logs_request(int client_fd, const char *log_dir)
{
    char path[1024];
    FILE *file;
    char *buffer;
    long size;

    if (make_log_path(path, sizeof(path), log_dir) != 0) {
        send_response(client_fd, 500, "Internal Server Error", "text/plain", "failed to resolve log file path\n");
        return;
    }

    file = fopen(path, "r");
    if (file == NULL) {
        send_response(client_fd, 404, "Not Found", "text/plain", "log file not found\n");
        return;
    }

    if (fseek(file, 0L, SEEK_END) != 0) {
        fclose(file);
        send_response(client_fd, 500, "Internal Server Error", "text/plain", "failed to read log file\n");
        return;
    }

    size = ftell(file);
    if (size < 0 || fseek(file, 0L, SEEK_SET) != 0) {
        fclose(file);
        send_response(client_fd, 500, "Internal Server Error", "text/plain", "failed to read log file\n");
        return;
    }

    buffer = malloc((size_t)size + 1U);
    if (buffer == NULL) {
        fclose(file);
        send_response(client_fd, 500, "Internal Server Error", "text/plain", "out of memory\n");
        return;
    }

    if (size > 0 && fread(buffer, 1U, (size_t)size, file) != (size_t)size) {
        free(buffer);
        fclose(file);
        send_response(client_fd, 500, "Internal Server Error", "text/plain", "failed to read log file\n");
        return;
    }

    buffer[size] = '\0';
    send_response(client_fd, 200, "OK", "text/plain; charset=utf-8", buffer);

    free(buffer);
    fclose(file);
}

int log_http_server_run(const char *log_dir, int port)
{
    int server_fd;
    struct sockaddr_in address;

    if (log_dir == NULL || log_dir[0] == '\0' || port <= 0 || port > 65535) {
        return -1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        return -1;
    }

    {
        int enable = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) != 0) {
            close(server_fd);
            return -1;
        }
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons((unsigned short)port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) != 0) {
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 10) != 0) {
        close(server_fd);
        return -1;
    }

    for (;;) {
        int client_fd;
        char request[1024];
        ssize_t bytes;

        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }

        bytes = recv(client_fd, request, sizeof(request) - 1, 0);
        if (bytes <= 0) {
            close(client_fd);
            continue;
        }

        request[bytes] = '\0';

        if (strncmp(request, "GET /logs ", 10) == 0 || strncmp(request, "GET /logs?", 10) == 0) {
            handle_logs_request(client_fd, log_dir);
        } else if (strncmp(request, "GET /health ", 12) == 0) {
            send_response(client_fd, 200, "OK", "text/plain", "ok\n");
        } else {
            send_response(client_fd, 404, "Not Found", "text/plain", "route not found\n");
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
