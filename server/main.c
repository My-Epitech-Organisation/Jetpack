/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-myftp-santiago.pidcova
** File description:
** main
*/

#include "includes/server.h"

void display_help(void)
{
    printf("USAGE: ./jetpack_server -p <port> -m <map> [-d]\n");
}

int main(int argc, char **argv)
{
    if (strcmp(argv[1], "-help") == 0) {
        display_help();
        return 0;
    }
    if (arg_missing(argc) != 0 || check_args(argc, argv) != 0)
        return 84;
    server(argc, argv);
    return 0;
}
