// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-jetpack-santiago.pidcova
** File description:
** Graphics implementation for Jetpack client
*/

#include "graphics.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace jetpack {
namespace graphics {

Graphics::Graphics(GameState* gameState, bool debugMode)
    : window_(sf::VideoMode(800, 600), "Jetpack Client"),
      gameState_(gameState), debugMode_(debugMode), running_(false) {
  initialize();
}

Graphics::~Graphics() {
  stop();
}

void Graphics::initialize() {
  // Set framerate limit
  window_.setFramerateLimit(60);

  // Load font
  if (!font_.loadFromFile("assets/jetpack_font.ttf")) {
    std::cerr << "Failed to load font" << std::endl;
    // Proceed without font
  }

  // Initialize player shape (green circle)
  playerShape_.setRadius(PLAYER_RADIUS);
  playerShape_.setFillColor(sf::Color::Green);
  playerShape_.setOrigin(PLAYER_RADIUS, PLAYER_RADIUS);

  // Initialize wall shape (red square)
  wallShape_.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
  wallShape_.setFillColor(sf::Color::Red);

  // Initialize coin shape (yellow circle)
  coinShape_.setRadius(COIN_RADIUS);
  coinShape_.setFillColor(sf::Color::Yellow);
  coinShape_.setOrigin(COIN_RADIUS, COIN_RADIUS);

  // Initialize electric shape (blue rectangle)
  electricShape_.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
  electricShape_.setFillColor(sf::Color::Blue);
}

void Graphics::run() {
  running_ = true;
  graphicsThread_ = std::thread([this]() {
    while (running_ && window_.isOpen()) {
      processEvents();
      update();
      render();
    }
  });
}

void Graphics::stop() {
  running_ = false;
  if (graphicsThread_.joinable()) {
    graphicsThread_.join();
  }
  if (window_.isOpen()) {
    window_.close();
  }
}

bool Graphics::isRunning() const {
  return running_ && window_.isOpen();
}

void Graphics::processEvents() {
  sf::Event event;
  while (window_.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window_.close();
    } else if (event.type == sf::Event::KeyPressed) {
      handleKeyPress(event.key.code, true);
    } else if (event.type == sf::Event::KeyReleased) {
      handleKeyPress(event.key.code, false);
    }
  }
}

void Graphics::update() {
  // Update is minimal since most game state is managed by the server
}

void Graphics::render() {
  window_.clear(sf::Color(50, 50, 50));  // Dark gray background

  if (gameState_->isConnected()) {
    renderMap();
    renderPlayers();
    renderUI();
  } else {
    // Display connecting message
    sf::Text text;
    text.setFont(font_);
    text.setString("Connecting to server...");
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setPosition(
        window_.getSize().x / 2 - text.getLocalBounds().width / 2,
        window_.getSize().y / 2 - text.getLocalBounds().height / 2);
    window_.draw(text);
  }

  if (debugMode_) {
    renderDebugInfo();
  }

  window_.display();
}

void Graphics::renderMap() {
  auto mapData = gameState_->getMapData();
  auto [width, height] = gameState_->getMapDimensions();

  if (mapData.empty() || width == 0 || height == 0) {
    // Map not received yet
    return;
  }

  // Calculate view scaling to fit the map
  float viewWidth = window_.getSize().x;
  float viewHeight = window_.getSize().y;
  float mapWidth = width * TILE_SIZE;
  float mapHeight = height * TILE_SIZE;

  // Camera offset to center the map
  float offsetX = (viewWidth - mapWidth) / 2;
  float offsetY = (viewHeight - mapHeight) / 2;

  // Render map elements
  for (uint16_t y = 0; y < height; y++) {
    for (uint16_t x = 0; x < width; x++) {
      uint16_t index = y * width + x;
      if (index >= mapData.size()) continue;

      uint8_t tileType = mapData[index];
      float posX = offsetX + x * TILE_SIZE;
      float posY = offsetY + y * TILE_SIZE;

      switch (tileType) {
        case protocol::WALL:
          wallShape_.setPosition(posX, posY);
          window_.draw(wallShape_);
          break;

        case protocol::COIN:
          coinShape_.setPosition(posX + TILE_SIZE / 2, posY + TILE_SIZE / 2);
          window_.draw(coinShape_);
          break;

        case protocol::ELECTRIC:
          electricShape_.setPosition(posX, posY);
          window_.draw(electricShape_);
          break;

        default:  // EMPTY or unknown
          break;
      }
    }
  }
}

void Graphics::renderPlayers() {
  auto players = gameState_->getPlayerStates();
  auto [width, height] = gameState_->getMapDimensions();

  if (players.empty() || width == 0 || height == 0) {
    return;
  }

  // Calculate view scaling
  float viewWidth = window_.getSize().x;
  float viewHeight = window_.getSize().y;
  float mapWidth = width * TILE_SIZE;
  float mapHeight = height * TILE_SIZE;

  // Camera offset to center the map
  float offsetX = (viewWidth - mapWidth) / 2;
  float offsetY = (viewHeight - mapHeight) / 2;

  // Render each player
  for (const auto& player : players) {
    if (player.alive == 0) continue;  // Skip dead players

    // Convert player coordinates to screen coordinates
    float screenX = offsetX + player.posX * TILE_SIZE / 100.0f;
    float screenY = offsetY + player.posY * TILE_SIZE / 100.0f;

    // Set player color - our player is green, others are different colors
    if (player.id == gameState_->getAssignedId()) {
      playerShape_.setFillColor(sf::Color::Green);
    } else {
      playerShape_.setFillColor(sf::Color(255, 128, 0));  // Orange
    }

    playerShape_.setPosition(screenX, screenY);
    window_.draw(playerShape_);

    // Draw player ID and score
    sf::Text idText;
    idText.setFont(font_);
    idText.setString(std::to_string(player.id));
    idText.setCharacterSize(12);
    idText.setFillColor(sf::Color::White);
    idText.setPosition(screenX - 5, screenY - 25);
    window_.draw(idText);

    sf::Text scoreText;
    scoreText.setFont(font_);
    scoreText.setString("Score: " + std::to_string(player.score));
    scoreText.setCharacterSize(12);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(screenX - 30, screenY + 15);
    window_.draw(scoreText);
  }
}

void Graphics::renderUI() {
  // Render game status at the top of the screen
  sf::Text statusText;
  statusText.setFont(font_);

  if (gameState_->hasGameEnded()) {
    uint8_t winnerId = gameState_->getWinnerId();
    if (winnerId == 0xFF) {
      statusText.setString("Game Over - No Winner");
    } else if (winnerId == gameState_->getAssignedId()) {
      statusText.setString("Game Over - You Win!");
    } else {
      statusText.setString("Game Over - Player " +
                          std::to_string(winnerId) + " Wins!");
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
  window_.draw(statusText);

  // Show control info at the bottom
  sf::Text controlsText;
  controlsText.setFont(font_);
  controlsText.setString("Controls: Left/Right Arrows, Space = Jetpack");
  controlsText.setCharacterSize(14);
  controlsText.setFillColor(sf::Color(200, 200, 200));
  controlsText.setPosition(10, window_.getSize().y - 25);
  window_.draw(controlsText);
}

void Graphics::renderDebugInfo() {
  sf::Text debugText;
  debugText.setFont(font_);
  std::stringstream ss;

  ss << "DEBUG INFO:" << std::endl
     << "Connected: " << (gameState_->isConnected() ? "Yes" : "No") << std::endl
     << "Player ID: " << static_cast<int>(gameState_->getAssignedId()) << std::endl
     << "Game Running: " << (gameState_->isGameRunning() ? "Yes" : "No") << std::endl
     << "Current Tick: " << gameState_->getCurrentTick() << std::endl
     << "Input Mask: 0x" << std::hex << std::setw(2) << std::setfill('0')
     << static_cast<int>(gameState_->getInputMask());

  debugText.setString(ss.str());
  debugText.setCharacterSize(12);
  debugText.setFillColor(sf::Color::Green);
  debugText.setPosition(window_.getSize().x - 200, 10);
  window_.draw(debugText);
}

void Graphics::handleKeyPress(sf::Keyboard::Key key, bool isPressed) {
  uint8_t currentMask = gameState_->getInputMask();
  uint8_t newMask = currentMask;

  switch (key) {
    case sf::Keyboard::Left:
      if (isPressed) {
        newMask |= protocol::MOVE_LEFT;
      } else {
        newMask &= ~protocol::MOVE_LEFT;
      }
      break;

    case sf::Keyboard::Right:
      if (isPressed) {
        newMask |= protocol::MOVE_RIGHT;
      } else {
        newMask &= ~protocol::MOVE_RIGHT;
      }
      break;

    case sf::Keyboard::Space:
      if (isPressed) {
        newMask |= protocol::JETPACK;
      } else {
        newMask &= ~protocol::JETPACK;
      }
      break;

    case sf::Keyboard::Escape:
      window_.close();
      break;

    default:
      return;
  }

  if (newMask != currentMask) {
    gameState_->setInputMask(newMask);
  }
}

}  // namespace graphics
}  // namespace jetpack
