/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** handle_input_from_clients
*/

#include "includes/server.h"

void handle_input(server_t *server, int client_id, char *payload)
{
    uint8_t player_id;
    uint8_t jetpack_status;

    if (!payload || server->client[client_id] == NULL)
        return;
    player_id = payload[0];
    jetpack_status = payload[1];
    if (jetpack_status == 1)
        server->client[player_id]->jetpack = true;
    else if (jetpack_status == 0)
        server->client[player_id]->jetpack = false;
    else
        fprintf(stderr, "Invalid jetpack status: %d\n", jetpack_status);
}
