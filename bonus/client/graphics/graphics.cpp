// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Graphics implementation for Jetpack client
*/

#include "graphics.hpp"
#include "../debug/debug.hpp"
#include <iostream>
#include <memory>
#include <thread>

namespace jetpack {
namespace graphics {

Graphics::Graphics(GameState *gameState, bool debugMode)
    : window_(nullptr), debugMode_(debugMode), running_(false),
      graphicsInitialized_(false) {

  // Create component classes
  renderer_ = std::make_unique<Renderer>(gameState, debugMode);
  inputHandler_ = std::make_unique<InputHandler>(gameState, debugMode);

  // Set up the resize callback
  inputHandler_->setOnWindowResizeCallback(
      [this](unsigned int width, unsigned int height) {
        this->handleWindowResize(width, height);
      });
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

  // Initialize the renderer with the window and font
  if (!renderer_->initialize(font_)) {
    std::cerr << "Failed to initialize renderer" << std::endl;
    return false;
  }

  return true;
}

void Graphics::run() {
  running_ = true;
  graphicsThread_ = std::thread([this]() {
    graphicsInitialized_ = initializeWindow();

    if (graphicsInitialized_) {
      initializeResources();

      while (running_ && window_ && window_->isOpen()) {
        processEvents();
        renderer_->render(window_.get());
      }

      if (!window_->isOpen()) {
        debug::print("Graphics",
                     "Window was closed, calling window closed callback",
                     debugMode_);
      }
    } else {
      std::cerr << "Graphics initialization failed - running in headless mode"
                << std::endl;
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
  inputHandler_->setOnWindowClosedCallback(callback);
}

void Graphics::setOnCountdownEndCallback(std::function<void()> callback) {
  renderer_->setOnCountdownEndCallback(callback);
}

void Graphics::processEvents() {
  if (!window_)
    return;

  sf::Event event;
  while (window_->pollEvent(event)) {
    inputHandler_->processEvent(event, window_.get());
  }
}

void Graphics::handleWindowResize(unsigned int width, unsigned int height) {
  if (!window_ || !renderer_)
    return;

  renderer_->handleResize(window_.get(), width, height);
}

} // namespace graphics
} // namespace jetpack
