/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** String output utilities
*/

#include "includes/server.h"

int strlen_fd(char *str)
{
    int len = 0;

    if (!str)
        return 0;
    while (str[len] != '\0')
        len++;
    return len;
}

void put_str_fd(int fd, char *str)
{
    int len = 0;
    int write_ret = 0;

    if (!str)
        return;
    len = strlen_fd(str);
    write_ret = write(fd, str, len);
    if (write_ret != len)
        put_str_fd(fd, str + write_ret);
}
