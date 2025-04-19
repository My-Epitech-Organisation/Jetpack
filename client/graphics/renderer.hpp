// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Renderer class for Jetpack client
*/

#ifndef CLIENT_GRAPHICS_RENDERER_HPP_
#define CLIENT_GRAPHICS_RENDERER_HPP_

#include "../gamestate.hpp"
#include <SFML/Graphics.hpp>

namespace jetpack {
namespace graphics {

class Renderer {
public:
  Renderer(GameState *gameState, bool debugMode);
  ~Renderer() = default;

  bool initialize(sf::Font &font);
  void render(sf::RenderWindow *window);
  void handleResize(sf::RenderWindow *window, unsigned int width,
                    unsigned int height);

private:
  // Core data
  GameState *gameState_;
  bool debugMode_;
  sf::Font *font_; // Pointer to the font used for rendering text

  // View management
  sf::View gameView_; // View for the game world
  sf::View uiView_;   // View for the UI elements
  const unsigned int virtualWidth_ = 800;
  const unsigned int virtualHeight_ = 600;

  // SFML shape objects
  sf::CircleShape playerShape_;      // Green circle
  sf::RectangleShape wallShape_;     // Gray square
  sf::CircleShape coinShape_;        // Yellow circle
  sf::RectangleShape electricShape_; // Purple rectangle

  // Game scale factors
  const float TILE_SIZE = 20.0f;
  const float PLAYER_RADIUS = 10.0f;
  const float COIN_RADIUS = 5.0f;

  // Rendering methods
  void renderMap(sf::RenderWindow *window);
  void renderPlayers(sf::RenderWindow *window);
  void renderUI(sf::RenderWindow *window, const sf::Font &font);
  void renderDebugInfo(sf::RenderWindow *window, const sf::Font &font);
  void renderConnectingMessage(sf::RenderWindow *window, const sf::Font &font);
};

} // namespace graphics
} // namespace jetpack

#endif // CLIENT_GRAPHICS_RENDERER_HPP_