/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** send_messages_to_clients
*/

#include "includes/server.h"

void send_welcome(int client_fd, uint8_t assigned_id)
{
    uint8_t buffer[4 + 2];
    uint16_t length = htons(4 + 2);
    ssize_t bytes_sent;

    write_header(buffer, SERVER_WELCOME, length);
    buffer[4] = 1;
    buffer[5] = assigned_id;
    bytes_sent = send(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_sent == -1) {
        perror("send");
    }
}

void send_map_chunk(server_t *server, int client_fd, uint8_t col_index)
{
    size_t total_size;
    uint8_t *buffer;
    ssize_t bytes_sent;

    if (col_index >= server->map_cols)
        return;
    total_size = 4 + (4 + server->map_rows);
    buffer = malloc(total_size);
    if (!buffer)
        handle_error("malloc");
    write_header(buffer, MAP_CHUNK, htons(4 + server->map_rows));
    write_map_payload(buffer, col_index, (uint16_t)server->map_cols);
    for (size_t row = 0; row < server->map_rows; row++)
        buffer[8 + row] = server->map[row][col_index];
    bytes_sent = send(client_fd, buffer, total_size, 0);
    if (bytes_sent == -1)
        perror("send");
    free(buffer);
}

void send_map(server_t *server, int client_fd)
{
    for (uint8_t col_index = 0; col_index < server->map_cols; col_index++)
        send_map_chunk(server, client_fd, col_index);
}

void send_game_start(server_t *server, int client_fd)
{
    uint8_t buffer[9];
    uint16_t length = 9;
    ssize_t bytes_sent;

    write_header(buffer, GAME_START, length);
    buffer[4] = server->client_count;
    buffer[5] = (server->start_x >> 8) & 0xFF;
    buffer[6] = server->start_x & 0xFF;
    buffer[7] = (server->start_y >> 8) & 0xFF;
    buffer[8] = server->start_y & 0xFF;
    bytes_sent = send(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_sent == -1) {
        perror("send");
    }
}
