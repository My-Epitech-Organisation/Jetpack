/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** read_client
*/

#include "includes/server.h"

void handle_message(server_t *server, int client_id, uint16_t length)
{
    switch (server->message_type) {
        case CLIENT_CONNECT:
            send_welcome(server->client[client_id]->fd, client_id, server);
            if (server->client_count == MAX_CLIENTS)
                launch_game(server);
            break;
        case GAME_INPUT:
            handle_input(server, client_id, server->buffer, length);
            break;
        default:
            printf("Unknown message (type: 0x%02X) from client %d\n",
                server->message_type, client_id);
            break;
    }
}

void read_client(server_t *server, int i)
{
    unsigned char header[4];
    ssize_t read_ret;
    uint16_t payload_length;
    char *payload;

    read_ret = read(server->client[i]->fd, header, 4);
    if (read_ret <= 0 || !check_header((unsigned char *)header, i, server))
        return;
    payload_length = ntohs(*(uint16_t *)(header + 2));
    if (check_payload_length(payload_length, server, i) == 84)
        return;
    payload = malloc((payload_length - 4) * sizeof(char));
    if (!payload)
        handle_error("malloc");
    read_ret = read(server->client[i]->fd, payload, payload_length - 4);
    if (check_read(read_ret, payload_length, payload) == 84)
        return;
    print_debug_all(server, "Server", payload, header);
    handle_message(server, i, payload_length - 4);
    free(payload);
}
