cmake_minimum_required(VERSION 3.10)
project(JetpackClient CXX)

set(CMAKE_CXX_STANDARD 17)  # Définit le standard C++
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")

# Recherche la bibliothèque SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Ajoute les fichiers source
add_executable(jetpack_client main.cpp graphics/graphics.cpp network/network.cpp)

# Lie la bibliothèque SFML
target_link_libraries(jetpack_client sfml-graphics sfml-window sfml-system)
