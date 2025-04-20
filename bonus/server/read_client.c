/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** read_client
*/

#include "includes/server.h"

void handle_message(server_t *server, int client_id, char *payload)
{
    switch (server->message_type) {
        case CLIENT_CONNECT:
            send_welcome(server, server->client[client_id]->fd, client_id);
            if (server->client_count == server->max_client)
                launch_game(server);
            break;
        case GAME_INPUT:
            handle_input(server, client_id, payload);
            break;
        case CLIENT_DISCONNECT:
            handle_error("Client disconnected", server);
            break;
        default:
            break;
    }
}

ssize_t read_all(int fd, char *buffer, size_t size)
{
    size_t total_read = 0;
    ssize_t bytes_read;

    while (total_read < size) {
        bytes_read = read(fd, buffer + total_read, size - total_read);
        if (bytes_read <= 0)
            return bytes_read;
        total_read += bytes_read;
    }
    return total_read;
}

void read_client(server_t *server, int i)
{
    unsigned char header[4];
    ssize_t read_ret;
    uint16_t payload_length;
    char *payload;

    read_ret = read_all(server->client[i]->fd, (char *)header, 4);
    if (read_ret <= 0 || !check_header((unsigned char *)header, i, server))
        return;
    payload_length = ntohs(*(uint16_t *)(header + 2));
    if (check_payload_length(payload_length, server, i) == 84)
        return;
    payload = malloc((payload_length - 4) * sizeof(char));
    if (!payload)
        handle_error("malloc", server);
    read_ret = read_all(server->client[i]->fd, payload, payload_length - 4);
    if (check_read(read_ret, payload) == 84)
        return;
    print_debug_all(server, "Server", payload, header);
    handle_message(server, i, payload);
    free(payload);
}
