// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-jetpack-santiago.pidcova
** File description:
** Main client entrypoint
*/

#include "gamestate.hpp"
#include "graphics/graphics.hpp"
#include "network/network.hpp"
#include <csignal>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

// Global pointers for signal handling
jetpack::network::Network *g_network = nullptr;
jetpack::graphics::Graphics *g_graphics = nullptr;

void signal_handler(int signal) {
  std::cout << "Received signal " << signal << ", shutting down..."
            << std::endl;
  if (g_network)
    g_network->stop();
  if (g_graphics)
    g_graphics->stop();
}

void print_usage(const char *program_name) {
  std::cout << "Usage: " << program_name << " -h <host> -p <port> [-d]"
            << std::endl;
  std::cout << "  -h <host>   Server hostname or IP" << std::endl;
  std::cout << "  -p <port>   Server port" << std::endl;
  std::cout << "  -d          Enable debug mode" << std::endl;
}

int main(int argc, char *argv[]) {
  // Parse command line arguments
  std::string host;
  int port = 0;
  bool debug_mode = false;

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "-h" && i + 1 < argc) {
      host = argv[++i];
    } else if (arg == "-p" && i + 1 < argc) {
      port = std::stoi(argv[++i]);
    } else if (arg == "-d") {
      debug_mode = true;
    } else {
      print_usage(argv[0]);
      return 1;
    }
  }

  if (host.empty() || port <= 0) {
    std::cerr << "Error: Host and port are required" << std::endl;
    print_usage(argv[0]);
    return 1;
  }

  // Register signal handler
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  try {
    // Initialize shared game state
    auto gameState = std::make_unique<jetpack::GameState>();

    // Initialize network and graphics
    auto network = std::make_unique<jetpack::network::Network>(
        host, port, debug_mode, gameState.get());
    auto graphics = std::make_unique<jetpack::graphics::Graphics>(
        gameState.get(), debug_mode);

    // Set global pointers for signal handling
    g_network = network.get();
    g_graphics = graphics.get();

    // Start network and graphics threads
    std::cout << "Connecting to " << host << ":" << port << std::endl;
    if (!network->connect()) {
      std::cerr << "Failed to connect to server" << std::endl;
      return 1;
    }

    network->run();
    graphics->run();

    // Wait for graphics thread to exit
    while (graphics->isRunning()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Stop network thread
    network->stop();

    std::cout << "Client shutting down" << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
