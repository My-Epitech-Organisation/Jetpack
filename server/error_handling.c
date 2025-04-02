/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-myftp-santiago.pidcova
** File description:
** error_handling
*/

#include "includes/server.h"

void handle_error(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int arg_missing(int argc)
{
    if (argc < 5) {
        printf("Invalid number of arguments.\n");
        return 84;
    }
    return 0;
}
