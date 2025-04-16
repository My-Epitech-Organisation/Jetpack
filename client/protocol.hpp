// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** B-NWP-400-NAN-4-1-jetpack-santiago.pidcova
** File description:
** Protocol definitions for Jetpack client
*/

#ifndef CLIENT_PROTOCOL_HPP_
#define CLIENT_PROTOCOL_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace jetpack {
namespace protocol {

// Protocol constants
constexpr uint8_t MAGIC_BYTE = 0xAB;

// Packet types
enum PacketType : uint8_t {
  CLIENT_CONNECT = 0x01,
  SERVER_WELCOME = 0x02,
  MAP_CHUNK = 0x03,
  GAME_START = 0x04,
  CLIENT_INPUT = 0x05,
  GAME_STATE = 0x06,
  GAME_END = 0x07,
  CLIENT_DISCONNECT = 0x08,
  DEBUG_INFO = 0x09
};

// Input mask bits
enum InputMask : uint8_t {
  MOVE_LEFT = 0x01,
  MOVE_RIGHT = 0x02,
  JETPACK = 0x04
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

} // namespace protocol
} // namespace jetpack

#endif // CLIENT_PROTOCOL_HPP_
