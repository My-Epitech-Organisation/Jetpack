/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** game_loop
*/

#include "includes/server.h"

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
