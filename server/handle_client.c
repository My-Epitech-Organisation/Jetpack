/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-myftp-santiago.pidcova
** File description:
** handle_client
*/

#include "includes/server.h"

void accept_client(server_t *server)
{
    client_t *new_client = malloc(sizeof(client_t));

    new_client->addr_len = sizeof(new_client->addr);
    new_client->fd = accept(server->fd, (struct sockaddr *) &new_client->addr,
        &new_client->addr_len);
    if (new_client->fd == -1) {
        perror("accept");
        free(new_client);
        return;
    }
    server->client = realloc(server->client,
        (server->client_count + 1) * sizeof(client_t *));
    server->client[server->client_count] = new_client;
    server->fds[server->nfds].fd = new_client->fd;
    server->fds[server->nfds].events = POLLIN;
    server->nfds++;
    put_str_fd(new_client->fd, "ID ");
    char buffer[32];
    sprintf(buffer, "%d", server->client_count);
    server->client_count++;
    put_str_fd(new_client->fd, buffer);
    put_str_fd(new_client->fd, "\n");
}

void loop_clients(server_t *server, int current_idx)
{
    if (server->fds[current_idx].revents & POLLIN) {
        if (server->fds[current_idx].fd == server->fd)
            accept_client(server);
    }
}

void handle_clients(server_t *server)
{
    server->client = NULL;
    server->client_count = 0;
    server->fds = malloc(sizeof(struct pollfd) * MAX_CLIENTS + 1);
    server->nfds = 1;
    server->fds[0].fd = server->fd;
    server->fds[0].events = POLLIN;
    while (1) {
        if (poll(server->fds, server->nfds, -1) == -1)
            handle_error("poll");
        for (int current_idx = 0; current_idx < server->nfds; current_idx++)
            loop_clients(server, current_idx);
    }
}
