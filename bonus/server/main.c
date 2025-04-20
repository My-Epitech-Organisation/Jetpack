/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Server entry point
*/

#include "includes/server.h"

void display_help(void)
{
    printf("USAGE: ./jetpack_server -p <port> -m <map> [-d] [-c <nb_user>]\n");
}

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    if (argc == 1) {
        display_help();
        return 84;
    }
    if (strcmp(argv[1], "-help") == 0) {
        display_help();
        return 0;
    }
    if (arg_missing(argc) != 0 || check_args(argc, argv) != 0)
        return 84;
    server(argc, argv);
    return 0;
}
