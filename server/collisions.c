/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** collisions
*/

#include "includes/server.h"

bool is_in_bounds(server_t *server, size_t row, size_t col)
{
    return row < server->map_rows && col < server->map_cols;
}

void handle_coin(client_t *client, server_t *server, size_t row, size_t col)
{
    client->score++;
    server->map[row][col] = 'd';
    client->collected_coin = true;
}

void handle_doin(client_t *client, server_t *server, size_t row, size_t col)
{
    client->score++;
    server->map[row][col] = '_';
    client->collected_coin = true;
}

void handle_electic(client_t *client)
{
    client->is_alive = false;
}
