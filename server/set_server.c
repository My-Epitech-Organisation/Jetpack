/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Server socket setup functions
*/

#include "includes/server.h"


int set_server_socket(server_t *server)
{
    int fd = 0;
    int opt = 1;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        handle_error("setsockopt", server);
    if (fd == -1)
        handle_error("socket", server);
    return fd;
}

void set_bind(server_t *server)
{
    memset(&server->addr, 0, sizeof(server->addr));
    server->addr.sin_family = AF_INET;
    server->addr.sin_port = htons(server->port);
    server->addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server->fd, (struct sockaddr *) &server->addr,
        sizeof(server->addr)) == -1)
        handle_error("bind", server);
}

void set_listen(server_t *server)
{
    if (listen(server->fd, 50) == -1)
        handle_error("listen", server);
}
