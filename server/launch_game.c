/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** launch_game
*/

#include "includes/server.h"

void launch_game(server_t *server)
{
    for (int i = 0; i < server->client_count; i++) {
        send_map(server, server->client[i]->fd);
        send_game_start(server, server->client[i]->fd);
    }
}
