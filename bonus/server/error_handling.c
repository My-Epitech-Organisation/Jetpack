/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Error handling functions
*/

#include "includes/server.h"

void handle_error(char *msg, server_t *server)
{
    send_disconnect(server);
    close_everything(server);
    perror(msg);
    exit(EXIT_FAILURE);
}

int arg_missing(int argc)
{
    if (argc < 5) {
        printf("Invalid number of arguments.\n");
        return 84;
    }
    return 0;
}

bool check_header(unsigned char header[4], int i, server_t *server)
{
    if ((unsigned char)header[0] != 0xAB) {
        close(server->client[i]->fd);
        return false;
    }
    server->message_type = (uint8_t)header[1];
    return true;
}

int check_payload_length(uint16_t payload_length, server_t *server, int i)
{
    if (payload_length < 4) {
        close(server->client[i]->fd);
        return 84;
    }
    return 0;
}

int check_read(int read_ret, char *payload)
{
    if (read_ret <= 0) {
        free(payload);
        return 84;
    }
    return 0;
}
