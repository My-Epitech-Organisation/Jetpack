// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Protocol handler implementations
*/

#include "protocol_handlers.hpp"
#include "../debug/debug.hpp"
#include <cmath>
#include <sstream>

namespace jetpack {
namespace network {

ProtocolHandlers::ProtocolHandlers(GameState *gameState, bool debugMode)
    : gameState_(gameState), debugMode_(debugMode), expectedChunkCount(0),
      receivedChunkCount(0), mapComplete(false) {}

void ProtocolHandlers::handleServerWelcome(
    const std::vector<uint8_t> &payload) {
  if (payload.size() < 2) {
    debugPrint("SERVER_WELCOME: Invalid payload size");
    return;
  }

  uint8_t acceptCode = payload[0];
  uint8_t assignedId = payload[1];

  if (acceptCode == 1) {
    debugLogToFile("SERVER_WELCOME: Connection accepted, Assigned ID=" +
                   std::to_string(assignedId));

    if (payload.size() > 2) {
      std::stringstream ss;
      ss << "Additional data (" << (payload.size() - 2) << " bytes): ";
      ss << debug::formatHexDump(
          std::vector<uint8_t>(payload.begin() + 2, payload.end()));
      debugLogToFile(ss.str());
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

  debugLogToFile("MAP_CHUNK: Index=" + std::to_string(chunkIndex) +
                 ", Count=" + std::to_string(chunkCount) +
                 ", Size=" + std::to_string(payload.size() - 4) + " bytes");

  // On first chunk, initialize our storage
  if (chunkIndex == 0) {
    mapChunks.clear();
    mapChunks.resize(chunkCount);
    expectedChunkCount = chunkCount;
    receivedChunkCount = 0;
    mapComplete = false;
  }

  if (chunkIndex >= mapChunks.size()) {
    debugPrint("MAP_CHUNK: Invalid chunk index: " + std::to_string(chunkIndex) +
               ", expected max " + std::to_string(mapChunks.size() - 1));
    return;
  }

  // Store the chunk data (excluding the 4-byte header)
  std::vector<uint8_t> chunkData(payload.begin() + 4, payload.end());
  mapChunks[chunkIndex] = chunkData;
  receivedChunkCount++;

  if (receivedChunkCount == expectedChunkCount) {
    debugLogToFile("MAP_CHUNK: All " + std::to_string(expectedChunkCount) +
                   " chunks received, processing complete map");
    processCompleteMap();
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

  debugLogToFile("GAME_START: Player count=" + std::to_string(playerCount) +
                 ", Start position=(" + std::to_string(startX) + "," +
                 std::to_string(startY) + ")");

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

  debugLogToFile("GAME_STATE: Tick=" + std::to_string(tick) +
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

    debugLogToFile("Processing player " + std::to_string(i) +
                   " data at offset " + std::to_string(offset));
    state.id = payload[offset];
    state.posX = (payload[offset + 1] << 8) | payload[offset + 2];
    state.posY = (payload[offset + 3] << 8) | payload[offset + 4];
    state.score = (payload[offset + 5] << 8) | payload[offset + 6];
    state.alive = payload[offset + 7];

    playerStates.push_back(state);

    debugLogToFile("Player " + std::to_string(state.id) + ": Position=(" +
                   std::to_string(state.posX) + "," +
                   std::to_string(state.posY) +
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

  size_t requiredSize = static_cast<size_t>(2) + static_cast<size_t>(msgLen);
  if (payload.size() < requiredSize) {
    debugPrint("DEBUG_INFO: Not enough data");
    return;
  }

  std::string debugMsg(payload.begin() + 2, payload.begin() + 2 + msgLen);
  debugPrint("SERVER DEBUG: " + debugMsg);
}

void ProtocolHandlers::processCompleteMap() {
  // The server sends the map column by column
  // Each column is a separate chunk

  uint16_t numColumns = static_cast<uint16_t>(mapChunks.size());

  // Check if we have valid chunks
  if (numColumns == 0) {
    debugPrint("processCompleteMap: No map chunks received");
    return;
  }

  // The height of the map is determined by the size of each chunk
  uint16_t mapHeight = static_cast<uint16_t>(mapChunks[0].size());

  debugLogToFile("Processing map with dimensions: " +
                 std::to_string(numColumns) + "x" + std::to_string(mapHeight));

  // First, set the map dimensions
  gameState_->setMapDimensions(numColumns, mapHeight);

  // Create a vector for the final map in row-major format
  std::vector<uint8_t> finalMap(numColumns * mapHeight, protocol::EMPTY);

  // Process column by column (as received from server)
  for (uint16_t col = 0; col < numColumns; col++) {
    const auto &columnData = mapChunks[col];

    // Check if this column has the expected height
    if (columnData.size() != mapHeight) {
      debugPrint("WARNING: Column " + std::to_string(col) +
                 " has unexpected size: " + std::to_string(columnData.size()) +
                 " (expected " + std::to_string(mapHeight) + ")");
      continue;
    }

    // Process each character in the column
    for (uint16_t row = 0; row < mapHeight; row++) {
      uint8_t tileValue = protocol::EMPTY; // Default to empty
      char mapChar = static_cast<char>(columnData[row]);

      // Convert characters to tile types according to map format
      switch (mapChar) {
      case '#': // Wall
        tileValue = protocol::WALL;
        break;
      case 'c': // Coin
      case 'C':
        tileValue = protocol::COIN;
        break;
      case 'e': // Electric
      case 'E':
        tileValue = protocol::ELECTRIC;
        break;
      case '_': // Empty
      default:
        tileValue = protocol::EMPTY;
        break;
      }

      // Store in row-major order
      finalMap[row * numColumns + col] = tileValue;
    }
  }

  // Store the final map in the game state
  gameState_->setMapData(finalMap);

  mapComplete = true;
  debugLogToFile("Map processing completed successfully");
}

void ProtocolHandlers::debugPrint(const std::string &message) {
  debug::print("Protocol", message, debugMode_);
}

void ProtocolHandlers::debugLogToFile(const std::string &message) {
  debug::logToFile("Protocol", message, debugMode_);
}

} // namespace network
} // namespace jetpack