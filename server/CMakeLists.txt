cmake_minimum_required(VERSION 3.10)
project(JetpackServer C)

set(CMAKE_C_STANDARD 11)  # Définit la version du standard C
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror")  # Active les warnings

# Ajoute le fichier source
add_executable(jetpack_server server.c main.c error_handling.c put_str_fd.c set_server.c check_args.c handle_client.c parsing.c load_map.c read_client.c send_messages_to_clients.c write_messages.c launch_game.c game_loop.c send_game_messages.c handle_input_from_clients.c)
