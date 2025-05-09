// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Thread-safe game state for Jetpack client
*/

#ifndef CLIENT_GAMESTATE_HPP_
#define CLIENT_GAMESTATE_HPP_

#include "protocol.hpp"
#include <atomic>
#include <mutex>
#include <set>
#include <utility>
#include <vector>

namespace jetpack {

class GameState {
public:
  GameState()
      : connected(false), assignedId(0), gameRunning(false),
        jetpackActive(false), mapWidth(0), mapHeight(0), currentTick(0),
        gameEnded(false), winnerId(0xFF) {}

  // Thread-safe setters
  void setConnected(bool status);
  void setAssignedId(uint8_t id);
  void setGameRunning(bool running);
  void setJetpackActive(bool active);
  void setMapDimensions(uint16_t width, uint16_t height);
  void addMapChunk(const std::vector<uint8_t> &chunkData);
  void setMapData(const std::vector<uint8_t> &mapData);
  void setPlayerStates(const std::vector<protocol::PlayerState> &states);
  void setCurrentTick(uint32_t tick);
  void setGameEnded(bool ended, uint8_t winnerId);
  void addCollectedCoinByLocalPlayer(uint16_t tileX, uint16_t tileY);
  void addCollectedCoinByOtherPlayer(uint16_t tileX, uint16_t tileY);

  // Thread-safe getters
  bool isConnected() const;
  uint8_t getAssignedId() const;
  bool isGameRunning() const;
  bool isJetpackActive() const;
  std::pair<uint16_t, uint16_t> getMapDimensions() const;
  std::vector<uint8_t> getMapData() const;
  std::vector<protocol::PlayerState> getPlayerStates() const;
  uint32_t getCurrentTick() const;
  bool hasGameEnded() const;
  uint8_t getWinnerId() const;
  bool isCoinCollectedByLocalPlayer(uint16_t tileX, uint16_t tileY) const;
  bool isCoinCollectedByOtherPlayer(uint16_t tileX, uint16_t tileY) const;

private:
  mutable std::mutex mutex_;
  std::atomic<bool> connected;
  uint8_t assignedId;
  std::atomic<bool> gameRunning;
  bool jetpackActive;

  // Map data
  uint16_t mapWidth;
  uint16_t mapHeight;
  std::vector<uint8_t> mapData;

  // Player states
  std::vector<protocol::PlayerState> playerStates;
  uint32_t currentTick;

  // Collected coins positions (tileX, tileY)
  std::set<std::pair<uint16_t, uint16_t>> coinsCollectedByLocalPlayer;
  std::set<std::pair<uint16_t, uint16_t>> coinsCollectedByOtherPlayers;

  // Game end state
  bool gameEnded;
  uint8_t winnerId;
};

} // namespace jetpack

#endif // CLIENT_GAMESTATE_HPP_
