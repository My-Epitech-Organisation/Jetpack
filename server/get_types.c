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
        return "GAME_INPUT";
    if (type == GAME_STATE)
        return "GAME_STATE";
    if (type == GAME_END)
        return "GAME_END";
    if (type == CLIENT_DISCONNECT)
        return "CLIENT_DISCONNECT";
    if (type == DEBUG_INFO)
        return "DEBUG_INFO";
    return "UNKNOWN_TYPE";
}

char *get_type_string_prev(uint8_t type)
{
    if (type == CLIENT_CONNECT)
        return "CLIENT_CONNECT";
    if (type == SERVER_WELCOME)
        return "SERVER_WELCOME";
    if (type == MAP_CHUNK)
        return "MAP_CHUNK";
    if (type == GAME_START)
        return "GAME_START";
    return get_type_string_next(type);
}
