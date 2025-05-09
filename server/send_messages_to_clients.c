/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** send_messages_to_clients
*/

#include "includes/server.h"

void send_welcome(server_t *server, int client_fd, uint8_t assigned_id)
{
    uint8_t buffer[4 + 2];
    uint16_t length = 4 + 2;

    write_header(buffer, SERVER_WELCOME, length);
    buffer[4] = 1;
    buffer[5] = assigned_id;
    if (!send_with_write(client_fd, buffer, sizeof(buffer)))
        handle_error("send_with_write SERVER_WELCOME", server);
    print_debug_info_package_sent(server, get_type_string_prev(buffer[1]),
        buffer, sizeof(buffer));
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
        handle_error("malloc", server);
    write_header(buffer, MAP_CHUNK, 4 + 4 + server->map_rows);
    write_map_payload(buffer, col_index, (uint16_t)server->map_cols);
    for (size_t row = 0; row < server->map_rows; row++)
        buffer[8 + row] = server->map[row][col_index];
    if (!send_with_write(client_fd, buffer, total_size))
        handle_error("send_with_write MAP_CHUNK", server);
    print_debug_info_package_sent(server, get_type_string_prev(buffer[1]),
        buffer, total_size);
    free(buffer);
}

void send_map(server_t *server, int client_fd)
{
    for (uint8_t col_index = 0; col_index < server->map_cols; col_index++)
        send_map_chunk(server, client_fd, col_index);
}

void send_disconnect(server_t *server)
{
    uint8_t buffer[4 + 1];
    uint16_t length = 4 + 1;

    write_header(buffer, CLIENT_DISCONNECT, length);
    buffer[4] = 0;
    for (int i = 0; i < server->client_count; i++) {
        if (server->client[i] == NULL)
            continue;
        if (!send_with_write(server->client[i]->fd, buffer, sizeof(buffer)))
            handle_error("send_with_write CLIENT_DISCONNECT", server);
        print_debug_info_package_sent(server,
            get_type_string_prev(buffer[1]), buffer, sizeof(buffer));
    }
}
