// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Input handler for Jetpack client
*/

#ifndef CLIENT_GRAPHICS_INPUT_HANDLER_HPP_
#define CLIENT_GRAPHICS_INPUT_HANDLER_HPP_

#include "../gamestate.hpp"
#include <SFML/Graphics.hpp>
#include <functional>

namespace jetpack {
namespace graphics {

class InputHandler {
public:
  InputHandler(GameState *gameState, bool debugMode);
  ~InputHandler() = default;

  // Handle input events from the window
  void processEvent(const sf::Event &event, sf::RenderWindow *window);

  // Set callback for window closing
  void setOnWindowClosedCallback(std::function<void()> callback);

private:
  GameState *gameState_;
  bool debugMode_;
  std::function<void()> onWindowClosedCallback_;

  // Helper to handle key presses
  void handleKeyPress(sf::Keyboard::Key key, bool isPressed);
};

} // namespace graphics
} // namespace jetpack

#endif // CLIENT_GRAPHICS_INPUT_HANDLER_HPP_