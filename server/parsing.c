/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Command line argument parsing
*/

#include "includes/server.h"

void parsing_launch(int argc, char **argv, server_t *server)
{
    if (argc == 6 && strcmp(argv[5], "-d") == 0) {
        server->debug_mode = true;
        printf("Debug mode enabled - ");
        printf("verbose protocol logging will be displayed\n");
        printf("Debug logging initialized\n");
    } else
        server->debug_mode = false;
    server->port = atoi(argv[2]);
    server->map_path = argv[4];
    if (server->debug_mode)
        print_debug_info_connection(server, "Main");
}
