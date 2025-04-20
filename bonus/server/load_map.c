/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** load_map
*/

#include "includes/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void check_coins(server_t *server, size_t row, coin_t coin)
{
    for (size_t col = 0; col < server->map_cols; col++) {
        if (server->map[row][col] == 'c') {
            coin.row = row;
            coin.col = col;
            coin.is_collected = calloc(server->max_client, sizeof(bool));
            server->coin_count++;
            server->coins[server->coin_count - 1] = coin;
        }
    }
}

static void coins_handler(server_t *server)
{
    size_t estimated_coin_count = server->map_rows * server->map_cols;
    coin_t coin;

    server->coins = malloc(sizeof(coin_t) * estimated_coin_count);
    if (!server->coins)
        handle_error("malloc coins", server);
    server->coin_count = 0;
    for (size_t row = 0; row < server->map_rows; row++)
        check_coins(server, row, coin);
}

static void close_and_free(FILE *file, char *line)
{
    if (file)
        fclose(file);
    if (line)
        free(line);
}

void get_map_size(server_t *server, size_t *rows, size_t *cols)
{
    FILE *file = fopen(server->map_path, "r");
    size_t row_count = 0;
    size_t col_count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (!file)
        handle_error("fopen", server);
    while (1) {
        read = getline(&line, &len, file);
        if (read == -1)
            break;
        row_count++;
        if (read > (ssize_t)col_count)
            col_count = read;
    }
    close_and_free(file, line);
    *rows = row_count;
    *cols = col_count;
}

char **allocate_map(server_t *server)
{
    size_t rows = server->map_rows;
    size_t cols = server->map_cols;
    char **map = malloc(rows * sizeof(char *));

    if (!map)
        handle_error("malloc", server);
    for (size_t i = 0; i < rows; i++) {
        map[i] = malloc(sizeof(char) * (cols + 1));
        if (!map[i])
            handle_error("malloc", server);
    }
    return map;
}

void fill_map(const char *path, char **map, server_t *server)
{
    FILE *file = fopen(path, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t i = 0;
    size_t rows = server->map_rows;
    size_t cols = server->map_cols;

    if (!file)
        handle_error("fopen", server);
    while (i < rows) {
        read = getline(&line, &len, file);
        if (read == -1)
            break;
        strncpy(map[i], line, cols);
        map[i][cols] = '\0';
        i++;
    }
    free(line);
    fclose(file);
}

void load_map(server_t *server)
{
    get_map_size(server, &server->map_rows, &server->map_cols);
    server->map = allocate_map(server);
    fill_map(server->map_path, server->map, server);
    coins_handler(server);
}
