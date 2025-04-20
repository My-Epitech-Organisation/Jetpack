// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Main client entrypoint
*/

#include "debug/debug.hpp"
#include "gamestate.hpp"
#include "graphics/graphics.hpp"
#include "network/network.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

// Global variables for the main application components
jetpack::network::Network *g_network = nullptr;
jetpack::graphics::Graphics *g_graphics = nullptr;
bool g_debug_mode = false;
std::atomic<bool> g_window_closed(false);
std::atomic<bool> g_countdown_ended(false);

void signal_handler(int signal) {
  // Properly shut down the application when receiving termination signals
  jetpack::debug::print("Main",
                        "Received signal " + std::to_string(signal) +
                            ", shutting down...",
                        g_debug_mode);

  if (g_network) {
    if (g_debug_mode) {
      g_network->sendDebugMessage("Client shutting down due to signal " +
                                  std::to_string(signal));
    }
    g_network->stop();
  }

  if (g_graphics) {
    g_graphics->stop();
  }

  jetpack::debug::shutdownLogging();
}

void print_usage(const char *program_name) {
  std::cout << "Usage: " << program_name << " -h <host> -p <port> [-d]"
            << std::endl;
  std::cout << "  -h <host>   Server hostname or IP" << std::endl;
  std::cout << "  -p <port>   Server port" << std::endl;
  std::cout << "  -d          Enable debug mode (verbose protocol logging)"
            << std::endl;
}

void handle_window_closed() {
  // Callback triggered when window is closed to initiate shutdown
  jetpack::debug::print("Main",
                        "Window closed callback triggered, initiating shutdown",
                        g_debug_mode);
  g_window_closed = true;
}

void handle_countdown_end() {
  // Callback triggered when the end game countdown finishes
  jetpack::debug::print("Main",
                        "Game end countdown finished, sending "
                        "CLIENT_DISCONNECT and initiating shutdown",
                        g_debug_mode);

  g_countdown_ended = true;

  if (g_network) {
    g_network->disconnect();
    g_network->stop();
  }

  g_window_closed = true;
}

int main(int argc, char *argv[]) {
  std::string host;
  int port = 0;
  bool debug_mode = false;

  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "-h" && i + 1 < argc) {
      host = argv[++i];
    } else if (arg == "-p" && i + 1 < argc) {
      try {
        port = std::stoi(argv[++i]);
        if (port <= 0 || port > 65535) {
          std::cerr << "Error: Port must be between 1 and 65535" << std::endl;
          print_usage(argv[0]);
          return 1;
        }
      } catch (const std::exception &e) {
        std::cerr << "Error: Invalid port number" << std::endl;
        print_usage(argv[0]);
        return 1;
      }
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

  // Set global debug flag
  g_debug_mode = debug_mode;

  if (debug_mode) {
    std::cout
        << "Debug mode enabled - verbose protocol logging will be displayed"
        << std::endl;

    if (jetpack::debug::initLogging(debug_mode)) {
      std::cout << "Debug logging initialized" << std::endl;
      jetpack::debug::logToFile(
          "Main", "Client starting with debug mode enabled", debug_mode);
    } else {
      std::cerr << "Warning: Failed to initialize file logging" << std::endl;
    }
  }

  // Set up signal handlers
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  try {
    // Initialize core game components
    auto gameState = std::make_unique<jetpack::GameState>();

    auto network = std::make_unique<jetpack::network::Network>(
        host, port, debug_mode, gameState.get());
    auto graphics = std::make_unique<jetpack::graphics::Graphics>(
        gameState.get(), debug_mode);

    g_network = network.get();
    g_graphics = graphics.get();

    graphics->setOnWindowClosedCallback(handle_window_closed);
    graphics->setOnCountdownEndCallback(
        handle_countdown_end);

    // Connect to the server
    jetpack::debug::print("Main",
                          "Connecting to " + host + ":" + std::to_string(port),
                          debug_mode);
    std::cout << "Connecting to " << host << ":" << port << std::endl;

    if (!network->connect()) {
      std::cerr << "Failed to connect to server" << std::endl;
      jetpack::debug::shutdownLogging();
      return 1;
    }

    // Start network and graphics systems
    network->run();
    graphics->run();

    if (debug_mode) {
      network->sendDebugMessage("Client started with debug mode enabled");
      jetpack::debug::logToFile(
          "Main", "Network and graphics systems initialized", debug_mode);
    }

    // Main application loop
    while (graphics->isRunning() && !g_window_closed) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Clean shutdown
    if (g_window_closed) {
      jetpack::debug::print("Main",
                            "Main window was closed, stopping network thread",
                            debug_mode);
      network->disconnect();
      network->stop();
    } else {
      network->stop();
    }

    jetpack::debug::print("Main", "Client shutting down normally", debug_mode);
    jetpack::debug::shutdownLogging();

  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    jetpack::debug::shutdownLogging();
    return 1;
  }

  return 0;
}
