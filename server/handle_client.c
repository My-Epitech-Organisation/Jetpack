/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Client connection handling
*/

#include "includes/server.h"

client_t *set_values_to_client(client_t *new_client, server_t *server)
{
    new_client->score = 0;
    new_client->is_alive = true;
    new_client->x = server->start_x * 100;
    new_client->y = server->start_y * 100;
    new_client->input_left = false;
    new_client->input_right = false;
    new_client->input_jetpack = false;
    return new_client;
}

void accept_client(server_t *server)
{
    client_t *new_client = malloc(sizeof(client_t));

    if (!new_client)
        handle_error("malloc");
    memset(new_client, 0, sizeof(client_t));
    new_client->addr_len = sizeof(new_client->addr);
    new_client->fd = accept(server->fd, (struct sockaddr *) &new_client->addr,
        &new_client->addr_len);
    if (new_client->fd == -1) {
        perror("accept");
        free(new_client);
        return;
    }
    new_client = set_values_to_client(new_client, server);
    server->client = realloc(server->client,
        (server->client_count + 1) * sizeof(client_t *));
    server->client[server->client_count] = new_client;
    server->fds[server->nfds].fd = new_client->fd;
    server->fds[server->nfds].events = POLLIN;
    server->nfds++;
}

void check_read_client(server_t *server, int current_idx)
{
    int client_idx = -1;

    for (int j = 0; j < server->client_count; j++) {
        if (server->client[j]->fd == server->fds[current_idx].fd) {
            client_idx = j;
            break;
        }
    }
    if (client_idx != -1)
        read_client(server, client_idx);
}

void loop_clients(server_t *server, int current_idx)
{
    if (server->fds[current_idx].revents & POLLIN) {
        if (server->fds[current_idx].fd == server->fd &&
            server->client_count < MAX_CLIENTS) {
            accept_client(server);
            server->client_count++;
        } else
            check_read_client(server, current_idx);
    }
}

void handle_clients(server_t *server)
{
    server->client = calloc(MAX_CLIENTS, sizeof(client_t *));
    server->client_count = 0;
    server->fds = malloc(sizeof(struct pollfd) * (MAX_CLIENTS + 1));
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
