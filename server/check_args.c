/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Command line argument validation
*/

#include "includes/server.h"
#include <sys/stat.h>

bool check_port(char *port)
{
    for (int i = 0; port[i]; i++) {
        if (!isdigit(port[i])) {
            fprintf(stderr, "Port must be a number\n");
            return false;
        }
    }
    if (atoi(port) < 1024 || atoi(port) > 65535) {
        fprintf(stderr, "Port must be between 1024 and 65535\n");
        return false;
    }
    return true;
}

bool check_path_map(char *map)
{
    struct stat path;

    if (stat(map, &path) != 0) {
        fprintf(stderr, "Map path is invalid\n");
        return false;
    }
    if (!S_ISREG(path.st_mode)) {
        fprintf(stderr, "Map path is not a file\n");
        return false;
    }
    if (access(map, R_OK) == -1) {
        fprintf(stderr, "Map path is not readable\n");
        return false;
    }
    return true;
}

int check_args(int argc, char **argv)
{
    if (argc < 5 || argc > 6)
        return 84;
    if (strcmp(argv[1], "-p") != 0 || strcmp(argv[3], "-m") != 0)
        return 84;
    if (!check_port(argv[2]) || !check_path_map(argv[4]))
        return 84;
    return 0;
}
