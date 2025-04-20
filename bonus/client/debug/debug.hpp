// Copyright 2025 paul-antoine.salmon@epitech.eu
/*
** EPITECH PROJECT, 2025
** Jetpack
** File description:
** Debug utilities for Jetpack client
*/

#ifndef CLIENT_DEBUG_DEBUG_HPP_
#define CLIENT_DEBUG_DEBUG_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace jetpack {
namespace debug {

/**
 * Initialize the logging system
 * Creates a log file in the debug directory with the format:
 * client_[date]_[time].log
 */
bool initLogging(bool debugMode);

/**
 * Clean up logging resources
 */
void shutdownLogging();

/**
 * Print debug message to console and log file if debug mode is enabled
 */
void print(const std::string &component, const std::string &message,
           bool debugMode);

/**
 * Log debug message to file only (no console output)
 */
void logToFile(const std::string &component, const std::string &message,
               bool debugMode);

/**
 * Format binary data as hex dump for debugging
 */
std::string formatHexDump(const std::vector<uint8_t> &data,
                          size_t maxBytes = 0);

/**
 * Get current timestamp for debug messages
 */
std::string getTimestamp();

/**
 * Get current date and time formatted for filenames
 */
std::string getFileTimestamp();

} // namespace debug
} // namespace jetpack

#endif // CLIENT_DEBUG_DEBUG_HPP_