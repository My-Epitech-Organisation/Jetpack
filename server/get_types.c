/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** get_types
*/

#include "includes/server.h"

char *get_type_string_next(uint8_t type)
{
    if (type == GAME_INPUT)
        return strdup("GAME_INPUT");
    if (type == GAME_STATE)
        return strdup("GAME_STATE");
    if (type == GAME_END)
        return strdup("GAME_END");
    if (type == CLIENT_DISCONNECT)
        return strdup("CLIENT_DISCONNECT");
    if (type == DEBUG_INFO)
        return strdup("DEBUG_INFO");
    return strdup("UNKNOWN_TYPE");
}

char *get_type_string_prev(uint8_t type)
{
    if (type == CLIENT_CONNECT)
        return strdup("CLIENT_CONNECT");
    if (type == SERVER_WELCOME)
        return strdup("SERVER_WELCOME");
    if (type == MAP_CHUNK)
        return strdup("MAP_CHUNK");
    if (type == GAME_START)
        return strdup("GAME_START");
    return get_type_string_next(type);
}
