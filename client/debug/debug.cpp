// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Debug utilities implementation for Jetpack client
*/

#include "debug.hpp"
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
    std::filesystem::path debugDir = "debug";
    if (!std::filesystem::exists(debugDir)) {
      std::filesystem::create_directory(debugDir);
    }

    logFileName = "debug/client_" + getFileTimestamp() + ".log";
    logFile.open(logFileName, std::ios::out | std::ios::trunc);

    if (!logFile.is_open()) {
      std::cerr << "Failed to open log file: " << logFileName << std::endl;
      return false;
    }

    loggingEnabled = true;

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
    std::string timestamp = getTimestamp();

    std::cout << "[" << timestamp << "][" << component << "] " << message
              << std::endl
              << std::flush;

    if (loggingEnabled && logFile.is_open()) {
      std::lock_guard<std::mutex> lock(logMutex);
      logFile << "[" << timestamp << "][" << component << "] " << message
              << std::endl;
      logFile.flush();
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

} // namespace debug
} // namespace jetpack