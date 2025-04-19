// filepath:
// /home/psalmon/Documents/Sem4/Jetpack/Jetpack/client/graphics/renderer.cpp
// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Renderer implementation for Jetpack client
*/

#include "renderer.hpp"
#include <sstream>

namespace jetpack {
namespace graphics {

Renderer::Renderer(GameState *gameState, bool debugMode)
    : gameState_(gameState), debugMode_(debugMode), font_(nullptr) {}

bool Renderer::initialize(sf::Font &font) {
  // Store the font reference for later use in rendering text
  font_ = &font;

  // Initialize shapes with their properties
  playerShape_.setRadius(PLAYER_RADIUS);
  playerShape_.setFillColor(sf::Color::Green);
  playerShape_.setOrigin(PLAYER_RADIUS, PLAYER_RADIUS);

  wallShape_.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
  wallShape_.setFillColor(sf::Color(128, 128, 128));

  coinShape_.setRadius(COIN_RADIUS);
  coinShape_.setFillColor(sf::Color::Yellow);
  coinShape_.setOrigin(COIN_RADIUS, COIN_RADIUS);

  electricShape_.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
  electricShape_.setFillColor(sf::Color(255, 0, 255));

  return true;
}

void Renderer::render(sf::RenderWindow *window) {
  if (!window || !window->isOpen() || !font_)
    return;

  window->clear(sf::Color(50, 50, 50));

  if (gameState_->isConnected()) {
    renderMap(window);
    renderPlayers(window);
    renderUI(window, *font_);
  } else {
    renderConnectingMessage(window, *font_);
  }

  if (debugMode_) {
    renderDebugInfo(window, *font_);
  }

  window->display();
}

void Renderer::renderMap(sf::RenderWindow *window) {
  auto mapData = gameState_->getMapData();
  auto [width, height] = gameState_->getMapDimensions();
  auto players = gameState_->getPlayerStates();

  if (mapData.empty() || width == 0 || height == 0 || players.empty()) {
    return;
  }

  float viewWidth = window->getSize().x;

  // Scale map coordinates (x100)
  float scaleFactor = 100.0f;

  // No camera offset, map stays fixed in place
  float cameraOffsetX = 0.0f;

  for (uint16_t y = 0; y < height; y++) {
    for (uint16_t x = 0; x < width; x++) {
      uint16_t index = y * width + x;
      if (index >= mapData.size())
        continue;

      uint8_t tileType = mapData[index];

      // Scale positions (multiply by 100)
      float posX = x * scaleFactor - cameraOffsetX;
      float posY = y * scaleFactor;

      // Only draw tiles that are visible on screen
      if (posX < -TILE_SIZE || posX > viewWidth)
        continue;

      switch (tileType) {
      case protocol::WALL:
        wallShape_.setPosition(posX, posY);
        window->draw(wallShape_);
        break;

      case protocol::COIN:
        coinShape_.setPosition(posX + TILE_SIZE / 2, posY + TILE_SIZE / 2);
        window->draw(coinShape_);
        break;

      case protocol::ELECTRIC:
        electricShape_.setPosition(posX, posY);
        window->draw(electricShape_);
        break;

      default:
        break;
      }
    }
  }
}

void Renderer::renderPlayers(sf::RenderWindow *window) {
  auto players = gameState_->getPlayerStates();
  auto [width, height] = gameState_->getMapDimensions();
  if (players.empty() || width == 0 || height == 0 || !font_) {
    return;
  }

  float viewWidth = window->getSize().x;

  for (const auto &player : players) {
    if (player.alive == 0)
      continue;

    // Use player coordinates directly without any camera offset
    float posX = player.posX;
    float posY = player.posY;

    // Only draw players that are visible on screen
    if (posX < -PLAYER_RADIUS || posX > viewWidth + PLAYER_RADIUS)
      continue;

    // Set color based on player ID
    if (player.id == gameState_->getAssignedId()) {
      playerShape_.setFillColor(sf::Color::Green);
    } else {
      playerShape_.setFillColor(
          sf::Color(255, 128, 0)); // Orange for other players
    }

    playerShape_.setPosition(posX, posY);
    window->draw(playerShape_);

    sf::Text idText;
    idText.setFont(*font_);
    idText.setString(std::to_string(player.id));
    idText.setCharacterSize(12);
    idText.setFillColor(sf::Color::White);
    idText.setPosition(posX - 5, posY - 25);
    window->draw(idText);

    sf::Text scoreText;
    scoreText.setFont(*font_);
    scoreText.setString("Score: " + std::to_string(player.score));
    scoreText.setCharacterSize(12);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(posX - 30, posY + 15);
    window->draw(scoreText);
  }
}

void Renderer::renderUI(sf::RenderWindow *window, const sf::Font &font) {
  sf::Text statusText;
  statusText.setFont(font);
  if (gameState_->hasGameEnded()) {
    uint8_t winnerId = gameState_->getWinnerId();
    if (winnerId == 0xFF) {
      statusText.setString("Game Over - No Winner");
    } else if (winnerId == gameState_->getAssignedId()) {
      statusText.setString("Game Over - You Win!");
    } else {
      statusText.setString("Game Over - Player " + std::to_string(winnerId) +
                           " Wins!");
    }
    statusText.setFillColor(sf::Color::Yellow);
  } else if (gameState_->isGameRunning()) {
    statusText.setString("Game Running - Tick: " +
                         std::to_string(gameState_->getCurrentTick()));
    statusText.setFillColor(sf::Color::White);
  } else {
    statusText.setString("Waiting for game to start...");
    statusText.setFillColor(sf::Color::White);
  }
  statusText.setCharacterSize(16);
  statusText.setPosition(10, 10);
  window->draw(statusText);

  sf::Text controlsText;
  controlsText.setFont(font);
  controlsText.setString("Controls: Space = Jetpack");
  controlsText.setCharacterSize(14);
  controlsText.setFillColor(sf::Color(200, 200, 200));
  controlsText.setPosition(10, window->getSize().y - 25);
  window->draw(controlsText);
}

void Renderer::renderDebugInfo(sf::RenderWindow *window, const sf::Font &font) {
  sf::Text debugText;
  debugText.setFont(font);
  std::stringstream ss;

  ss << "DEBUG INFO:" << std::endl
     << "Connected: " << (gameState_->isConnected() ? "Yes" : "No") << std::endl
     << "Player ID: " << static_cast<int>(gameState_->getAssignedId())
     << std::endl
     << "Game Running: " << (gameState_->isGameRunning() ? "Yes" : "No")
     << std::endl
     << "Current Tick: " << gameState_->getCurrentTick() << std::endl;

  debugText.setString(ss.str());
  debugText.setCharacterSize(12);
  debugText.setFillColor(sf::Color::Green);
  debugText.setPosition(window->getSize().x - 200, 10);
  window->draw(debugText);
}

void Renderer::renderConnectingMessage(sf::RenderWindow *window,
                                       const sf::Font &font) {
  sf::Text text;
  text.setFont(font);
  text.setString("Connecting to server...");
  text.setCharacterSize(24);
  text.setFillColor(sf::Color::White);
  text.setPosition(window->getSize().x / 2 - text.getLocalBounds().width / 2,
                   window->getSize().y / 2 - text.getLocalBounds().height / 2);
  window->draw(text);
}

} // namespace graphics
} // namespace jetpack