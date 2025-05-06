#include "ClubManager.hpp"
#include "event.hpp"
#include "utils.hpp"
#include <algorithm>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

bool ClubManager::loadConfig(std::istream &in) {
  std::string line;

  if (!std::getline(in, line)) {
    return false;
  }
  try { // Если первая строка - не ОДНО число, то выводим её и завершаемся
    size_t pos;
    tableCount = std::stoi(line, &pos);
    if (pos != line.size()) {
      std::cout << line << std::endl;
      return false;
    }
  } catch (...) {
    std::cout << line << std::endl;
    return false;
  }

  if (!std::getline(in, line)) {
    return false;
  }

  std::istringstream timeStream(line);
  std::string openStr;
  std::string closeStr;
  timeStream >> openStr >> closeStr;

  // Парсинг времени открытия
  try {
    openTime = parseTime(openStr);
  } catch (...) {
    std::cout << line << std::endl;
    return false;
  }

  // Парсинг времени закрытия
  try {
    closeTtime = parseTime(closeStr);
  } catch (...) {
    std::cout << line << std::endl;
    return false;
  }

  if (!std::getline(in, line))
    return false;

  try { // Если третья строка - не ОДНО число, то выводим её и завершаемся
    size_t pos;
    hourlyRate = std::stoi(line, &pos);
    if (pos != line.size()) {
      std::cout << line << std::endl;
      return false;
    }
  } catch (...) {
    std::cout << line << std::endl;
    return false;
  }

  for (int i = 1; i <= tableCount; ++i) {
    tables[i] = TableStats();
  }

  return true;
}

std::chrono::system_clock::time_point
ClubManager::parseTime(const std::string &str) const {
  return TimeUtils::parseTime(str);
}

bool ClubManager::processEvent(const std::string &line) {
  std::istringstream iss(line);
  std::string timeStr;
  int id;
  iss >> timeStr >> id;
  std::chrono::system_clock::time_point now;
  // Обработка ошибки формата времени события
  try {
    now = parseTime(timeStr);
  } catch (...) {
    return false;
  }

  std::string name;
  int table;
  iss >> name;

  std::string leftOver;
  // Проверка формата имени
  if (!isValidClientName(name)) {
    return false;
  }
  switch (id) {

  case 1: // Клиент пришел
    // Проверка на отсутствие лишних параметров в строке
    iss >> leftOver;
    if (leftOver != "") {
      return false;
    }
    if (clients[name].inClub) {
      logEvent(timeStr, EventType::ClientCame, name);
      logEvent(timeStr, EventType::Error, "", {}, "YouShallNotPass");
      return true;
    }
    if (now < openTime) {
      logEvent(timeStr, EventType::ClientCame, name);
      logEvent(timeStr, EventType::Error, "", {}, "NotOpenYet");
      return true;
    }
    clients[name].inClub = true;
    logEvent(timeStr, EventType::ClientCame, name);
    break;

  case 2: // Клиент сел за стол
    iss >> table;
    // Проверка формата номера стола
    if (table < 1 || table > tableCount) {
      return false;
    }

    // Проверка на отсутствие лишних параметров в строке
    iss >> leftOver;
    if (leftOver != "") {
      return false;
    }

    if (!clients[name].inClub) {
      logEvent(timeStr, EventType::ClientSat, name, table);
      logEvent(timeStr, EventType::Error, "", {}, "ClientUnknown");
      return true;
    }
    if (tables[table].busy) {
      logEvent(timeStr, EventType::ClientSat, name, table);
      logEvent(timeStr, EventType::Error, "", {}, "PlaceIsBusy");
      return true;
    }
    // Выше уже учли, что он попытался сесть за свой же стол
    if (clients[name].table != NO_TABLE) {
      auto &prev = tables[clients[name].table];
      auto minutes =
          std::chrono::duration_cast<std::chrono::minutes>(now - prev.lastStart)
              .count();
      // Округляем вверх до часа
      prev.occupiedMunites += ((minutes + 59) / 60) * hourlyRate;
      prev.busy = false;
      prev.client.clear();
    }
    seatClient(name, table, now);
    logEvent(timeStr, EventType::ClientSat, name, table);
    break;

  case 3: // Клиент ожидает
    // Проверка на отсутствие лишних параметров в строке
    iss >> leftOver;
    if (leftOver != "") {
      return false;
    }
    for (const auto &[i, t] : tables) {
      if (!t.busy) {
        logEvent(timeStr, EventType::ClientWaiting, name);
        logEvent(timeStr, EventType::Error, "", {}, "ICanWaitNoLonger!");
        return true;
      }
    }
    if (waiting.size() > tableCount) {
      logEvent(timeStr, EventType::ClientWaiting, name);
      logEvent(timeStr, EventType::AutoLeft, name);
      clients.erase(name);
    } else {
      waiting.push(name);
      logEvent(timeStr, EventType::ClientWaiting, name);
    }
    break;

  case 4: // Клиент ушел
    // Проверка на отсутствие лишних параметров в строке
    iss >> leftOver;
    if (leftOver != "") {
      return false;
    }

    if (!clients[name].inClub) {
      logEvent(timeStr, EventType::ClientLeft, name);
      logEvent(timeStr, EventType::Error, "", {}, "ClientUnknown");
      return true;
    }

    if (clients[name].table != NO_TABLE) {
      int prevTable = clients[name].table;
      auto &t = tables[prevTable];
      auto minutes =
          std::chrono::duration_cast<std::chrono::minutes>(now - t.lastStart)
              .count();
      t.occupiedMunites += minutes;
      t.revenue += ((minutes + 59) / 60) * hourlyRate;
      t.busy = false;
      t.client.clear();

      clients.erase(name);
      logEvent(timeStr, EventType::ClientLeft, name);

      if (!waiting.empty()) {
        std::string next = waiting.front();
        waiting.pop();
        seatClient(next, prevTable, now);
        logEvent(timeStr, EventType::AutoSat, next, prevTable);
      }
    } else {
      clients.erase(name);
      logEvent(timeStr, EventType::ClientLeft, name);
    }
    break;

  default:
    return false;
  }
  return true;
}

void ClubManager::logEvent(const std::string &time, EventType type,
                           const std::string &name, std::optional<int> table,
                           std::optional<std::string> err) {
  log.push_back(Event{time, type, name, table, err});
}

void ClubManager::finishDay() {
  for (auto &[name, info] : clients) {
    if (info.table != NO_TABLE) {
      auto &t = tables[info.table];
      auto minutes = std::chrono::duration_cast<std::chrono::minutes>(
                         closeTtime - t.lastStart)
                         .count();
      t.occupiedMunites += minutes;
      t.revenue += ((minutes + 59) / 60) * hourlyRate;
      t.busy = false;
      t.client.clear();
    }
  }

  std::vector<std::string> remaining;
  for (const auto &[name, _] : clients) {
    remaining.push_back(name);
  }

  std::sort(remaining.begin(), remaining.end());

  for (const auto &name : remaining) {
    logEvent(TimeUtils::formatTime(closeTtime), EventType::AutoLeft, name);
  }
}

void ClubManager::printReport() const {
  std::cout << TimeUtils::formatTime(openTime) << "\n";

  for (const auto &e : log) {
    std::cout << e.toString() << "\n";
  }

  std::cout << TimeUtils::formatTime(closeTtime) << "\n";

  for (const auto &[id, t] : tables) {
    int h = t.occupiedMunites / 60;
    int m = t.occupiedMunites % 60;
    std::cout << id << " " << t.revenue << " " << std::setfill('0')
              << std::setw(2) << h << ":" << std::setw(2) << m << "\n";
  }
}

void ClubManager::seatClient(const std::string &name, int table,
                             std::chrono::system_clock::time_point time) {
  clients[name].table = table;
  tables[table].client = name;
  tables[table].lastStart = time;
  tables[table].busy = true;
}
