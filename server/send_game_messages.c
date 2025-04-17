/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** send_game_messages
*/

#include "includes/server.h"

void send_game_start(server_t *server, int client_fd)
{
    uint8_t buffer[9];
    uint16_t length = 9;  // Raw length, not converted to network byte order
    ssize_t bytes_sent;

    write_header(buffer, GAME_START, length);
    write_start_payload(buffer, server);
    bytes_sent = send(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_sent == -1) {
        perror("send");
    }
}

void send_game_state(server_t *server, int client_fd)
{
    size_t player_data_size = server->client_count * 8;
    uint16_t total_payload_size = 4 + 1 + player_data_size;
    uint16_t total_msg_size = 4 + total_payload_size;
    uint8_t *buffer = malloc(total_msg_size);
    size_t offset;
    client_t *client;

    if (!buffer)
        handle_error("malloc");
    write_header(buffer, GAME_STATE, total_msg_size);
    write_state_payload(buffer, server, server->client_count);
    offset = 9;
    for (int i = 0; i < server->client_count; i++) {
        client = server->client[i];
        write_data_state_payload(buffer, client, offset, i);
        offset += 9;  // Move to the next player's data block
    }
    if (send(client_fd, buffer, total_msg_size, 0) == -1)
        perror("send GAME_STATE");
    free(buffer);
}

void send_game_state_to_all_clients(server_t *server)
{
    for (int i = 0; i < server->client_count; i++)
        send_game_state(server, server->client[i]->fd);
}
