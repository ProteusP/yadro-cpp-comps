#ifndef UTILS_HPP
#define UTILS_HPP

#include <chrono>
#include <string>

namespace TimeUtils {
std::chrono::system_clock::time_point parseTime(const std::string &timeStr);
std::string formatTime(std::chrono::system_clock::time_point t);
} // namespace TimeUtils

bool isValidClientName(const std::string &name);
#endif