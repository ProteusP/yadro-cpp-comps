#include "utils.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <regex>
#include <sstream>

std::chrono::system_clock::time_point
TimeUtils::parseTime(const std::string &timeStr) {
  // Проверка по формату XX:XX
  static const std::regex timeRegex(R"(^\d{2}:\d{2}$)");
  if (!std::regex_match(timeStr, timeRegex)) {
    throw std::runtime_error("Invalid time format: " + timeStr);
  }

  // Парсим как целые числа
  int hour = std::stoi(timeStr.substr(0, 2));
  int minute = std::stoi(timeStr.substr(3, 2));

  if (hour < 0 || hour >= 24 || minute < 0 || minute >= 60) {
    throw std::runtime_error("Invalid time range: " + timeStr);
  }

  using namespace std::chrono;
  return system_clock::time_point{minutes{hour * 60 + minute}};
}

std::string TimeUtils::formatTime(std::chrono::system_clock::time_point t) {
  using namespace std::chrono;
  auto total_minutes = duration_cast<minutes>(t.time_since_epoch()).count();
  int hour = total_minutes / 60;
  int minute = total_minutes % 60;

  std::ostringstream ss;
  ss << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2)
     << std::setfill('0') << minute;
  return ss.str();
}

bool isValidClientName(const std::string &name) {
  static const std::regex nameRegex(R"(^[a-z0-9_-]+$)");
  return std::regex_match(name, nameRegex);
}