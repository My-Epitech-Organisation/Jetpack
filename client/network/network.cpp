// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Network implementation for Jetpack client
*/

#include "network.hpp"
#include "../debug/debug.hpp"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <thread>

namespace jetpack {
namespace network {

Network::Network(const std::string &host, int port, bool debugMode,
                 GameState *gameState)
    : host_(host), port_(port), debugMode_(debugMode), socket_(-1),
      running_(false), gameState_(gameState) {
  protocolHandlers_ = std::make_unique<ProtocolHandlers>(gameState, debugMode);

  pfd_.fd = -1;
  pfd_.events = POLLIN;
  pfd_.revents = 0;
}

Network::~Network() {
  stop();
  if (socket_ >= 0) {
    debug::logToFile("Network", "Closing socket", debugMode_);
    close(socket_);
  }
}

bool Network::connect() {
  struct sockaddr_in server_addr;
  struct hostent *server;

  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ < 0) {
    std::cerr << "Error creating socket" << std::endl;
    return false;
  }

  debug::logToFile("Network", "Socket created", debugMode_);

  server = gethostbyname(host_.c_str());
  if (server == nullptr) {
    std::cerr << "Error: No such host" << std::endl;
    close(socket_);
    socket_ = -1;
    return false;
  }

  debug::logToFile("Network", "Hostname resolved", debugMode_);

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  server_addr.sin_port = htons(port_);

  debug::logToFile("Network",
                   "Connecting to " + host_ + ":" + std::to_string(port_),
                   debugMode_);
  if (::connect(socket_, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    std::cerr << "Error connecting to server" << std::endl;
    close(socket_);
    socket_ = -1;
    return false;
  }

  debug::logToFile("Network", "Connected successfully", debugMode_);

  pfd_.fd = socket_;
  pfd_.events = POLLIN;

  std::vector<uint8_t> payload;
  uint8_t reqPlayerID = 0;
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

  debug::logToFile("Network",
                   "CLIENT_CONNECT sent with name: " + std::string(name),
                   debugMode_);
  return true;
}

void Network::disconnect() {
  if (socket_ >= 0) {
    debug::logToFile("Network", "Sending CLIENT_DISCONNECT", debugMode_);
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
    auto lastInputTime = std::chrono::steady_clock::now();

    debug::logToFile("Network", "Network thread started", debugMode_);

    while (running_) {
      int pollResult = poll(&pfd_, 1, 50); // Timeout de 50ms

      if (pollResult < 0) {
        debug::logToFile("Network",
                         "Error in poll(): " + std::string(strerror(errno)),
                         debugMode_);
        break;
      } else if (pollResult > 0) {
        if (pfd_.revents & POLLIN) {
          if (receivePacket(&header, &payload)) {
            protocol::PacketType packetType =
                static_cast<protocol::PacketType>(header.type);
            switch (packetType) {
            case protocol::SERVER_WELCOME:
              protocolHandlers_->handleServerWelcome(payload);
              break;
            case protocol::MAP_CHUNK:
              protocolHandlers_->handleMapChunk(payload);
              break;
            case protocol::GAME_START:
              protocolHandlers_->handleGameStart(payload);
              break;
            case protocol::GAME_STATE:
              protocolHandlers_->handleGameState(payload);
              break;
            case protocol::GAME_END:
              protocolHandlers_->handleGameEnd(payload);
              break;
            case protocol::DEBUG_INFO:
              protocolHandlers_->handleDebugInfo(payload);
              break;
            default:
              debug::print("Network",
                           "Received unknown packet type: " +
                               std::to_string(header.type),
                           debugMode_);
            }
          }
        }
        if (pfd_.revents & (POLLHUP | POLLERR)) {
          debug::logToFile("Network", "Socket error or disconnect detected",
                           debugMode_);
          break;
        }
      }

      auto now = std::chrono::steady_clock::now();
      if (gameState_->isGameRunning() &&
          std::chrono::duration_cast<std::chrono::milliseconds>(now -
                                                                lastInputTime)
                  .count() > 50) { // 20Hz input rate

        sendPlayerInput();
        lastInputTime = now;
      }

      checkConnectionHealth();
    }

    debug::logToFile("Network", "Network thread exiting", debugMode_);
  });
}

void Network::stop() {
  running_ = false;
  if (networkThread_.joinable()) {
    debug::logToFile("Network", "Waiting for network thread to exit",
                     debugMode_);
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

  if (write(socket_, &header, sizeof(header)) != sizeof(header)) {
    debug::logToFile("Network",
                     "Failed to send packet header: " +
                         std::string(strerror(errno)),
                     debugMode_);
    return false;
  }

  if (!payload.empty()) {
    if (write(socket_, payload.data(), payload.size()) !=
        static_cast<ssize_t>(payload.size())) {
      debug::logToFile("Network",
                       "Failed to send packet payload: " +
                           std::string(strerror(errno)),
                       debugMode_);
      return false;
    }
  }

  if (debugMode_) {
    std::stringstream ss;
    ss << "Sent packet: Type=0x" << std::hex << static_cast<int>(type)
       << std::dec << " (" << packetTypeToString(type)
       << "), Length=" << (payload.size() + sizeof(header)) << " bytes";

    if (!payload.empty() && payload.size() <= 64) {
      ss << "\nPayload: " << debug::formatHexDump(payload);
    }
    debug::logToFile("Network", ss.str(), true);
  }

  return true;
}

bool Network::receivePacket(protocol::PacketHeader *header,
                            std::vector<uint8_t> *payload) {
  if (socket_ < 0)
    return false;

  ssize_t bytesRead = read(socket_, header, sizeof(*header));
  if (bytesRead != sizeof(*header)) {
    if (bytesRead == 0) {
      debug::logToFile("Network", "Server closed connection", debugMode_);
    } else if (bytesRead < 0) {
      debug::logToFile("Network",
                       "Error reading header: " + std::string(strerror(errno)),
                       debugMode_);
    }
    return false;
  }

  if (header->magic != protocol::MAGIC_BYTE) {
    debug::logToFile("Network",
                     "Invalid magic byte: 0x" + toHexString(header->magic),
                     debugMode_);
    return false;
  }

  uint16_t packetLength = ntohs(header->length);
  if (packetLength < sizeof(*header)) {
    debug::logToFile("Network",
                     "Invalid packet length: " + std::to_string(packetLength),
                     debugMode_);
    return false;
  }

  uint16_t payloadLength = packetLength - sizeof(*header);

  if (payloadLength > 0) {
    payload->resize(payloadLength);

    bytesRead = read(socket_, payload->data(), payloadLength);
    if (bytesRead != static_cast<ssize_t>(payloadLength)) {
      debug::logToFile("Network",
                       "Failed to receive full payload: expected " +
                           std::to_string(payloadLength) + " bytes, got " +
                           std::to_string(bytesRead),
                       debugMode_);
      return false;
    }
  } else {
    payload->clear();
  }

  if (debugMode_) {
    std::stringstream ss;
    ss << "Received packet: Type=0x" << std::hex
       << static_cast<int>(header->type) << std::dec << " ("
       << packetTypeToString(static_cast<protocol::PacketType>(header->type))
       << "), Length=" << packetLength << " bytes";

    if (!payload->empty() && payload->size() <= 64) {
      ss << "\nPayload: " << debug::formatHexDump(*payload);
    }
    debug::print("Network", ss.str(), true);
  }

  return true;
}

void Network::sendPlayerInput() {
  std::vector<uint8_t> payload;
  uint8_t playerId = gameState_->getAssignedId();
  uint8_t jetpackState = gameState_->isJetpackActive() ? 
                        protocol::JETPACK_ON : protocol::JETPACK_OFF;

  payload.push_back(playerId);
  payload.push_back(jetpackState);

  sendPacket(protocol::CLIENT_INPUT, payload);

  // Log when jetpack state changes
  static uint8_t lastJetpackState = 0xFF; // Initialize to invalid value to ensure first log
  if (jetpackState != lastJetpackState) {
    std::stringstream ss;
    ss << "Input changed: Jetpack=" << (jetpackState == protocol::JETPACK_ON ? "ON" : "OFF");
    debug::logToFile("Network", ss.str(), debugMode_);
    lastJetpackState = jetpackState;
  }
}

bool Network::sendDebugMessage(const std::string &message) {
  if (!debugMode_ || socket_ < 0 || message.empty()) {
    return false;
  }

  std::vector<uint8_t> payload;
  uint16_t msgLen = static_cast<uint16_t>(message.length());
  uint16_t msgLenNet = htons(msgLen);

  uint8_t *lenBytes = reinterpret_cast<uint8_t *>(&msgLenNet);
  payload.push_back(lenBytes[0]);
  payload.push_back(lenBytes[1]);

  payload.insert(payload.end(), message.begin(), message.end());

  return sendPacket(protocol::DEBUG_INFO, payload);
}

void Network::checkConnectionHealth() {}

std::string Network::packetTypeToString(protocol::PacketType type) {
  switch (type) {
  case protocol::CLIENT_CONNECT:
    return "CLIENT_CONNECT";
  case protocol::SERVER_WELCOME:
    return "SERVER_WELCOME";
  case protocol::MAP_CHUNK:
    return "MAP_CHUNK";
  case protocol::GAME_START:
    return "GAME_START";
  case protocol::CLIENT_INPUT:
    return "CLIENT_INPUT";
  case protocol::GAME_STATE:
    return "GAME_STATE";
  case protocol::GAME_END:
    return "GAME_END";
  case protocol::CLIENT_DISCONNECT:
    return "CLIENT_DISCONNECT";
  case protocol::DEBUG_INFO:
    return "DEBUG_INFO";
  default:
    return "UNKNOWN";
  }
}

std::string Network::toHexString(uint8_t byte) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
  return ss.str();
}

} // namespace network
} // namespace jetpack
