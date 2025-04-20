/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** check_in_game
*/

#include "includes/server.h"


void check_limits(client_t *client)
{
    if (client->x >= 1000)
        client->x = 1000 - 1;
    if (client->x <= 0)
        client->x = 0;
    if (client->y >= 1000)
        client->y = 1000 - 1;
    if (client->y <= 0)
        client->y = 0;
}

void check_entities_colisions(client_t *client, server_t *server)
{
    size_t row = client->y * server->map_rows / 1000;
    size_t col = client->x * server->map_cols / 1000;

    if (row >= server->map_rows || col >= server->map_cols)
        return;
    if (server->map[row][col] == 'c') {
        client->score++;
        server->map[row][col] = '_';
        client->collected_coin = true;
    }
    if (server->map[row][col] == 'e') {
        server->map[row][col] = '_';
        client->is_alive = false;
    }
}

bool check_alive_begin(server_t *server, int *alive_count,
    uint8_t *alive_player_id)
{
    for (int i = 0; i < server->client_count; i++) {
        if (server->client[i]->is_alive) {
            (*alive_count)++;
            *alive_player_id = i;
        }
    }
    if (*alive_count <= 1 && server->client_count > 1) {
        send_game_end(server, 2, *alive_player_id);
        return false;
    }
    *alive_count = 0;
    return true;
}

void check_alive_end(client_t *client, int *alive_count,
    uint8_t *alive_player_id, int i)
{
    if (client->is_alive) {
        (*alive_count)++;
        *alive_player_id = i;
    }
}

void check_jetpack(client_t *client, server_t *server)
{
    if (client->jetpack) {
        client->y = client->y < 40 ? 0 :
        client->y - (5 * 100 / server->map_rows);
    } else
        client->y += (3 * 100 / server->map_rows);
    client->x += (5 * 100 / server->map_cols);
}
