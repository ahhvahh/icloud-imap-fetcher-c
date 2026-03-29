#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "attachment_saver.h"

int attachment_saver_prepare_directory(const char *path)
{
    char buffer[1024];
    size_t index;

    if (path == NULL || path[0] == '\0') {
        return -1;
    }

    if (strlen(path) >= sizeof(buffer)) {
        return -1;
    }

    strcpy(buffer, path);

    for (index = 1U; buffer[index] != '\0'; index++) {
        if (buffer[index] == '/') {
            buffer[index] = '\0';
            if (buffer[0] != '\0' && mkdir(buffer, 0755) != 0 && errno != EEXIST) {
                return -1;
            }
            buffer[index] = '/';
        }
    }

    if (mkdir(buffer, 0755) != 0 && errno != EEXIST) {
        return -1;
    }

    return 0;
}
