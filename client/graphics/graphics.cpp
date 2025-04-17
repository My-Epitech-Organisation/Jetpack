// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Graphics implementation for Jetpack client
*/

#include "graphics.hpp"
#include "../debug/debug.hpp"
#include <SFML/Graphics.hpp>
#include <atomic>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

namespace jetpack {
namespace graphics {

Graphics::Graphics(GameState *gameState, bool debugMode)
    : window_(nullptr), gameState_(gameState), debugMode_(debugMode),
      running_(false), graphicsInitialized_(false),
      onWindowClosedCallback_(nullptr) {
  // Defer window creation to the run() method
}

Graphics::~Graphics() { stop(); }

bool Graphics::initializeWindow() {
  try {
    window_ = std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 600),
                                                 "Jetpack Client");
    window_->setFramerateLimit(60);
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Failed to create SFML window: " << e.what() << std::endl;
    return false;
  }
}

bool Graphics::initializeResources() {
  // Load font
  if (!font_.loadFromFile("assets/jetpack_font.ttf")) {
    try {
      if (!font_.loadFromFile("/usr/share/fonts/truetype/liberation/"
                              "LiberationSans-Regular.ttf")) {
        std::cerr << "Failed to load fallback font" << std::endl;
        return false;
      }
    } catch (const std::exception &e) {
      std::cerr << "Error loading fallback font: " << e.what() << std::endl;
      return false;
    }
  }

  // Initialize shapes
  playerShape_.setRadius(PLAYER_RADIUS);
  playerShape_.setFillColor(sf::Color::Green);
  playerShape_.setOrigin(PLAYER_RADIUS, PLAYER_RADIUS);

  wallShape_.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
  wallShape_.setFillColor(sf::Color(128, 128, 128)); // Gray

  coinShape_.setRadius(COIN_RADIUS);
  coinShape_.setFillColor(sf::Color::Yellow);
  coinShape_.setOrigin(COIN_RADIUS, COIN_RADIUS);

  electricShape_.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
  electricShape_.setFillColor(sf::Color(255, 0, 255)); // Purple

  return true;
}

void Graphics::run() {
  running_ = true;
  graphicsThread_ = std::thread([this]() {
    // Try to initialize graphics in the thread
    graphicsInitialized_ = initializeWindow();

    if (graphicsInitialized_) {
      // Only initialize resources if window was created successfully
      initializeResources();

      // Main render loop
      while (running_ && window_ && window_->isOpen()) {
        processEvents();
        update();
        render();
      }

      // If the window was manually closed and a callback is defined, execute it
      if (!window_->isOpen() && onWindowClosedCallback_) {
        debug::print("Graphics",
                     "Window was closed, calling window closed callback",
                     debugMode_);
        onWindowClosedCallback_();
      }
    } else {
      std::cerr << "Graphics initialization failed - running in headless mode"
                << std::endl;
      // Keep the thread alive but do nothing
      while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }
  });
}

void Graphics::stop() {
  running_ = false;
  if (graphicsThread_.joinable()) {
    graphicsThread_.join();
  }
  if (window_) {
    window_->close();
    window_.reset();
  }
}

bool Graphics::isRunning() const { return running_; }

void Graphics::setOnWindowClosedCallback(std::function<void()> callback) {
  onWindowClosedCallback_ = callback;
}

void Graphics::processEvents() {
  if (!window_)
    return;

  sf::Event event;
  while (window_->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      debug::print("Graphics", "Window closed event received", debugMode_);
      window_->close();
      running_ = false;
      // Call callback if defined
      if (onWindowClosedCallback_) {
        onWindowClosedCallback_();
      }
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
  if (!window_ || !window_->isOpen())
    return;

  window_->clear(sf::Color(50, 50, 50)); // Dark gray background

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
    text.setPosition(window_->getSize().x / 2 - text.getLocalBounds().width / 2,
                     window_->getSize().y / 2 -
                         text.getLocalBounds().height / 2);
    window_->draw(text);
  }

  if (debugMode_) {
    renderDebugInfo();
  }

  window_->display();
}

void Graphics::renderMap() {
  if (!window_)
    return;

  auto mapData = gameState_->getMapData();
  auto [width, height] = gameState_->getMapDimensions();

  if (mapData.empty() || width == 0 || height == 0) {
    // Map not received yet
    return;
  }

  // Calculate view scaling to fit the map
  float viewWidth = window_->getSize().x;
  float viewHeight = window_->getSize().y;
  float mapWidth = width * TILE_SIZE;
  float mapHeight = height * TILE_SIZE;

  // Camera offset to center the map
  float offsetX = (viewWidth - mapWidth) / 2;
  float offsetY = (viewHeight - mapHeight) / 2;

  // Render map elements
  for (uint16_t y = 0; y < height; y++) {
    for (uint16_t x = 0; x < width; x++) {
      uint16_t index = y * width + x;
      if (index >= mapData.size())
        continue;

      uint8_t tileType = mapData[index];
      float posX = offsetX + x * TILE_SIZE;
      float posY = offsetY + y * TILE_SIZE;

      switch (tileType) {
      case protocol::WALL:
        wallShape_.setPosition(posX, posY);
        window_->draw(wallShape_);
        break;

      case protocol::COIN:
        coinShape_.setPosition(posX + TILE_SIZE / 2, posY + TILE_SIZE / 2);
        window_->draw(coinShape_);
        break;

      case protocol::ELECTRIC:
        electricShape_.setPosition(posX, posY);
        window_->draw(electricShape_);
        break;

      default: // EMPTY or unknown
        break;
      }
    }
  }
}

void Graphics::renderPlayers() {
  if (!window_)
    return;

  auto players = gameState_->getPlayerStates();
  auto [width, height] = gameState_->getMapDimensions();

  if (players.empty() || width == 0 || height == 0) {
    return;
  }

  // Calculate view scaling
  float viewWidth = window_->getSize().x;
  float viewHeight = window_->getSize().y;
  float mapWidth = width * TILE_SIZE;
  float mapHeight = height * TILE_SIZE;

  // Camera offset to center the map
  float offsetX = (viewWidth - mapWidth) / 2;
  float offsetY = (viewHeight - mapHeight) / 2;

  // Render each player
  for (const auto &player : players) {
    if (player.alive == 0)
      continue; // Skip dead players

    // Convert player coordinates to screen coordinates
    float screenX = offsetX + player.posX * TILE_SIZE / 100.0f;
    float screenY = offsetY + player.posY * TILE_SIZE / 100.0f;

    // Set player color - our player is green, others are different colors
    if (player.id == gameState_->getAssignedId()) {
      playerShape_.setFillColor(sf::Color::Green);
    } else {
      playerShape_.setFillColor(sf::Color(255, 128, 0)); // Orange
    }

    playerShape_.setPosition(screenX, screenY);
    window_->draw(playerShape_);

    // Draw player ID and score
    sf::Text idText;
    idText.setFont(font_);
    idText.setString(std::to_string(player.id));
    idText.setCharacterSize(12);
    idText.setFillColor(sf::Color::White);
    idText.setPosition(screenX - 5, screenY - 25);
    window_->draw(idText);

    sf::Text scoreText;
    scoreText.setFont(font_);
    scoreText.setString("Score: " + std::to_string(player.score));
    scoreText.setCharacterSize(12);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(screenX - 30, screenY + 15);
    window_->draw(scoreText);
  }
}

void Graphics::renderUI() {
  if (!window_)
    return;

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
  window_->draw(statusText);

  // Show control info at the bottom
  sf::Text controlsText;
  controlsText.setFont(font_);
  controlsText.setString("Controls: Left/Right Arrows, Space = Jetpack");
  controlsText.setCharacterSize(14);
  controlsText.setFillColor(sf::Color(200, 200, 200));
  controlsText.setPosition(10, window_->getSize().y - 25);
  window_->draw(controlsText);
}

void Graphics::renderDebugInfo() {
  if (!window_)
    return;

  sf::Text debugText;
  debugText.setFont(font_);
  std::stringstream ss;

  ss << "DEBUG INFO:" << std::endl
     << "Connected: " << (gameState_->isConnected() ? "Yes" : "No") << std::endl
     << "Player ID: " << static_cast<int>(gameState_->getAssignedId())
     << std::endl
     << "Game Running: " << (gameState_->isGameRunning() ? "Yes" : "No")
     << std::endl
     << "Current Tick: " << gameState_->getCurrentTick() << std::endl
     << "Input Mask: 0x" << std::hex << std::setw(2) << std::setfill('0')
     << static_cast<int>(gameState_->getInputMask());

  debugText.setString(ss.str());
  debugText.setCharacterSize(12);
  debugText.setFillColor(sf::Color::Green);
  debugText.setPosition(window_->getSize().x - 200, 10);
  window_->draw(debugText);
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
    if (window_) {
      window_->close();
    }
    break;

  default:
    return;
  }

  if (newMask != currentMask) {
    gameState_->setInputMask(newMask);
  }
}

} // namespace graphics
} // namespace jetpack
