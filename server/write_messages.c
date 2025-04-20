/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** write_messages
*/

#include "includes/server.h"

void write_header(uint8_t *buf, uint8_t type, uint16_t total_len)
{
    buf[0] = MAGIC_BYTE;
    buf[1] = type;
    buf[2] = (total_len >> 8) & 0xFF;
    buf[3] = total_len & 0xFF;
}

void write_map_payload(uint8_t *buffer, uint16_t chunk_index,
    uint16_t chunk_count)
{
    buffer[4] = (chunk_index >> 8) & 0xFF;
    buffer[5] = chunk_index & 0xFF;
    buffer[6] = (chunk_count >> 8) & 0xFF;
    buffer[7] = chunk_count & 0xFF;
}

void write_start_payload(uint8_t *buffer, server_t *server)
{
    buffer[4] = server->client_count;
    buffer[5] = (server->start_x >> 8) & 0xFF;
    buffer[6] = server->start_x & 0xFF;
    buffer[7] = (server->start_y >> 8) & 0xFF;
    buffer[8] = server->start_y & 0xFF;
}

void write_state_payload(uint8_t *buffer, server_t *server,
    uint8_t player_count)
{
    buffer[4] = (server->tick >> 24) & 0xFF;
    buffer[5] = (server->tick >> 16) & 0xFF;
    buffer[6] = (server->tick >> 8) & 0xFF;
    buffer[7] = server->tick & 0xFF;
    buffer[8] = player_count;
}

void write_data_state_payload(uint8_t *buffer, client_t *client, size_t offset,
    int i)
{
    buffer[offset] = i;
    buffer[offset + 1] = (client->x >> 8) & 0xFF;
    buffer[offset + 2] = client->x & 0xFF;
    buffer[offset + 3] = (client->y >> 8) & 0xFF;
    buffer[offset + 4] = client->y & 0xFF;
    buffer[offset + 5] = (client->score >> 8) & 0xFF;
    buffer[offset + 6] = client->score & 0xFF;
    buffer[offset + 7] = client->is_alive ? 1 : 0;
    buffer[offset + 8] = client->collected_coin ? 1 : 0;
}
