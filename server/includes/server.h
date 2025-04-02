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

void handle_error(char *msg);
int arg_missing(int argc);

void parsing_launch(int argc, char **argv, server_t *server);

void server(int argc, char **argv);
int set_server_socket(void);
void set_bind(server_t *server);
void set_listen(int fd);

void handle_clients(server_t *server);

void parse_line(server_t *server, int i);
void read_client(server_t *server, int i);

void put_str_fd(int fd, char *str);

int check_args(int argc, char **argv);
bool check_port(char *port);
bool check_path_map(char *path);

void load_map(server_t *server);

#endif /* !SERVER_H_ */
