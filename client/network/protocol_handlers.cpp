// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Protocol handler implementations
*/

#include "protocol_handlers.hpp"
#include <cmath>

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

  // Handle first chunk receiving
  if (chunkIndex == 0) {
    // Reset map chunk storage when receiving the first chunk
    mapChunks.clear();
    mapChunks.resize(chunkCount);
    expectedChunkCount = chunkCount;
    receivedChunkCount = 0;
    mapComplete = false;
  }

  // Validate chunk index
  if (chunkIndex >= mapChunks.size()) {
    debugPrint("MAP_CHUNK: Invalid chunk index: " + std::to_string(chunkIndex) +
               ", expected max " + std::to_string(mapChunks.size() - 1));
    return;
  }

  // Store the chunk data (excluding the chunk header)
  std::vector<uint8_t> chunkData(payload.begin() + 4, payload.end());
  mapChunks[chunkIndex] = chunkData;
  receivedChunkCount++;

  // Check if we've received all chunks
  if (receivedChunkCount == expectedChunkCount) {
    debugPrint("MAP_CHUNK: All " + std::to_string(expectedChunkCount) +
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

void ProtocolHandlers::processCompleteMap() {
  // Combine all chunks into a single data array
  std::vector<uint8_t> completeMapData;
  for (const auto &chunk : mapChunks) {
    completeMapData.insert(completeMapData.end(), chunk.begin(), chunk.end());
  }

  // Log the total size of the map data
  debugPrint("Map data complete: " + std::to_string(completeMapData.size()) +
             " bytes");

  // Determine map format (based on RFC section 6)
  // First, check if the data might be in binary format with width/height at the
  // beginning
  if (completeMapData.size() >= 4) {
    // Try to extract width and height from the first 4 bytes
    uint16_t width = (completeMapData[0] << 8) | completeMapData[1];
    uint16_t height = (completeMapData[2] << 8) | completeMapData[3];

    // Check if dimensions make sense (more data follows after width/height)
    if (completeMapData.size() >= 4 + (width * height)) {
      debugPrint("Binary map format detected: " + std::to_string(width) + "x" +
                 std::to_string(height));

      // Set dimensions and store map data (excluding the 4-byte header)
      gameState_->setMapDimensions(width, height);

      // Add only the map tile data (skip the dimensions)
      std::vector<uint8_t> mapTiles(completeMapData.begin() + 4,
                                    completeMapData.begin() + 4 +
                                        (width * height));
      gameState_->addMapChunk(mapTiles);

      mapComplete = true;
      return;
    }
  }

  // If binary format detection failed, try ASCII format
  // In ASCII format, each line is typically a row in the map
  // We'll count newlines to determine height, and the longest line for width

  // First, check if we have ASCII data (look for printable chars and newlines)
  bool isAscii = true;
  for (auto byte : completeMapData) {
    // Check for non-printable, non-whitespace characters
    if (byte != '\n' && byte != '\r' && (byte < 32 || byte > 126)) {
      isAscii = false;
      break;
    }
  }

  if (isAscii) {
    // Convert to string for easier line processing
    std::string mapStr(completeMapData.begin(), completeMapData.end());
    std::vector<std::string> lines;

    // Split into lines
    size_t pos = 0;
    size_t prev = 0;
    while ((pos = mapStr.find('\n', prev)) != std::string::npos) {
      lines.push_back(mapStr.substr(prev, pos - prev));
      prev = pos + 1;
    }
    // Add the last line if it exists
    if (prev < mapStr.length()) {
      lines.push_back(mapStr.substr(prev));
    }

    // Find the width (longest line) and height (number of lines)
    uint16_t height = static_cast<uint16_t>(lines.size());
    uint16_t width = 0;
    for (const auto &line : lines) {
      width = std::max(width, static_cast<uint16_t>(line.length()));
    }

    if (height > 0 && width > 0) {
      debugPrint("ASCII map format detected: " + std::to_string(width) + "x" +
                 std::to_string(height));

      // Set dimensions
      gameState_->setMapDimensions(width, height);

      // Convert ASCII map to binary tile format
      std::vector<uint8_t> mapTiles;
      mapTiles.reserve(width * height);

      for (const auto &line : lines) {
        for (size_t i = 0; i < width; ++i) {
          if (i < line.length()) {
            // Convert ASCII characters to tile types
            switch (line[i]) {
            case '#':
              mapTiles.push_back(protocol::WALL);
              break;
            case 'C':
            case 'c':
            case 'o':
            case 'O':
              mapTiles.push_back(protocol::COIN);
              break;
            case 'E':
            case 'e':
            case 'X':
            case 'x':
              mapTiles.push_back(protocol::ELECTRIC);
              break;
            default:
              mapTiles.push_back(protocol::EMPTY);
              break;
            }
          } else {
            // Pad shorter lines with empty tiles
            mapTiles.push_back(protocol::EMPTY);
          }
        }
      }

      gameState_->addMapChunk(mapTiles);
      mapComplete = true;
      return;
    }
  }

  // If we can't determine the format, make a simple guess based on the data
  // Assume it's a square or reasonable rectangle
  uint16_t size = static_cast<uint16_t>(std::sqrt(completeMapData.size()));
  uint16_t width = size;
  uint16_t height = size;

  // Try to find more suitable dimensions if not a perfect square
  if (width * height != completeMapData.size()) {
    // Look for a reasonable width that divides the total size
    for (uint16_t w = 1; w <= completeMapData.size(); ++w) {
      if (completeMapData.size() % w == 0) {
        width = w;
        height = static_cast<uint16_t>(completeMapData.size() / w);
        // Stop if we found reasonable dimensions (width wider than height)
        if (width >= height)
          break;
      }
    }
  }

  debugPrint("Map format unknown, using best guess dimensions: " +
             std::to_string(width) + "x" + std::to_string(height));

  gameState_->setMapDimensions(width, height);
  gameState_->addMapChunk(completeMapData);

  mapComplete = true;
}

void ProtocolHandlers::debugPrint(const std::string &message) {
  debug::logToFile("Protocol", message, debugMode_);
}

} // namespace network
} // namespace jetpack