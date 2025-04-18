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
    uint16_t length = 4 + 2;

    write_header(buffer, SERVER_WELCOME, length);
    buffer[4] = 1;
    buffer[5] = assigned_id;
    if (!send_with_write(client_fd, buffer, sizeof(buffer)))
        perror("send_with_write SERVER_WELCOME");
}

void send_map_chunk(server_t *server, int client_fd, uint8_t col_index)
{
    size_t total_size;
    uint8_t *buffer;

    if (col_index >= server->map_cols)
        return;
    total_size = 4 + (4 + server->map_rows);
    buffer = malloc(total_size);
    if (!buffer)
        handle_error("malloc");
    write_header(buffer, MAP_CHUNK, 4 + 4 + server->map_rows);
    write_map_payload(buffer, col_index, (uint16_t)server->map_cols);
    for (size_t row = 0; row < server->map_rows; row++)
        buffer[8 + row] = server->map[row][col_index];
    if (!send_with_write(client_fd, buffer, total_size))
        perror("send_with_write MAP_CHUNK");
    free(buffer);
}

void send_map(server_t *server, int client_fd)
{
    for (uint8_t col_index = 0; col_index < server->map_cols; col_index++)
        send_map_chunk(server, client_fd, col_index);
}
