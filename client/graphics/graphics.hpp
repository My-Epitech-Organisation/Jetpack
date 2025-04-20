// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Graphics system for Jetpack client
*/

#ifndef CLIENT_GRAPHICS_GRAPHICS_HPP_
#define CLIENT_GRAPHICS_GRAPHICS_HPP_

#include "../gamestate.hpp"
#include "input_handler.hpp"
#include "renderer.hpp"
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
  
  // Function to set callback for when the game end countdown finishes
  void setOnCountdownEndCallback(std::function<void()> callback);

private:
  // Window and game state
  std::unique_ptr<sf::RenderWindow> window_;
  bool debugMode_;
  std::atomic<bool> running_;
  std::atomic<bool> graphicsInitialized_;
  std::thread graphicsThread_;

  // Font resource
  sf::Font font_;

  // Component classes
  std::unique_ptr<Renderer> renderer_;
  std::unique_ptr<InputHandler> inputHandler_;

  // Graphics methods
  bool initializeWindow();
  bool initializeResources();
  void processEvents();

  // Window resize handler
  void handleWindowResize(unsigned int width, unsigned int height);
};

} // namespace graphics
} // namespace jetpack

#endif // CLIENT_GRAPHICS_GRAPHICS_HPP_
