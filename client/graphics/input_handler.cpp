// filepath: /home/psalmon/Documents/Sem4/Jetpack/Jetpack/client/graphics/input_handler.cpp
// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Input handler implementation for Jetpack client
*/

#include "input_handler.hpp"
#include "../debug/debug.hpp"

namespace jetpack {
namespace graphics {

InputHandler::InputHandler(GameState *gameState, bool debugMode)
    : gameState_(gameState), debugMode_(debugMode), onWindowClosedCallback_(nullptr) {}

void InputHandler::setOnWindowClosedCallback(std::function<void()> callback) {
  onWindowClosedCallback_ = callback;
}

void InputHandler::processEvent(const sf::Event &event, sf::RenderWindow *window) {
  if (!window)
    return;

  switch (event.type) {
    case sf::Event::Closed:
      window->close();
      if (onWindowClosedCallback_) {
        debug::print("InputHandler", "Window closed, triggering callback", debugMode_);
        onWindowClosedCallback_();
      }
      break;
      
    case sf::Event::KeyPressed:
      handleKeyPress(event.key.code, true);
      break;
      
    case sf::Event::KeyReleased:
      handleKeyPress(event.key.code, false);
      break;
      
    default:
      break;
  }
}

void InputHandler::handleKeyPress(sf::Keyboard::Key key, bool isPressed) {
  bool jetpackCurrentlyActive = gameState_->isJetpackActive();
  bool newJetpackState = jetpackCurrentlyActive;

  switch (key) {
    case sf::Keyboard::Space:
      newJetpackState = isPressed;
      break;

    case sf::Keyboard::Escape:
      // Escape is handled by window closing in processEvent
      break;

    default:
      return;
  }

  if (newJetpackState != jetpackCurrentlyActive) {
    gameState_->setJetpackActive(newJetpackState);
    if (debugMode_) {
      debug::logToFile("InputHandler", 
                      "Jetpack state changed to: " + std::string(newJetpackState ? "ON" : "OFF"), 
                      debugMode_);
    }
  }
}

} // namespace graphics
} // namespace jetpack