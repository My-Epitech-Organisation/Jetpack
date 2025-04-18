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
    ssize_t bytes_written = write(fd, buffer, length);

    if (bytes_written == -1)
        return false;
    return true;
}
