/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Command line argument parsing
*/

#include "includes/server.h"

void parsing_launch(int argc, char **argv, server_t *server)
{
    server->debug_mode = false;
    server->max_client = MAX_CLIENTS;
    server->port = 0;
    server->map_path = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "Missing port value\n");
                exit(84);
            }
            server->port = atoi(argv[i]);
        } else if (strcmp(argv[i], "-m") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "Missing map path\n");
                exit(84);
            }
            server->map_path = argv[i];
        } else if (strcmp(argv[i], "-d") == 0) {
            server->debug_mode = true;
            printf("Debug mode enabled - verbose protocol logging will be displayed\n");
            printf("Debug logging initialized\n");
        } else if (strcmp(argv[i], "-c") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "Missing number of clients\n");
                exit(84);
            }
            server->max_client = atoi(argv[i]);
            if (server->max_client < 2) {
                fprintf(stderr, "Invalid number of clients\n");
                exit(84);
            }
            printf("Max clients set to %d\n", server->max_client);
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            exit(84);
        }
    }

    if (server->port == 0 || server->map_path == NULL) {
        fprintf(stderr, "Port and map path are required\n");
        exit(84);
    }

    if (server->debug_mode)
        print_debug_info_connection(server, "Main");
}
