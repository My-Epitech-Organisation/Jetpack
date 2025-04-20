/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** game_loop
*/

#include "includes/server.h"

void check_limits(client_t *client)
{
    if (client->x >= 1000)
        client->x = 1000 - 1;
    if (client->x <= 0)
        client->x = 0;
    if (client->y >= 1000)
        client->y = 1000 - 1;
    if (client->y <= 0)
        client->y = 0;
}

void check_coins(client_t *client, server_t *server)
{
    size_t row = client->y * server->map_rows / 1000;
    size_t col = client->x * server->map_cols / 1000;

    if (row >= server->map_rows || col >= server->map_cols)
        return;
    if (server->map[row][col] == 'c') {
        client->score++;
        printf("Client %d collected a coin! Score: %d\n",
            client->fd, client->score);
        server->map[row][col] = '_';
        client->collected_coin = true;
    }
}

void check_laser(client_t *client, server_t *server)
{
    size_t row = client->y * server->map_rows / 1000;
    size_t col = client->x * server->map_cols / 1000;

    if (row >= server->map_rows || col >= server->map_cols)
        return;
    if (server->map[row][col] == 'e') {
        printf("Client %d touched a laser! Score: %d\n",
            client->fd, client->score);
        server->map[row][col] = '_';
        client->is_alive = false;
    }
}

void update_game_state(server_t *server)
{
    client_t *client;
    bool player_died = false;
    uint8_t alive_player_id = 0xFF; // No winner by default
    int alive_count = 0;

    // Comptage initial des joueurs en vie
    for (int i = 0; i < server->client_count; i++) {
        if (server->client[i]->is_alive) {
            alive_count++;
            alive_player_id = i; // Mémoriser le dernier joueur en vie trouvé
        }
    }

    // Si un seul joueur est en vie au début de la mise à jour, la partie est déjà terminée
    if (alive_count <= 1 && server->client_count > 1) {
        printf("Only one player left alive! Game over. Winner: Player %d\n", alive_player_id);
        send_game_end(server, 2, alive_player_id); // 2 = PLAYER_DIED
        return;
    }

    // Réinitialiser le compteur pour la mise à jour
    alive_count = 0;

    for (int i = 0; i < server->client_count; i++) {
        read_client(server, i);
        client = server->client[i];
        client->collected_coin = false;
        
        // Si un joueur était déjà mort avant cette mise à jour
        if (!client->is_alive)
            continue;
            
        // Mise à jour de la position du joueur
        if (client->jetpack) {
            client->y = client->y < 40 ? 0 :
            client->y - (5 * 100 / server->map_rows);
            printf("Client %d is using jetpack! y = %d : %ld\n", client->fd, client->y, (5 * server->map_rows / 1000));
        } else
            client->y += (3 * 100 / server->map_rows);
        client->x += (5 * 100 / server->map_cols);
        
        // Vérification des collisions
        check_limits(client);
        check_coins(client, server);
        check_laser(client, server);
        
        // Vérifier si le joueur vient de mourir
        if (!client->is_alive) {
            player_died = true;
        } else {
            alive_count++;
            alive_player_id = i;
        }
    }
    
    // Si un joueur est mort OU s'il ne reste qu'un seul joueur en vie après les mises à jour
    if (player_died || (alive_count == 1 && server->client_count > 1)) {
        // Envoyer un message GAME_END à tous les clients
        printf("Game over! Player %d is the last one standing. Winner: Player %d\n", 
               alive_player_id, alive_player_id);
        send_game_end(server, 2, alive_player_id); // 2 = PLAYER_DIED
    }
}

void game_loop(server_t *server)
{
    while (1) {
        usleep(50000);
        update_game_state(server);
        send_game_state_to_all_clients(server);
        server->tick++;
    }
}
