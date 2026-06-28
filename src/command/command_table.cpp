#include "command/command_table.hpp"

#include <string>

namespace kvcache::command {

CommandTable::CommandTable()
    : commands_{
          {"set", {"Set key to value", 2, 2, false}},
          {"get", {"Get value for key", 1, 1, false}},
          {"del", {"Delete key", 1, 1, false}},
          {"exists", {"Check whether key exists", 1, 1, false}},
          {"incr", {"Increment integer value by amount", 1, 2, true}},
          {"exit", {"Exit the CLI", 0, 0, false}},
          {"quit", {"Exit the CLI", 0, 0, false}},
      } {}

const CommandSpec* CommandTable::Find(std::string_view command_name) const {
    const auto it = commands_.find(std::string(command_name));
    if (it == commands_.end()) {
        return nullptr;
    }
    return &it->second;
}

const std::unordered_map<std::string, CommandSpec>& CommandTable::Commands() const noexcept {
    return commands_;
}

}  // namespace kvcache::command
