// Copyright 2025 paul-antoine.salmon@epitech.eu

#ifndef CLIENT_GRAPHICS_GRAPHICS_HPP_
#define CLIENT_GRAPHICS_GRAPHICS_HPP_

#include "../gamestate.hpp"
#include <SFML/Graphics.hpp>
#include <atomic>
#include <functional>
#include <memory>
#include <thread>

namespace jetpack {
namespace graphics {

class Graphics {
public:
  Graphics(GameState *gameState, bool debugMode);
  ~Graphics();

  void run();
  void stop();
  bool isRunning() const;

  // Function to set callback for window closing event
  void setOnWindowClosedCallback(std::function<void()> callback);

private:
  // Window and game state
  std::unique_ptr<sf::RenderWindow> window_;
  GameState *gameState_;
  bool debugMode_;
  std::atomic<bool> running_;
  std::atomic<bool> graphicsInitialized_;
  std::thread graphicsThread_;

  // Callback called when window is closed
  std::function<void()> onWindowClosedCallback_;

  // SFML objects
  sf::Font font_;
  sf::CircleShape playerShape_;  // Green circle
  sf::RectangleShape wallShape_; // Red square
  sf::CircleShape coinShape_;    // Yellow circle
  sf::RectangleShape
      electricShape_; // Yellow triangle (approximated with rectangle)

  // Game scale factors
  const float TILE_SIZE = 20.0f;
  const float PLAYER_RADIUS = 10.0f;
  const float COIN_RADIUS = 5.0f;
  const float FIXED_PLAYER_X_POS = 200.0f; // Fixed horizontal position for player

  // Graphics methods
  bool initializeWindow();
  bool initializeResources();
  void processEvents();
  void update();
  void render();

  // Rendering helpers
  void renderMap();
  void renderPlayers();
  void renderUI();
  void renderDebugInfo();

  // Input handling
  void handleKeyPress(sf::Keyboard::Key key, bool isPressed);
};

} // namespace graphics
} // namespace jetpack

#endif // CLIENT_GRAPHICS_GRAPHICS_HPP_
