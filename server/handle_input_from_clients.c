/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** handle_input_from_clients
*/

#include "includes/server.h"

void handle_input(server_t *server, int client_id, char *payload, uint16_t len)
{
    uint8_t pid;
    uint8_t mask;
    client_t *client;
    bool left;
    bool right;
    bool jetpack;

    if (len < 2)
        return;
    pid = payload[0];
    mask = payload[1];
    left = mask & (1 << 0);
    right = mask & (1 << 1);
    jetpack = mask & (1 << 2);
    client = server->client[client_id];
    client->input_left = left;
    client->input_right = right;
    client->input_jetpack = jetpack;
    printf("Client %d input: L:%d R:%d J:%d\n", pid, left, right, jetpack);
}
