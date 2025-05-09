// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Implementation of thread-safe game state
*/

#include "gamestate.hpp"

namespace jetpack {

void GameState::setConnected(bool status) { connected = status; }

void GameState::setAssignedId(uint8_t id) {
  std::lock_guard<std::mutex> lock(mutex_);
  assignedId = id;
}

void GameState::setGameRunning(bool running) { gameRunning = running; }

void GameState::setJetpackActive(bool active) {
  std::lock_guard<std::mutex> lock(mutex_);
  jetpackActive = active;
}

void GameState::setMapDimensions(uint16_t width, uint16_t height) {
  std::lock_guard<std::mutex> lock(mutex_);
  mapWidth = width;
  mapHeight = height;
  mapData.resize(width * height, protocol::EMPTY);
}

void GameState::addMapChunk(const std::vector<uint8_t> &chunkData) {
  std::lock_guard<std::mutex> lock(mutex_);
  mapData.insert(mapData.end(), chunkData.begin(), chunkData.end());
}

void GameState::setMapData(const std::vector<uint8_t> &data) {
  std::lock_guard<std::mutex> lock(mutex_);
  mapData = data;
}

void GameState::setPlayerStates(
    const std::vector<protocol::PlayerState> &states) {
  std::lock_guard<std::mutex> lock(mutex_);
  playerStates = states;
}

void GameState::setCurrentTick(uint32_t tick) {
  std::lock_guard<std::mutex> lock(mutex_);
  currentTick = tick;
}

void GameState::setGameEnded(bool ended, uint8_t winId) {
  std::lock_guard<std::mutex> lock(mutex_);
  gameEnded = ended;
  winnerId = winId;
}

void GameState::addCollectedCoinByLocalPlayer(uint16_t tileX, uint16_t tileY) {
  std::lock_guard<std::mutex> lock(mutex_);
  coinsCollectedByLocalPlayer.insert({tileX, tileY});
}

void GameState::addCollectedCoinByOtherPlayer(uint16_t tileX, uint16_t tileY) {
  std::lock_guard<std::mutex> lock(mutex_);
  coinsCollectedByOtherPlayers.insert({tileX, tileY});
}

bool GameState::isCoinCollectedByLocalPlayer(uint16_t tileX, uint16_t tileY) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return coinsCollectedByLocalPlayer.find({tileX, tileY}) != coinsCollectedByLocalPlayer.end();
}

bool GameState::isCoinCollectedByOtherPlayer(uint16_t tileX, uint16_t tileY) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return coinsCollectedByOtherPlayers.find({tileX, tileY}) != coinsCollectedByOtherPlayers.end();
}

bool GameState::isConnected() const { return connected; }

uint8_t GameState::getAssignedId() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return assignedId;
}

bool GameState::isGameRunning() const { return gameRunning; }

bool GameState::isJetpackActive() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return jetpackActive;
}

std::pair<uint16_t, uint16_t> GameState::getMapDimensions() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return {mapWidth, mapHeight};
}

std::vector<uint8_t> GameState::getMapData() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return mapData;
}

std::vector<protocol::PlayerState> GameState::getPlayerStates() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return playerStates;
}

uint32_t GameState::getCurrentTick() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return currentTick;
}

bool GameState::hasGameEnded() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return gameEnded;
}

uint8_t GameState::getWinnerId() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return winnerId;
}

} // namespace jetpack
