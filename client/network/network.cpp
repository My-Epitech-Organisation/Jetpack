// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-jetpack-santiago.pidcova
** File description:
** Network implementation for Jetpack client
*/

#include "network.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <thread>

namespace jetpack {
namespace network {

Network::Network(const std::string &host, int port, bool debugMode,
                 GameState *gameState)
    : host_(host), port_(port), debugMode_(debugMode), socket_(-1),
      running_(false), gameState_(gameState) {}

Network::~Network() {
  stop();
  if (socket_ >= 0) {
    close(socket_);
  }
}

bool Network::connect() {
  struct sockaddr_in server_addr;
  struct hostent *server;

  // Create socket
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ < 0) {
    std::cerr << "Error creating socket" << std::endl;
    return false;
  }

  // Get server by hostname
  server = gethostbyname(host_.c_str());
  if (server == nullptr) {
    std::cerr << "Error: No such host" << std::endl;
    close(socket_);
    socket_ = -1;
    return false;
  }

  // Set up server address
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  server_addr.sin_port = htons(port_);

  // Connect to server
  if (::connect(socket_, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    std::cerr << "Error connecting to server" << std::endl;
    close(socket_);
    socket_ = -1;
    return false;
  }

  // Send CLIENT_CONNECT packet
  std::vector<uint8_t> payload;
  uint8_t reqPlayerID = 0; // Let server assign ID
  uint8_t nameLen = 5;
  const char *name = "Guest";

  payload.push_back(reqPlayerID);
  payload.push_back(nameLen);
  payload.insert(payload.end(), name, name + nameLen);

  if (!sendPacket(protocol::CLIENT_CONNECT, payload)) {
    std::cerr << "Failed to send connect packet" << std::endl;
    close(socket_);
    socket_ = -1;
    return false;
  }

  debugPrint("Sent CLIENT_CONNECT packet");
  return true;
}

void Network::disconnect() {
  if (socket_ >= 0) {
    // Send CLIENT_DISCONNECT packet
    sendPacket(protocol::CLIENT_DISCONNECT, {});
    close(socket_);
    socket_ = -1;
  }
}

void Network::run() {
  if (socket_ < 0) {
    if (!connect()) {
      return;
    }
  }

  running_ = true;
  networkThread_ = std::thread([this]() {
    protocol::PacketHeader header;
    std::vector<uint8_t> payload;

    while (running_) {
      // Receive packet from server
      if (receivePacket(&header, &payload)) {
        switch (header.type) {
        case protocol::SERVER_WELCOME:
          handleServerWelcome(payload);
          break;
        case protocol::MAP_CHUNK:
          handleMapChunk(payload);
          break;
        case protocol::GAME_START:
          handleGameStart(payload);
          break;
        case protocol::GAME_STATE:
          handleGameState(payload);
          break;
        case protocol::GAME_END:
          handleGameEnd(payload);
          break;
        case protocol::DEBUG_INFO:
          handleDebugInfo(payload);
          break;
        default:
          debugPrint("Received unknown packet type: " +
                     std::to_string(header.type));
        }
      }

      // Send player input if game is running
      if (gameState_->isGameRunning()) {
        sendPlayerInput();
      }

      // Sleep to avoid busy waiting
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });
}

void Network::stop() {
  running_ = false;
  if (networkThread_.joinable()) {
    networkThread_.join();
  }
}

bool Network::sendPacket(protocol::PacketType type,
                         const std::vector<uint8_t> &payload) {
  if (socket_ < 0)
    return false;

  protocol::PacketHeader header;
  header.magic = protocol::MAGIC_BYTE;
  header.type = type;
  header.length = htons(payload.size() + sizeof(header));

  // Send header
  if (send(socket_, &header, sizeof(header), 0) != sizeof(header)) {
    return false;
  }

  // Send payload if any
  if (!payload.empty()) {
    if (send(socket_, payload.data(), payload.size(), 0) !=
        static_cast<ssize_t>(payload.size())) {
      return false;
    }
  }

  if (debugMode_) {
    debugPrint("Sent packet: Type=" + std::to_string(type) +
               ", Length=" + std::to_string(payload.size() + sizeof(header)));
  }

  return true;
}

bool Network::receivePacket(protocol::PacketHeader *header,
                            std::vector<uint8_t> *payload) {
  if (socket_ < 0)
    return false;

  // Receive header
  ssize_t bytesRead = recv(socket_, header, sizeof(*header), 0);
  if (bytesRead != sizeof(*header)) {
    return false;
  }

  // Check magic byte
  if (header->magic != protocol::MAGIC_BYTE) {
    debugPrint("Invalid magic byte: " + std::to_string(header->magic));
    return false;
  }

  // Get payload length
  uint16_t packetLength = ntohs(header->length);
  if (packetLength < sizeof(*header)) {
    debugPrint("Invalid packet length: " + std::to_string(packetLength));
    return false;
  }

  // Compute payload length
  uint16_t payloadLength = packetLength - sizeof(*header);

  if (payloadLength > 0) {
    // Resize payload vector
    payload->resize(payloadLength);

    // Receive payload
    bytesRead = recv(socket_, payload->data(), payloadLength, 0);
    if (bytesRead != static_cast<ssize_t>(payloadLength)) {
      debugPrint("Failed to receive full payload");
      return false;
    }
  } else {
    // No payload
    payload->clear();
  }

  if (debugMode_) {
    debugPrint("Received packet: Type=" + std::to_string(header->type) +
               ", Length=" + std::to_string(packetLength));
  }

  return true;
}

void Network::handleServerWelcome(const std::vector<uint8_t> &payload) {
  if (payload.size() < 2) {
    debugPrint("SERVER_WELCOME: Invalid payload size");
    return;
  }

  uint8_t acceptCode = payload[0];
  uint8_t assignedId = payload[1];

  if (acceptCode == 1) {
    debugPrint("SERVER_WELCOME: Accepted, Assigned ID=" +
               std::to_string(assignedId));
    gameState_->setConnected(true);
    gameState_->setAssignedId(assignedId);
  } else {
    debugPrint("SERVER_WELCOME: Rejected");
    gameState_->setConnected(false);
  }
}

void Network::handleMapChunk(const std::vector<uint8_t> &payload) {
  if (payload.size() < 4) {
    debugPrint("MAP_CHUNK: Invalid payload size");
    return;
  }

  uint16_t chunkIndex = (payload[0] << 8) | payload[1];
  uint16_t chunkCount = (payload[2] << 8) | payload[3];

  debugPrint("MAP_CHUNK: Index=" + std::to_string(chunkIndex) +
             ", Count=" + std::to_string(chunkCount));

  if (chunkIndex == 0) {
    // First chunk - assume it contains dimensions
    if (payload.size() < 8) {
      debugPrint("MAP_CHUNK: First chunk too small");
      return;
    }

    uint16_t width = (payload[4] << 8) | payload[5];
    uint16_t height = (payload[6] << 8) | payload[7];

    debugPrint("Map dimensions: " + std::to_string(width) + "x" +
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

void Network::handleGameStart(const std::vector<uint8_t> &payload) {
  if (payload.size() < 5) {
    debugPrint("GAME_START: Invalid payload size");
    return;
  }

  uint8_t playerCount = payload[0];
  uint16_t startX = (payload[1] << 8) | payload[2];
  uint16_t startY = (payload[3] << 8) | payload[4];

  debugPrint("GAME_START: Players=" + std::to_string(playerCount) +
             ", Start=(" + std::to_string(startX) + "," +
             std::to_string(startY) + ")");

  gameState_->setGameRunning(true);
}

void Network::handleGameState(const std::vector<uint8_t> &payload) {
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
  if (payload.size() < static_cast<size_t>(5 + numPlayers * 8)) {
    debugPrint("GAME_STATE: Not enough data for player states");
    return;
  }

  std::vector<protocol::PlayerState> playerStates;
  for (int i = 0; i < numPlayers; i++) {
    int offset = 5 + i * 8;
    protocol::PlayerState state;

    state.id = payload[offset];
    state.posX = (payload[offset + 1] << 8) | payload[offset + 2];
    state.posY = (payload[offset + 3] << 8) | payload[offset + 4];
    state.score = (payload[offset + 5] << 8) | payload[offset + 6];
    state.alive = payload[offset + 7];

    playerStates.push_back(state);

    debugPrint("Player " + std::to_string(state.id) + ": Pos=(" +
               std::to_string(state.posX) + "," + std::to_string(state.posY) +
               "), Score=" + std::to_string(state.score) +
               ", Alive=" + std::to_string(state.alive));
  }

  gameState_->setPlayerStates(playerStates);
}

void Network::handleGameEnd(const std::vector<uint8_t> &payload) {
  if (payload.size() < 2) {
    debugPrint("GAME_END: Invalid payload size");
    return;
  }

  uint8_t reasonCode = payload[0];
  uint8_t winnerId = payload[1];

  debugPrint("GAME_END: Reason=" + std::to_string(reasonCode) +
             ", Winner=" + std::to_string(winnerId));

  gameState_->setGameRunning(false);
  gameState_->setGameEnded(true, winnerId);
}

void Network::handleDebugInfo(const std::vector<uint8_t> &payload) {
  if (payload.size() < 2) {
    debugPrint("DEBUG_INFO: Invalid payload size");
    return;
  }

  uint16_t debugLen = (payload[0] << 8) | payload[1];

  if (payload.size() < static_cast<size_t>(2 + debugLen)) {
    debugPrint("DEBUG_INFO: Not enough data");
    return;
  }

  std::string debugMsg(payload.begin() + 2, payload.begin() + 2 + debugLen);
  debugPrint("DEBUG_INFO: " + debugMsg);
}

void Network::sendPlayerInput() {
  std::vector<uint8_t> payload;
  uint8_t playerId = gameState_->getAssignedId();
  uint8_t inputMask = gameState_->getInputMask();

  payload.push_back(playerId);
  payload.push_back(inputMask);

  sendPacket(protocol::CLIENT_INPUT, payload);
}

void Network::debugPrint(const std::string &message) {
  if (debugMode_) {
    std::cout << "[Network] " << message << std::endl;
  }
}

} // namespace network
} // namespace jetpack
