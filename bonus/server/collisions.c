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
    coin_t *coin;

    server->map[row][col] = 'd';
    client->collected_coin = true;
    for (size_t i = 0; i < server->coin_count; i++) {
        coin = &server->coins[i];
        if (coin->row == row && coin->col == col &&
            !coin->is_collected[client->fd]) {
            coin->is_collected[client->fd] = true;
            client->score++;
            client->collected_coin = true;
            break;
        }
    }
}

void handle_electic(client_t *client)
{
    client->is_alive = false;
}
