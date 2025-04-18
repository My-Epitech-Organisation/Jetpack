/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Main server implementation
*/

#include "includes/server.h"

void close_everything(server_t *server)
{
    for (int i = 0; i < server->client_count; i++) {
        close(server->client[i]->fd);
        free(server->client[i]);
        server->client_count--;
    }
    free(server);
}

void server(int argc, char **argv)
{
    server_t *server = malloc(sizeof(server_t));

    if (server == NULL)
        handle_error("malloc");
    parsing_launch(argc, argv, server);
    server->fd = set_server_socket();
    server->client_count = 0;
    server->start_x = 2;
    server->start_y = 2;
    set_bind(server);
    set_listen(server->fd);
    load_map(server);
    handle_clients(server);
    launch_game(server);
    close_everything(server);
}
