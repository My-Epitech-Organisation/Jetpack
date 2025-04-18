/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** handle_input_from_clients
*/

#include "includes/server.h"

void handle_input(server_t *server, int client_id, char *payload, uint16_t len)
{
    uint8_t mask;
    client_t *client;
    bool left;
    bool right;
    bool jetpack;

    if (len < 2)
        return;
    mask = payload[1];
    left = mask & (1 << 0);
    right = mask & (1 << 1);
    jetpack = mask & (1 << 2);
    client = server->client[client_id];
    client->input_left = left;
    client->input_right = right;
    client->input_jetpack = jetpack;
}
