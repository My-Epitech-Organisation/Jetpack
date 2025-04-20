// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Renderer implementation for Jetpack client
*/

#include "renderer.hpp"
#include <chrono>
#include <sstream>

namespace jetpack {
namespace graphics {

Renderer::Renderer(GameState *gameState, bool debugMode)
    : gameState_(gameState), debugMode_(debugMode), font_(nullptr),
      gameEndOverlayActive_(false), shutdownCountdownSeconds_(5),
      onCountdownEndCallback_(nullptr), cameraOffsetX_(0.0f) {

  gameView_.setSize(virtualWidth_, virtualHeight_);
  gameView_.setCenter(virtualWidth_ / 2.0f, virtualHeight_ / 2.0f);

  uiView_.setSize(virtualWidth_, virtualHeight_);
  uiView_.setCenter(virtualWidth_ / 2.0f, virtualHeight_ / 2.0f);
}

bool Renderer::initialize(sf::Font &font) {
  font_ = &font;

  playerShape_.setRadius(PLAYER_RADIUS);
  playerShape_.setFillColor(sf::Color::Green);
  playerShape_.setOrigin(PLAYER_RADIUS, PLAYER_RADIUS);

  wallShape_.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
  wallShape_.setFillColor(sf::Color(100, 100, 100));

  coinShape_.setRadius(COIN_RADIUS);
  coinShape_.setFillColor(sf::Color::Yellow);
  coinShape_.setOrigin(COIN_RADIUS, COIN_RADIUS);

  electricShape_.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
  electricShape_.setFillColor(sf::Color(230, 30, 230));

  return true;
}

void Renderer::render(sf::RenderWindow *window) {
  if (!window || !window->isOpen() || !font_)
    return;

  window->clear(sf::Color(50, 50, 50));

  if (gameState_->isConnected()) {
    updateCamera();

    window->setView(gameView_);
    renderMap(window);
    renderPlayers(window);

    window->setView(uiView_);
    renderUI(window, *font_);

    if (gameState_->hasGameEnded()) {
      if (!gameEndOverlayActive_) {
        gameEndOverlayActive_ = true;
        gameEndTime_ = std::chrono::steady_clock::now();
      }

      window->setView(uiView_);
      renderGameEndScreen(window, *font_);

      auto currentTime = std::chrono::steady_clock::now();
      auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                                currentTime - gameEndTime_)
                                .count();

      if (elapsedSeconds >= shutdownCountdownSeconds_) {
        if (onCountdownEndCallback_) {
          onCountdownEndCallback_();
        }
        window->close();
      }
    }
  } else {
    window->setView(uiView_);
    renderConnectingMessage(window, *font_);
  }

  if (debugMode_) {
    window->setView(uiView_);
    renderDebugInfo(window, *font_);
  }

  window->display();
}

void Renderer::renderMap(sf::RenderWindow *window) {
  std::pair<uint16_t, uint16_t> mapDimensions = gameState_->getMapDimensions();
  uint16_t mapWidth = mapDimensions.first;
  uint16_t mapHeight = mapDimensions.second;

  if (mapWidth == 0 || mapHeight == 0)
    return;

  std::vector<uint8_t> mapData = gameState_->getMapData();
  if (mapData.empty())
    return;

  auto players = gameState_->getPlayerStates();
  uint8_t localPlayerId = gameState_->getAssignedId();

  for (const auto &player : players) {
    if (player.collectedCoin) {
      sf::Vector2f displayPos =
          convertServerToDisplayCoords(player.posX, player.posY);
      uint16_t tileX = static_cast<uint16_t>(displayPos.x / TILE_SIZE);
      uint16_t tileY = static_cast<uint16_t>(displayPos.y / TILE_SIZE);

      if (player.id == localPlayerId) {
        gameState_->addCollectedCoinByLocalPlayer(tileX, tileY);
      } else {
        gameState_->addCollectedCoinByOtherPlayer(tileX, tileY);
      }
    }
  }

  for (uint16_t y = 0; y < mapHeight; ++y) {
    for (uint16_t x = 0; x < mapWidth; ++x) {
      size_t index = y * mapWidth + x;
      if (index >= mapData.size())
        continue;

      uint8_t tile = mapData[index];

      switch (tile) {
      case protocol::WALL: {
        wallShape_.setPosition(x * TILE_SIZE, y * TILE_SIZE);
        window->draw(wallShape_);
        break;
      }
      case protocol::COIN: {
        bool collectedByLocalPlayer =
            gameState_->isCoinCollectedByLocalPlayer(x, y);
        bool collectedByOtherPlayer =
            gameState_->isCoinCollectedByOtherPlayer(x, y);

        if (collectedByLocalPlayer && collectedByOtherPlayer) {
          break;
        }
        if (collectedByLocalPlayer) {
          coinShape_.setFillColor(sf::Color(150, 150, 150)); // Gris
        } else {
          coinShape_.setFillColor(sf::Color::Yellow);
        }
        coinShape_.setPosition(x * TILE_SIZE + TILE_SIZE / 2,
                               y * TILE_SIZE + TILE_SIZE / 2);
        window->draw(coinShape_);
        break;
      }
      case protocol::ELECTRIC: {
        electricShape_.setPosition(x * TILE_SIZE, y * TILE_SIZE);
        window->draw(electricShape_);
        break;
      }
      default:
        break;
      }
    }
  }
}

void Renderer::renderPlayers(sf::RenderWindow *window) {
  auto players = gameState_->getPlayerStates();
  uint8_t currentPlayerId = gameState_->getAssignedId();

  for (const auto &player : players) {
    if (!player.alive)
      continue;

    sf::Color playerColor =
        (player.id == currentPlayerId) ? sf::Color::Green : sf::Color::Red;

    playerShape_.setFillColor(playerColor);

    sf::Vector2f displayPos =
        convertServerToDisplayCoords(player.posX, player.posY);

    playerShape_.setPosition(displayPos.x, displayPos.y);
    window->draw(playerShape_);

    sf::Text scoreText;
    scoreText.setFont(*font_);
    scoreText.setString(std::to_string(player.score));
    scoreText.setCharacterSize(12);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(displayPos.x - 5, displayPos.y - 25);
    window->draw(scoreText);
  }
}

void Renderer::renderUI(sf::RenderWindow *window, const sf::Font &font) {
  sf::Text statusText;
  statusText.setFont(font);

  std::stringstream ss;
  if (gameState_->isGameRunning()) {
    ss << "Game running   Tick: " << gameState_->getCurrentTick();
    statusText.setFillColor(sf::Color::White);
  } else if (gameState_->hasGameEnded()) {
    uint8_t winnerId = gameState_->getWinnerId();
    if (winnerId == protocol::NO_WINNER) {
      ss << "Game ended: No winner";
    } else if (winnerId == gameState_->getAssignedId()) {
      ss << "Game ended: You win!";
      statusText.setFillColor(sf::Color::Green);
    } else {
      ss << "Game ended: Player " << static_cast<int>(winnerId) << " wins";
      statusText.setFillColor(sf::Color::Red);
    }
  } else {
    ss << "Connected, waiting for game start...";
    statusText.setFillColor(sf::Color::White);
  }
  statusText.setString(ss.str());
  statusText.setCharacterSize(16);
  statusText.setPosition(10, 10);
  window->draw(statusText);

  sf::Text controlsText;
  controlsText.setFont(font);
  controlsText.setString("Controls: Space = Jetpack");
  controlsText.setCharacterSize(14);
  controlsText.setFillColor(sf::Color(200, 200, 200));
  controlsText.setPosition(10, virtualHeight_ - 25);
  window->draw(controlsText);
}

void Renderer::renderDebugInfo(sf::RenderWindow *window, const sf::Font &font) {
  sf::Text debugText;
  debugText.setFont(font);

  std::stringstream ss;
  ss << "DEBUG MODE" << std::endl;
  ss << "Connection: " << (gameState_->isConnected() ? "YES" : "NO")
     << std::endl;
  ss << "Player ID: " << static_cast<int>(gameState_->getAssignedId())
     << std::endl;
  ss << "Game running: " << (gameState_->isGameRunning() ? "YES" : "NO")
     << std::endl;
  ss << "Jetpack: " << (gameState_->isJetpackActive() ? "ACTIVE" : "INACTIVE")
     << std::endl;

  auto dimensions = gameState_->getMapDimensions();
  ss << "Map: " << dimensions.first << "x" << dimensions.second << std::endl;

  auto players = gameState_->getPlayerStates();
  ss << "Players: " << players.size() << std::endl;
  for (const auto &player : players) {
    ss << "  ID " << static_cast<int>(player.id) << " (" << player.posX << ","
       << player.posY << ") "
       << "Score: " << player.score << (player.alive ? "" : " [DEAD]")
       << (player.collectedCoin ? " [COIN]" : "") << std::endl;
  }

  debugText.setString(ss.str());
  debugText.setCharacterSize(12);
  debugText.setFillColor(sf::Color::Red);
  debugText.setPosition(virtualWidth_ - 250, 10);
  window->draw(debugText);
}

void Renderer::renderConnectingMessage(sf::RenderWindow *window,
                                       const sf::Font &font) {
  sf::Text text;
  text.setFont(font);
  text.setString("Connecting to server...");
  text.setCharacterSize(24);
  text.setFillColor(sf::Color::White);

  text.setPosition(virtualWidth_ / 2.0f - text.getLocalBounds().width / 2.0f,
                   virtualHeight_ / 2.0f - text.getLocalBounds().height / 2.0f);
  window->draw(text);
}

void Renderer::handleResize(sf::RenderWindow *window, unsigned int width,
                            unsigned int height) {
  if (!window)
    return;

  float scaleX = static_cast<float>(width) / virtualWidth_;
  float scaleY = static_cast<float>(height) / virtualHeight_;

  float scale = std::min(scaleX, scaleY);
  float viewWidth = virtualWidth_ * scale;
  float viewHeight = virtualHeight_ * scale;

  float viewX = (width - viewWidth) / 2.0f;
  float viewY = (height - viewHeight) / 2.0f;

  gameView_.setViewport(sf::FloatRect(viewX / width, viewY / height,
                                      viewWidth / width, viewHeight / height));

  uiView_.setViewport(sf::FloatRect(viewX / width, viewY / height,
                                    viewWidth / width, viewHeight / height));
}

void Renderer::updateCamera() {
  auto players = gameState_->getPlayerStates();
  std::pair<uint16_t, uint16_t> mapDimensions = gameState_->getMapDimensions();
  uint16_t mapWidth = mapDimensions.first;
  uint16_t mapHeight = mapDimensions.second;

  float mapTotalWidth = mapWidth * TILE_SIZE;
  float mapTotalHeight = mapHeight * TILE_SIZE;

  for (const auto &player : players) {
    if (player.id == gameState_->getAssignedId() && player.alive) {
      sf::Vector2f displayPos =
          convertServerToDisplayCoords(player.posX, player.posY);

      float cameraOffsetX = 0.0f;
      if (displayPos.x > FIXED_PLAYER_X) {
        cameraOffsetX = displayPos.x - FIXED_PLAYER_X;

        float maxScrollX = mapTotalWidth - virtualWidth_;
        if (cameraOffsetX > maxScrollX) {
          cameraOffsetX = maxScrollX;
        }
      }

      float cameraOffsetY = 0.0f;

      cameraOffsetY = displayPos.y - virtualHeight_ / 2.0f;

      if (cameraOffsetY < 0) {
        cameraOffsetY = 0;
      } else if (cameraOffsetY > mapTotalHeight - virtualHeight_) {
        cameraOffsetY = mapTotalHeight - virtualHeight_;
      }

      gameView_.setCenter(virtualWidth_ / 2.0f + cameraOffsetX,
                          virtualHeight_ / 2.0f + cameraOffsetY);

      cameraOffsetX_ = cameraOffsetX;

      break;
    }
  }
}

sf::Vector2f Renderer::convertServerToDisplayCoords(uint16_t serverX,
                                                    uint16_t serverY) {
  std::pair<uint16_t, uint16_t> mapDimensions = gameState_->getMapDimensions();
  uint16_t mapWidth = mapDimensions.first;
  uint16_t mapHeight = mapDimensions.second;

  float mapTotalWidth = mapWidth * TILE_SIZE;
  float mapTotalHeight = mapHeight * TILE_SIZE;

  float displayX = (serverX / 1000.0f) * mapTotalWidth;

  float displayY = (serverY / 1000.0f) * mapTotalHeight;

  return sf::Vector2f(displayX, displayY);
}

void Renderer::renderGameEndScreen(sf::RenderWindow *window,
                                   const sf::Font &font) {
  sf::RectangleShape overlay;
  overlay.setSize(sf::Vector2f(virtualWidth_, virtualHeight_));
  overlay.setFillColor(sf::Color(0, 0, 0, 230)); // Black with transparency
  window->draw(overlay);

  auto currentTime = std::chrono::steady_clock::now();
  auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                            currentTime - gameEndTime_)
                            .count();
  int timeLeft = shutdownCountdownSeconds_ - static_cast<int>(elapsedSeconds);

  auto players = gameState_->getPlayerStates();
  uint8_t winnerId = gameState_->getWinnerId();
  uint8_t localPlayerId = gameState_->getAssignedId();

  sf::Text gameOverText;
  gameOverText.setFont(font);
  gameOverText.setCharacterSize(48);
  gameOverText.setFillColor(sf::Color::White);

  if (winnerId == protocol::NO_WINNER) {
    gameOverText.setString("GAME OVER - DRAW");
  } else if (winnerId == localPlayerId) {
    gameOverText.setString("YOU WIN!");
    gameOverText.setFillColor(sf::Color::Green);
  } else {
    gameOverText.setString("YOU LOSE");
    gameOverText.setFillColor(sf::Color::Red);
  }

  sf::FloatRect gameOverBounds = gameOverText.getLocalBounds();
  gameOverText.setPosition(virtualWidth_ / 2.0f - gameOverBounds.width / 2.0f,
                           virtualHeight_ / 2.0f - 100 -
                               gameOverBounds.height / 2.0f);

  window->draw(gameOverText);

  sf::Text scoresTitle;
  scoresTitle.setFont(font);
  scoresTitle.setString("PLAYER SCORES:");
  scoresTitle.setCharacterSize(24);
  scoresTitle.setFillColor(sf::Color::White);

  sf::FloatRect scoresTitleBounds = scoresTitle.getLocalBounds();
  scoresTitle.setPosition(virtualWidth_ / 2.0f - scoresTitleBounds.width / 2.0f,
                          gameOverText.getPosition().y + gameOverBounds.height +
                              40);

  window->draw(scoresTitle);

  float yOffset = scoresTitle.getPosition().y + scoresTitleBounds.height + 20;
  for (const auto &player : players) {
    sf::Text playerText;
    playerText.setFont(font);

    std::stringstream ss;
    ss << "Player " << static_cast<int>(player.id) << ": " << player.score
       << " points";
    if (player.id == localPlayerId) {
      ss << " (You)";
    }
    if (!player.alive) {
      ss << " [DEAD]";
    }

    playerText.setString(ss.str());
    playerText.setCharacterSize(20);

    if (player.id == localPlayerId) {
      playerText.setFillColor(sf::Color::Yellow);
    } else {
      playerText.setFillColor(sf::Color::White);
    }

    sf::FloatRect playerBounds = playerText.getLocalBounds();
    playerText.setPosition(virtualWidth_ / 2.0f - playerBounds.width / 2.0f,
                           yOffset);

    window->draw(playerText);
    yOffset += 30;
  }

  sf::Text countdownText;
  countdownText.setFont(font);
  countdownText.setString("Closing in " + std::to_string(timeLeft) +
                          " seconds...");
  countdownText.setCharacterSize(24);
  countdownText.setFillColor(sf::Color::White);

  sf::FloatRect countdownBounds = countdownText.getLocalBounds();
  countdownText.setPosition(virtualWidth_ / 2.0f - countdownBounds.width / 2.0f,
                            virtualHeight_ - 100);

  window->draw(countdownText);
}

void Renderer::setOnCountdownEndCallback(std::function<void()> callback) {
  onCountdownEndCallback_ = callback;
}

} // namespace graphics
} // namespace jetpack