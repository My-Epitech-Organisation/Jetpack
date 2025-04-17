// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Protocol handler implementations
*/

#include "protocol_handlers.hpp"

namespace jetpack {
namespace network {

ProtocolHandlers::ProtocolHandlers(GameState *gameState, bool debugMode)
    : gameState_(gameState), debugMode_(debugMode) {}

void ProtocolHandlers::handleServerWelcome(
    const std::vector<uint8_t> &payload) {
  if (payload.size() < 2) {
    debugPrint("SERVER_WELCOME: Invalid payload size");
    return;
  }

  uint8_t acceptCode = payload[0];
  uint8_t assignedId = payload[1];

  if (acceptCode == 1) {
    debugPrint("SERVER_WELCOME: Connection accepted, Assigned ID=" +
               std::to_string(assignedId));

    // Log additional data if present
    if (payload.size() > 2) {
      std::stringstream ss;
      ss << "Additional data (" << (payload.size() - 2) << " bytes): ";
      ss << debug::formatHexDump(
          std::vector<uint8_t>(payload.begin() + 2, payload.end()));
      debugPrint(ss.str());
    }

    gameState_->setConnected(true);
    gameState_->setAssignedId(assignedId);
  } else {
    debugPrint("SERVER_WELCOME: Connection rejected");
    gameState_->setConnected(false);
  }
}

void ProtocolHandlers::handleMapChunk(const std::vector<uint8_t> &payload) {
  if (payload.size() < 4) {
    debugPrint("MAP_CHUNK: Invalid payload size");
    return;
  }

  uint16_t chunkIndex = (payload[0] << 8) | payload[1];
  uint16_t chunkCount = (payload[2] << 8) | payload[3];

  debugPrint("MAP_CHUNK: Index=" + std::to_string(chunkIndex) +
             ", Count=" + std::to_string(chunkCount) +
             ", Size=" + std::to_string(payload.size() - 4) + " bytes");

  if (chunkIndex == 0) {
    // First chunk - should contain dimensions
    if (payload.size() < 8) {
      debugPrint("MAP_CHUNK: First chunk too small, missing dimensions");
      return;
    }

    uint16_t width = (payload[4] << 8) | payload[5];
    uint16_t height = (payload[6] << 8) | payload[7];

    debugPrint("MAP dimensions: " + std::to_string(width) + "x" +
               std::to_string(height));

    gameState_->setMapDimensions(width, height);

    // Add map data (skip first 8 bytes which are dimensions)
    std::vector<uint8_t> mapData(payload.begin() + 8, payload.end());
    gameState_->addMapChunk(mapData);
  } else {
    // Additional chunk - just add the data
    std::vector<uint8_t> mapData(payload.begin() + 4, payload.end());
    gameState_->addMapChunk(mapData);
  }
}

void ProtocolHandlers::handleGameStart(const std::vector<uint8_t> &payload) {
  if (payload.size() < 5) {
    debugPrint("GAME_START: Invalid payload size");
    return;
  }

  uint8_t playerCount = payload[0];
  uint16_t startX = (payload[1] << 8) | payload[2];
  uint16_t startY = (payload[3] << 8) | payload[4];

  debugPrint("GAME_START: Player count=" + std::to_string(playerCount) +
             ", Start position=(" + std::to_string(startX) + "," +
             std::to_string(startY) + ")");

  // Game is now running
  gameState_->setGameRunning(true);
}

void ProtocolHandlers::handleGameState(const std::vector<uint8_t> &payload) {
  if (payload.size() < 5) {
    debugPrint("GAME_STATE: Invalid payload size");
    return;
  }

  uint32_t tick =
      (payload[0] << 24) | (payload[1] << 16) | (payload[2] << 8) | payload[3];
  uint8_t numPlayers = payload[4];

  debugPrint("GAME_STATE: Tick=" + std::to_string(tick) +
             ", Players=" + std::to_string(numPlayers));

  gameState_->setCurrentTick(tick);

  // Each player data is 8 bytes (ID, X, Y, Score, Alive)
  const size_t PLAYER_DATA_SIZE = 8;
  if (payload.size() < 5 + (numPlayers * PLAYER_DATA_SIZE)) {
    debugPrint("GAME_STATE: Not enough data for " + std::to_string(numPlayers) +
               " players (need " +
               std::to_string(5 + numPlayers * PLAYER_DATA_SIZE) +
               " bytes, got " + std::to_string(payload.size()) + ")");
    return;
  }

  std::vector<protocol::PlayerState> playerStates;
  for (int i = 0; i < numPlayers; i++) {
    int offset = 5 + (i * PLAYER_DATA_SIZE);
    protocol::PlayerState state;

    state.id = payload[offset];
    state.posX = (payload[offset + 1] << 8) | payload[offset + 2];
    state.posY = (payload[offset + 3] << 8) | payload[offset + 4];
    state.score = (payload[offset + 5] << 8) | payload[offset + 6];
    state.alive = payload[offset + 7];

    playerStates.push_back(state);

    debugPrint("Player " + std::to_string(state.id) + ": Position=(" +
               std::to_string(state.posX) + "," + std::to_string(state.posY) +
               "), Score=" + std::to_string(state.score) +
               ", Alive=" + std::to_string(state.alive));
  }

  gameState_->setPlayerStates(playerStates);
}

void ProtocolHandlers::handleGameEnd(const std::vector<uint8_t> &payload) {
  if (payload.size() < 2) {
    debugPrint("GAME_END: Invalid payload size");
    return;
  }

  uint8_t reasonCode = payload[0];
  uint8_t winnerId = payload[1];

  std::string reasonStr;
  switch (reasonCode) {
  case protocol::MAP_COMPLETE:
    reasonStr = "Map completed";
    break;
  case protocol::PLAYER_DIED:
    reasonStr = "Player died";
    break;
  case protocol::PLAYER_DISCONNECT:
    reasonStr = "Player disconnected";
    break;
  default:
    reasonStr = "Unknown (" + std::to_string(reasonCode) + ")";
  }

  std::string winnerStr = (winnerId == protocol::NO_WINNER)
                              ? "Draw"
                              : "Player " + std::to_string(winnerId);

  debugPrint("GAME_END: Reason=" + reasonStr + ", Winner=" + winnerStr);

  // Additional score data may be present
  if (payload.size() > 2) {
    debugPrint("GAME_END: Score data present (" +
               std::to_string(payload.size() - 2) + " bytes)");
  }

  gameState_->setGameRunning(false);
  gameState_->setGameEnded(true, winnerId);
}

void ProtocolHandlers::handleDebugInfo(const std::vector<uint8_t> &payload) {
  if (payload.size() < 2) {
    debugPrint("DEBUG_INFO: Invalid payload size");
    return;
  }

  uint16_t msgLen = (payload[0] << 8) | payload[1];

  // Fix: Use explicit size_t type for the sum to avoid sign comparison issues
  size_t requiredSize = static_cast<size_t>(2) + static_cast<size_t>(msgLen);
  if (payload.size() < requiredSize) {
    debugPrint("DEBUG_INFO: Not enough data");
    return;
  }

  std::string debugMsg(payload.begin() + 2, payload.begin() + 2 + msgLen);
  debugPrint("SERVER DEBUG: " + debugMsg);
}

void ProtocolHandlers::debugPrint(const std::string &message) {
  debug::logToFile("Protocol", message, debugMode_);
}

} // namespace network
} // namespace jetpack