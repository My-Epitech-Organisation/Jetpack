// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Protocol definitions for Jetpack client
*/

#ifndef CLIENT_PROTOCOL_HPP_
#define CLIENT_PROTOCOL_HPP_

#include <cstdint>

namespace jetpack {
namespace protocol {

// Protocol constants
constexpr uint8_t MAGIC_BYTE = 0xAB;

// Packet types (as defined in RFC)
enum PacketType : uint8_t {
  CLIENT_CONNECT = 0x01,    // Client -> Server: Connect request
  SERVER_WELCOME = 0x02,    // Server -> Client: Connection accepted/rejected
  MAP_CHUNK = 0x03,         // Server -> Client: Map data fragment
  GAME_START = 0x04,        // Server -> Client: Game start signal
  CLIENT_INPUT = 0x05,      // Client -> Server: Input state
  GAME_STATE = 0x06,        // Server -> Client: Game state update
  GAME_END = 0x07,          // Server -> Client: Game over
  CLIENT_DISCONNECT = 0x08, // Both ways: Graceful disconnect
  DEBUG_INFO = 0x09         // Both ways: Debug text messages
};

// Game end reason codes
enum GameEndReason : uint8_t {
  MAP_COMPLETE = 0x01,
  PLAYER_DIED = 0x02,
  PLAYER_DISCONNECT = 0x03
};

// Jetpack state values
enum JetpackState : uint8_t {
  JETPACK_OFF = 0x00, // Falling
  JETPACK_ON = 0x01   // Ascending
};

// Header structure (4 bytes)
struct PacketHeader {
  uint8_t magic;   // Always MAGIC_BYTE (0xAB)
  uint8_t type;    // PacketType enum
  uint16_t length; // Total packet length including header (at least 4)
};

// Player data as received in GAME_STATE
struct PlayerState {
  uint8_t id;
  uint16_t posX;
  uint16_t posY;
  uint16_t score;
  uint8_t alive;
};

// Map element types
enum MapElement : uint8_t {
  EMPTY = 0x00,
  WALL = 0x01,
  COIN = 0x02,
  ELECTRIC = 0x03
};

// Special values
constexpr uint8_t NO_WINNER = 0xFF;

} // namespace protocol
} // namespace jetpack

#endif // CLIENT_PROTOCOL_HPP_
