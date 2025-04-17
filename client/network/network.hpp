// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Network class for Jetpack client
*/

#ifndef CLIENT_NETWORK_NETWORK_HPP_
#define CLIENT_NETWORK_NETWORK_HPP_

#include "../debug/debug.hpp"
#include "../gamestate.hpp"
#include "../protocol.hpp"
#include "protocol_handlers.hpp"
#include <arpa/inet.h>
#include <atomic>
#include <iomanip>
#include <memory>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace jetpack {
namespace network {

class Network {
public:
  /**
   * @brief Initialize network component
   *
   * @param host Server hostname or IP
   * @param port Server port number
   * @param debugMode Whether debug mode is enabled
   * @param gameState Shared game state
   */
  Network(const std::string &host, int port, bool debugMode,
          GameState *gameState);

  /**
   * @brief Cleanup resources
   */
  ~Network();

  /**
   * @brief Connect to server and send CLIENT_CONNECT packet
   *
   * @return true if connected successfully
   */
  bool connect();

  /**
   * @brief Send CLIENT_DISCONNECT packet and close connection
   */
  void disconnect();

  /**
   * @brief Start network thread to process packets
   */
  void run();

  /**
   * @brief Stop network thread
   */
  void stop();

  /**
   * @brief Send a debug message to server
   *
   * @param message Message to send
   * @return true if sent successfully
   */
  bool sendDebugMessage(const std::string &message);

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

  // Protocol handlers
  std::unique_ptr<ProtocolHandlers> protocolHandlers_;

  // Network methods
  bool sendPacket(protocol::PacketType type,
                  const std::vector<uint8_t> &payload);
  bool receivePacket(protocol::PacketHeader *header,
                     std::vector<uint8_t> *payload);

  // Input sending
  void sendPlayerInput();

  // Connection health checking
  void checkConnectionHealth();

  // Helper functions
  std::string packetTypeToString(protocol::PacketType type);
  std::string toHexString(uint8_t byte);
};

} // namespace network
} // namespace jetpack

#endif // CLIENT_NETWORK_NETWORK_HPP_
