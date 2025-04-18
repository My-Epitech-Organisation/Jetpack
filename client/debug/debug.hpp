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
 * @brief Initialize the logging system
 *
 * Creates a log file in the debug directory with the format:
 * client_[date]_[time].log
 *
 * @param debugMode Whether debug mode is enabled
 * @return true if initialization was successful
 */
bool initLogging(bool debugMode);

/**
 * @brief Clean up logging resources
 */
void shutdownLogging();

/**
 * @brief Print debug message to console and log file if debug mode is enabled
 *
 * @param component Name of the component (e.g., "Network", "Graphics")
 * @param message Debug message to print
 * @param debugMode Whether debug mode is enabled
 */
void print(const std::string &component, const std::string &message,
           bool debugMode);

/**
 * @brief Log debug message to file only (no console output)
 *
 * @param component Name of the component (e.g., "Network", "Graphics")
 * @param message Debug message to log
 * @param debugMode Whether debug mode is enabled
 */
void logToFile(const std::string &component, const std::string &message,
               bool debugMode);

/**
 * @brief Format binary data as hex dump for debugging
 *
 * @param data Binary data to format
 * @param maxBytes Max bytes to show (0 for all)
 * @return Formatted hex string
 */
std::string formatHexDump(const std::vector<uint8_t> &data,
                          size_t maxBytes = 0);

/**
 * @brief Get current timestamp for debug messages
 *
 * @return Formatted timestamp string
 */
std::string getTimestamp();

/**
 * @brief Get current date and time formatted for filenames
 *
 * @return Formatted date and time string for filenames (YYYY-MM-DD_HH-MM-SS)
 */
std::string getFileTimestamp();

/**
 * @brief Send a DEBUG_INFO packet to server
 *
 * @param socket Socket to send on
 * @param message Debug message to send
 * @param debugMode Whether debug mode is enabled
 * @return true if sent successfully
 */
bool sendDebugPacket(int socket, const std::string &message, bool debugMode);

} // namespace debug
} // namespace jetpack

#endif // CLIENT_DEBUG_DEBUG_HPP_