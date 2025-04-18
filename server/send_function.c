/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** send_function
*/

#include "includes/server.h"
#include <unistd.h>

bool send_with_write(int fd, const void *buffer, size_t length)
{
    const char *buf = (const char *)buffer;
    size_t total_written = 0;
    ssize_t bytes_written;

    while (total_written < length) {
        bytes_written = write(fd, buf + total_written, length - total_written);
        if (bytes_written == -1 || bytes_written == 0)
            return false;
        total_written += bytes_written;
    }
    return true;
}
