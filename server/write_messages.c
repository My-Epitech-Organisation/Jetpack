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
