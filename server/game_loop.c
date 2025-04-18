/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** game_loop
*/

#include "includes/server.h"

void check_limits(client_t *client, server_t *server)
{
    if (client->x >= server->map_cols * 100)
        client->x = server->map_cols * 100 - 1;
    if (client->x <= 0)
        client->x = 0;

    if (client->y >= server->map_rows * 100)
        client->y = server->map_rows * 100 - 1;
    if (client->y <= 0)
        client->y = 0;
}

void update_game_state(server_t *server)
{
    client_t *client;

    for (int i = 0; i < server->client_count; i++) {
        client = server->client[i];
        if (!client->is_alive)
            continue;
        if (client->input_left)
            client->x--;
        if (client->input_right)
            client->x++;
        if (client->input_jetpack)
            client->y--;
        else
            client->y++;
        check_limits(client, server);
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
