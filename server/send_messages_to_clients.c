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

    buffer[0] = MAGIC_BYTE;
    buffer[1] = SERVER_WELCOME;
    buffer[2] = (length >> 8) & 0xFF;
    buffer[3] = length & 0xFF;
    buffer[4] = 1;
    buffer[5] = assigned_id;
    bytes_sent = send(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_sent == -1) {
        perror("send");
    }
}
