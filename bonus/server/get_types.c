/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** get_types
*/

#include "includes/server.h"

char *get_type_string_next(uint8_t type)
{
    switch (type) {
        case GAME_INPUT:
            return "GAME_INPUT";
        case GAME_STATE:
            return "GAME_STATE";
        case GAME_END:
            return "GAME_END";
        case CLIENT_DISCONNECT:
            return "CLIENT_DISCONNECT";
        case DEBUG_INFO:
            return "DEBUG_INFO";
    }
    return "UNKNOWN_TYPE";
}

char *get_type_string_prev(uint8_t type)
{
    switch (type) {
        case CLIENT_CONNECT:
            return "CLIENT_CONNECT";
        case SERVER_WELCOME:
            return "SERVER_WELCOME";
        case MAP_CHUNK:
            return "MAP_CHUNK";
        case GAME_START:
            return "GAME_START";
    }
    return get_type_string_next(type);
}
