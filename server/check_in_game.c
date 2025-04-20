/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** check_in_game
*/

#include "includes/server.h"


bool check_limits(client_t *client)
{
    if (client->x >= 1000) {
        client->x = 1000 - 1;
        return true;
    }
    if (client->x <= 0)
        client->x = 0;
    if (client->y >= 1000)
        client->y = 1000 - 1;
    if (client->y <= 0)
        client->y = 0;
    return false;
}

void check_entities_collisions(client_t *client, server_t *server)
{
    size_t row = client->y * server->map_rows / 1000;
    size_t col = client->x * server->map_cols / 1000;
    char entity;

    if (!is_in_bounds(server, row, col))
        return;
    entity = server->map[row][col];
    switch (entity) {
        case 'c':
            handle_coin(client, server, row, col);
            break;
        case 'd':
            handle_doin(client, server, row, col);
            break;
        case 'e':
            handle_electic(client);
            break;
        default:
            break;
    }
}

bool initialize_alive_tracking(server_t *server, int *alive_count,
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

void process_client_state(client_t *client, server_t *server,
    uint8_t alive_player_id)
{
    client->collected_coin = false;
    if (!client->is_alive)
        return;
    check_jetpack(client, server);
    if (check_limits(client))
        send_game_end(server, 1, alive_player_id);
    check_entities_collisions(client, server);
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
