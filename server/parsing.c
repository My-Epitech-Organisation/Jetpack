/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-jetpack-santiago.pidcova
** File description:
** parsing
*/

#include "includes/server.h"

void parsing_launch(int argc, char **argv, server_t *server)
{
    if (argc == 6 && strcmp(argv[5], "-d") == 0)
        server->debug_mode = true;
    else
        server->debug_mode = false;
    server->port = atoi(argv[2]);
    server->map_path = argv[4];
    if (server->debug_mode) {
        printf("Debug mode activated\n");
        printf("Port: %d\n", server->port);
        printf("Map: %s\n", server->map_path);
    }
}
