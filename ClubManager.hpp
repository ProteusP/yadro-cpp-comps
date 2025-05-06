#ifndef CLUB_MANAGER_H
#define CLUB_MANAGER_H

#include "event.hpp"
#include <chrono>
#include <cstddef>
#include <map>
#include <optional>
#include <queue>
#include <string>

const int NO_TABLE = -1;

struct ClientInfo {
  bool inClub = false;
  int table = NO_TABLE;
  std::chrono::system_clock::time_point start;
};

struct TableStats {
  int revenue = 0;
  int occupiedMunites = 0;
  std::chrono::system_clock::time_point lastStart;
  bool busy = false;
  std::string client;
};

class ClubManager {
public:
  bool loadConfig(std::istream &in);
  bool processEvent(const std::string &line);
  void finishDay();
  void printReport() const;

private:
  size_t tableCount;
  size_t hourlyRate;
  std::chrono::system_clock::time_point openTime;
  std::chrono::system_clock::time_point closeTtime;
  std::map<std::string, ClientInfo> clients;
  std::map<size_t, TableStats> tables;
  std::queue<std::string> waiting;
  std::vector<Event> log;

  void logEvent(const std::string &time, EventType type,
                const std::string &name, std::optional<int> table = {},
                std::optional<std::string> err = {});
  std::chrono::system_clock::time_point parseTime(const std::string &str) const;
  std::string formatTime(std::chrono::system_clock::time_point t) const;
  void seatClient(const std::string &name, int table,
                  std::chrono::system_clock::time_point time);
};

#endif // CLUB_MANAGER_H