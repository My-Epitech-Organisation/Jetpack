/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** game_loop
*/

#include "includes/server.h"

void check_limits(client_t *client, server_t *server)
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

void check_coins(client_t *client, server_t *server)
{
    size_t row = client->y / 100;
    size_t col = client->x / 100;

    if (row >= server->map_rows || col >= server->map_cols)
        return;
    if (server->map[row][col] == 'c') {
        client->score++;
        printf("Client %d collected a coin! Score: %d\n",
            client->fd, client->score);
        server->map[row][col] = '_';
        client->collected_coin = true;
    }
}

void update_game_state(server_t *server)
{
    client_t *client;

    for (int i = 0; i < server->client_count; i++) {
        read_client(server, i);
        client = server->client[i];
        client->collected_coin = false;
        if (!client->is_alive)
            continue;
        if (client->jetpack) {
            printf("Client %d is using jetpack!\n", client->fd);
            client->y = client->y < 40 ? 0 : client->y - 40;
        } else
            client->y += 50;
        client->x += 50;
        check_limits(client, server);
        check_coins(client, server);
    }
}

void game_loop(server_t *server)
{
    while (1) {
        usleep(50000);
        update_game_state(server);
        send_game_state_to_all_clients(server);
        server->tick++;
    }
}
