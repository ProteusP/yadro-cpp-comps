#ifndef EVENT_HPP
#define EVENT_HPP

#include <optional>
#include <string>

enum class EventType {
  ClientCame = 1,
  ClientSat = 2,
  ClientWaiting = 3,
  ClientLeft = 4,
  AutoLeft = 11,
  AutoSat = 12,
  Error = 13
};

struct Event {
  std::string timeStr;
  EventType type;
  std::string clientName;
  std::optional<int> table;
  std::optional<std::string> errorMsg;

  std::string toString() const {
    std::string line = timeStr + " " + std::to_string(static_cast<int>(type));

    if (!clientName.empty()) {
      line += " " + clientName;
    }
     if (table.has_value()){
        line += " " + std::to_string(table.value());
     }
     if (errorMsg.has_value()){
      line += " " + errorMsg.value();
     }
    return line;
  }
};
#endif // EVENT_HPP