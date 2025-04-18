// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Debug utilities implementation for Jetpack client
*/

#include "debug.hpp"
#include "../protocol.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sys/stat.h>

namespace jetpack {
namespace debug {

// Static file stream for logging
static std::ofstream logFile;
static std::mutex logMutex;
static bool loggingEnabled = false;
static std::string logFileName;

bool initLogging(bool debugMode) {
  if (!debugMode)
    return false;

  try {
    // Create debug directory if it doesn't exist
    std::filesystem::path debugDir = "debug";
    if (!std::filesystem::exists(debugDir)) {
      std::filesystem::create_directory(debugDir);
    }

    // Create log file with timestamp in name
    logFileName = "debug/client_" + getFileTimestamp() + ".log";
    logFile.open(logFileName, std::ios::out | std::ios::trunc);

    if (!logFile.is_open()) {
      std::cerr << "Failed to open log file: " << logFileName << std::endl;
      return false;
    }

    loggingEnabled = true;

    // Log initial message
    logFile << "=======================================" << std::endl;
    logFile << "Jetpack Client Debug Log" << std::endl;
    logFile << "Started at: " << getFileTimestamp() << std::endl;
    logFile << "=======================================" << std::endl;

    return true;
  } catch (const std::exception &e) {
    std::cerr << "Error initializing logging: " << e.what() << std::endl;
    return false;
  }
}

void shutdownLogging() {
  if (loggingEnabled && logFile.is_open()) {
    std::lock_guard<std::mutex> lock(logMutex);

    logFile << "=======================================" << std::endl;
    logFile << "Logging ended at: " << getFileTimestamp() << std::endl;
    logFile << "=======================================" << std::endl;

    logFile.close();
    std::cout << "Debug log written to: " << logFileName << std::endl;
  }
}

void print(const std::string &component, const std::string &message,
           bool debugMode) {
  if (debugMode) {
    // Get timestamp
    std::string timestamp = getTimestamp();

    // Print to console
    std::cout << "[" << timestamp << "][" << component << "] " << message
              << std::endl
              << std::flush;

    // Also log to file if logging is enabled
    if (loggingEnabled && logFile.is_open()) {
      std::lock_guard<std::mutex> lock(logMutex);
      logFile << "[" << timestamp << "][" << component << "] " << message
              << std::endl;
      logFile.flush(); // Ensure it's written immediately
    }
  }
}

void logToFile(const std::string &component, const std::string &message,
               bool debugMode) {
  if (debugMode && loggingEnabled && logFile.is_open()) {
    std::lock_guard<std::mutex> lock(logMutex);
    logFile << "[" << getTimestamp() << "][" << component << "] " << message
            << std::endl;
    logFile.flush();
  }
}

std::string formatHexDump(const std::vector<uint8_t> &data, size_t maxBytes) {
  if (data.empty()) {
    return "<empty>";
  }

  std::stringstream ss;
  size_t bytesToShow =
      (maxBytes == 0 || maxBytes > data.size()) ? data.size() : maxBytes;

  for (size_t i = 0; i < bytesToShow; ++i) {
    ss << std::hex << std::setfill('0') << std::setw(2)
       << static_cast<int>(data[i]);

    if ((i + 1) % 16 == 0 && i < bytesToShow - 1) {
      ss << "\n";
    } else if (i < bytesToShow - 1) {
      ss << " ";
    }
  }

  if (bytesToShow < data.size()) {
    ss << " ... (" << (data.size() - bytesToShow) << " more bytes)";
  }

  return ss.str();
}

std::string getTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) %
            1000;

  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&now_c);

  std::stringstream ss;
  ss << std::put_time(&tm, "%H:%M:%S") << "." << std::setfill('0')
     << std::setw(3) << ms.count();

  return ss.str();
}

std::string getFileTimestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&now_c);

  std::stringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
  return ss.str();
}

bool sendDebugPacket(int socket, const std::string &message, bool debugMode) {
  if (!debugMode || socket < 0 || message.empty()) {
    return false;
  }

  // Create packet header
  protocol::PacketHeader header;
  header.magic = protocol::MAGIC_BYTE;
  header.type = protocol::DEBUG_INFO;

  // Create payload (2 bytes length + message)
  std::vector<uint8_t> payload;
  uint16_t msgLen = static_cast<uint16_t>(message.length());
  uint16_t msgLenNet = htons(msgLen);

  // Add length bytes
  uint8_t *lenBytes = reinterpret_cast<uint8_t *>(&msgLenNet);
  payload.push_back(lenBytes[0]);
  payload.push_back(lenBytes[1]);

  // Add message bytes
  payload.insert(payload.end(), message.begin(), message.end());

  // Set packet length
  header.length = htons(sizeof(header) + payload.size());

  // Send header
  if (send(socket, &header, sizeof(header), 0) != sizeof(header)) {
    return false;
  }

  // Send payload
  if (send(socket, payload.data(), payload.size(), 0) !=
      static_cast<ssize_t>(payload.size())) {
    return false;
  }

  // Print locally too
  print("DEBUG_SENT", message, debugMode);
  return true;
}

} // namespace debug
} // namespace jetpack