/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-myftp-santiago.pidcova
** File description:
** server
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

    if (server == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    parsing_launch(argc, argv, server);
    server->fd = set_server_socket();
    server->client_count = 0;
    set_bind(server);
    set_listen(server->fd);
    handle_clients(server);
    close_everything(server);
}
