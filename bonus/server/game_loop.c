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
    uint8_t alive_player_id = 0xFF;
    int alive_count = 0;

    if (!initialize_alive_tracking(server, &alive_count, &alive_player_id))
        return;
    for (int i = 0; i < server->client_count; i++) {
        client = server->client[i];
        read_client(server, i);
        process_client_state(client, server, alive_player_id);
        if (client->is_alive) {
            alive_count++;
            alive_player_id = i;
        }
    }
    if (!client->is_alive || (alive_count == 1 && server->client_count > 1))
        send_game_end(server, 2, alive_player_id);
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
