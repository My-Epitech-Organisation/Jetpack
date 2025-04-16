// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-jetpack-santiago.pidcova
** File description:
** Network class for Jetpack client
*/

#ifndef CLIENT_NETWORK_NETWORK_HPP_
#define CLIENT_NETWORK_NETWORK_HPP_

#include "../gamestate.hpp"
#include "../protocol.hpp"
#include <arpa/inet.h>
#include <atomic>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace jetpack {
namespace network {

class Network {
public:
  Network(const std::string &host, int port, bool debugMode,
          GameState *gameState);
  ~Network();

  bool connect();
  void disconnect();
  void run();
  void stop();

private:
  // Connection info
  std::string host_;
  int port_;
  bool debugMode_;
  int socket_;
  std::atomic<bool> running_;
  GameState *gameState_;

  // Thread
  std::thread networkThread_;

  // Network methods
  bool sendPacket(protocol::PacketType type,
                  const std::vector<uint8_t> &payload);
  bool receivePacket(protocol::PacketHeader *header,
                     std::vector<uint8_t> *payload);

  // Protocol handlers
  void handleServerWelcome(const std::vector<uint8_t> &payload);
  void handleMapChunk(const std::vector<uint8_t> &payload);
  void handleGameStart(const std::vector<uint8_t> &payload);
  void handleGameState(const std::vector<uint8_t> &payload);
  void handleGameEnd(const std::vector<uint8_t> &payload);
  void handleDebugInfo(const std::vector<uint8_t> &payload);

  // Input sending
  void sendPlayerInput();

  // Debug printing
  void debugPrint(const std::string &message);
};

} // namespace network
} // namespace jetpack

#endif // CLIENT_NETWORK_NETWORK_HPP_
