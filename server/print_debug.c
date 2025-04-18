/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** print_debug
*/

#include "includes/server.h"
#include <time.h>

void print_packet_hex(const unsigned char *header,
    const unsigned char *payload, size_t payload_len)
{
    printf("Payload: ");
    for (int i = 0; i < 4; i++)
        printf("%02x ", header[i]);
    for (size_t i = 0; i < payload_len; i++)
        printf("%02x%s", payload[i], i == payload_len - 1 ? "" : " ");
    printf("\n");
}

void print_debug_info_connection(server_t *server, char *context)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buf[9];

    if (!server->debug_mode)
        return;
    strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);
    printf("[%s][%s] Connecting on the port %i \n",
        time_buf, context, server->port);
    fflush(stdout);
}

void print_debug_info_package(server_t *server, char *context,
    uint16_t payload_length)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buf[9];
    char *type = NULL;

    strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);
    type = get_type_string_prev(server->message_type);
    printf("[%s][%s] Received packet: type=0x%02X (%s), length=%u bytes\n",
        time_buf, context, server->message_type, type, payload_length);
    fflush(stdout);
}

void print_debug_all(server_t *server, char *context, char *payload,
    unsigned char *header)
{
    uint16_t payload_length;

    if (!server->debug_mode)
        return;
    payload_length = ntohs(*(uint16_t *)(header + 2));
    print_debug_info_package(server, context, payload_length);
    print_packet_hex(header, (unsigned char *)payload,
        payload_length - 4);
}
