// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Network class for Jetpack client
*/

#ifndef CLIENT_NETWORK_NETWORK_HPP_
#define CLIENT_NETWORK_NETWORK_HPP_

#include "../gamestate.hpp"
#include "../protocol.hpp"
#include "protocol_handlers.hpp"
#include <arpa/inet.h>
#include <atomic>
#include <memory>
#include <netinet/in.h>
#include <poll.h>
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

  // Core network operations
  bool connect();
  void disconnect();
  void run();
  void stop();

  // Packet communication
  bool sendPacket(protocol::PacketType type,
                  const std::vector<uint8_t> &payload);
  bool receivePacket(protocol::PacketHeader *header,
                     std::vector<uint8_t> *payload);

  // Game-specific communication
  void sendPlayerInput();
  bool sendDebugMessage(const std::string &message);
  void checkConnectionHealth();

private:
  // Network configuration
  std::string host_;
  int port_;
  bool debugMode_;
  int socket_;
  struct pollfd pfd_;

  // Thread management
  std::atomic<bool> running_;
  std::thread networkThread_;

  // Game state reference
  GameState *gameState_;

  // Protocol handlers
  ProtocolHandlers protocolHandlers_;

  // Network thread function
  void networkLoop();

  // Helper methods for debugging
  std::string packetTypeToString(protocol::PacketType type);
  std::string toHexString(uint8_t byte);
};

} // namespace network
} // namespace jetpack

#endif // CLIENT_NETWORK_NETWORK_HPP_
