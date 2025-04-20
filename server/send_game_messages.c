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
    uint16_t length = 9;

    write_header(buffer, GAME_START, length);
    write_start_payload(buffer, server);
    if (!send_with_write(client_fd, buffer, sizeof(buffer)))
        perror("send_with_write GAME_START");
    print_debug_info_package_sent(server, get_type_string_prev(buffer[1]),
        buffer, sizeof(buffer));
}

void send_game_state(server_t *server, int client_fd)
{
    size_t player_data_size = server->client_count * 9;
    uint16_t total_payload_size = 4 + 1 + player_data_size;
    uint16_t total_msg_size = 4 + total_payload_size;
    uint8_t *buffer = malloc(total_msg_size);
    size_t offset;

    if (!buffer)
        handle_error("malloc", server);
    write_header(buffer, GAME_STATE, total_msg_size);
    write_state_payload(buffer, server, server->client_count);
    offset = 9;
    for (int i = 0; i < server->client_count; i++) {
        write_data_state_payload(buffer, server->client[i], offset, i);
        offset += 9;
    }
    if (!send_with_write(client_fd, buffer, total_msg_size))
        perror("send_with_write GAME_STATE");
    print_debug_info_package_sent(server, get_type_string_prev(buffer[1]),
        buffer, sizeof(buffer));
    free(buffer);
}

void send_game_state_to_all_clients(server_t *server)
{
    for (int i = 0; i < server->client_count; i++)
        send_game_state(server, server->client[i]->fd);
}

void send_game_end(server_t *server, uint8_t reason, uint8_t winner_id)
{
    uint8_t buffer[6]; // 4 octets d'en-tête + 1 octet de raison + 1 octet d'ID de gagnant
    uint16_t length = 6;

    // Écriture de l'en-tête
    write_header(buffer, GAME_END, length);
    
    // Ajout de la raison et de l'ID du gagnant au payload
    buffer[4] = reason;
    buffer[5] = winner_id;
    
    // Envoi du message à tous les clients
    for (int i = 0; i < server->client_count; i++) {
        if (!send_with_write(server->client[i]->fd, buffer, length))
            perror("send_with_write GAME_END");
        print_debug_info_package_sent(server, get_type_string_prev(buffer[1]),
            buffer, length);
    }
    
    printf("Game end message sent. Reason: %d, Winner: %d\n", reason, winner_id);
}
