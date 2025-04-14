/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** send_messages_to_clients
*/

#include "includes/server.h"

void write_header(uint8_t *buf, uint8_t type, uint16_t total_len)
{
    buf[0] = MAGIC_BYTE;
    buf[1] = type;
    buf[2] = (total_len >> 8) & 0xFF;
    buf[3] = total_len & 0xFF;
}

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
