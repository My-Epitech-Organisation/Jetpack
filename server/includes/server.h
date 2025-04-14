/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-myftp-santiago.pidcova
** File description:
** server
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/wait.h>

#ifndef SERVER_H_
    #define SERVER_H_

    #define MAX_CLIENTS 2
    #define MAGIC_BYTE 0xAB
    #define CLIENT_CONNECT 0x01
    #define SERVER_WELCOME 0x02
    #define MAP_CHUNK 0x03
    #define GAME_START 0x04
    #define GAME_INPUT 0x05
    #define GAME_STATE 0x06
    #define GAME_END 0x07
    #define CLIENT_DISCONNECT 0x08
    #define DEBUG_INFO 0x09

typedef struct client_s {
    int fd;
    struct sockaddr_in addr;
    socklen_t addr_len;
    char *user;
    bool is_active;
    bool is_passive;
    char ip[INET_ADDRSTRLEN];
    int data_port;
    int data_fd;
} client_t;

typedef struct server_s {
    int port;
    char *map_path;
    char **map;
    size_t map_rows;
    size_t map_cols;
    uint16_t start_x;
    uint16_t start_y;
    uint8_t message_type;
    int fd;
    struct sockaddr_in addr;
    struct pollfd *fds;
    int nfds;
    char buffer[1024];
    ssize_t bytes_read;
    client_t **client;
    int client_count;
    bool debug_mode;
} server_t;

// Error handling functions
void handle_error(char *msg);
int arg_missing(int argc);
int check_args(int argc, char **argv);
bool check_port(char *port);
bool check_path_map(char *path);
bool check_header(unsigned char header[4], int i, server_t *server);
int check_payload_length(uint16_t payload_length, server_t *server, int i);
int check_read(int read_ret, uint16_t payload_length, char *payload);

// Sending messages to clients
void send_welcome(int client_fd, uint8_t assigned_id);
void send_game_start(server_t *server, int client_fd);
void send_map(server_t *server, int client_fd);

// Writing messages to clients
void write_header(uint8_t *buf, uint8_t type, uint16_t total_len);
void write_map_payload(uint8_t *buffer, uint16_t chunk_index,
    uint16_t chunk_count);

// Server set up functions
void server(int argc, char **argv);
int set_server_socket(void);
void set_bind(server_t *server);
void set_listen(int fd);

// Handling client functions
void handle_clients(server_t *server);
void read_client(server_t *server, int i);

void parsing_launch(int argc, char **argv, server_t *server);
void put_str_fd(int fd, char *str);
void load_map(server_t *server);
void launch_game(server_t *server);

#endif /* !SERVER_H_ */
