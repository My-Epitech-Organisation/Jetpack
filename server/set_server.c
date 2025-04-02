/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-myftp-santiago.pidcova
** File description:
** set_server
*/

#include "includes/server.h"


int set_server_socket(void)
{
    int fd = 0;
    int opt = 1;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        handle_error("setsockopt");
    if (fd == -1)
        handle_error("socket");
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
        handle_error("bind");
}

void set_listen(int fd)
{
    if (listen(fd, 50) == -1)
        handle_error("listen");
}
