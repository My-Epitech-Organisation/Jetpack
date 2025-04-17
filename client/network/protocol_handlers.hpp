// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Protocol handlers for different message types
*/

#ifndef CLIENT_NETWORK_PROTOCOL_HANDLERS_HPP_
#define CLIENT_NETWORK_PROTOCOL_HANDLERS_HPP_

#include "../debug/debug.hpp"
#include "../gamestate.hpp"
#include "../protocol.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace jetpack {
namespace network {

class ProtocolHandlers {
public:
  ProtocolHandlers(GameState *gameState, bool debugMode);
  ~ProtocolHandlers() = default;

  // Protocol message handlers
  void handleServerWelcome(const std::vector<uint8_t> &payload);
  void handleMapChunk(const std::vector<uint8_t> &payload);
  void handleGameStart(const std::vector<uint8_t> &payload);
  void handleGameState(const std::vector<uint8_t> &payload);
  void handleGameEnd(const std::vector<uint8_t> &payload);
  void handleDebugInfo(const std::vector<uint8_t> &payload);

private:
  GameState *gameState_;
  bool debugMode_;

  // Helper methods
  void debugPrint(const std::string &message);
};

} // namespace network
} // namespace jetpack

#endif // CLIENT_NETWORK_PROTOCOL_HANDLERS_HPP_