#include "command/command_executor.hpp"

#include <charconv>
#include <limits>
#include <optional>
#include <string_view>

namespace kvcache::command {

namespace {

std::optional<long long> ParseStrictInteger(std::string_view input) {
    if (input.empty()) {
        return std::nullopt;
    }

    long long value = 0;
    const char* first = input.data();
    const char* last = input.data() + input.size();
    const auto [ptr, ec] = std::from_chars(first, last, value);
    if (ec != std::errc{} || ptr != last) {
        return std::nullopt;
    }
    return value;
}

}  // namespace

CommandExecutor::CommandExecutor()
    : handlers_{
          {"set", &CommandExecutor::HandleSet},
          {"get", &CommandExecutor::HandleGet},
          {"del", &CommandExecutor::HandleDel},
          {"exists", &CommandExecutor::HandleExists},
          {"incr", &CommandExecutor::HandleIncr},
          {"exit", &CommandExecutor::HandleExit},
          {"quit", &CommandExecutor::HandleQuit},
      } {}

CommandResponse CommandExecutor::Execute(const ParsedCommand& command) {
    const auto it = handlers_.find(command.name);
    if (it == handlers_.end()) {
        return {false, "ERROR unsupported command"};
    }
    const Handler handler = it->second;
    return (this->*handler)(command);
}

CommandResponse CommandExecutor::HandleSet(const ParsedCommand& command) {
    store_[command.args[0]] = command.args[1];
    return {true, "OK"};
}

CommandResponse CommandExecutor::HandleGet(const ParsedCommand& command) {
    const auto it = store_.find(command.args[0]);
    if (it == store_.end()) {
        return {true, "(nil)"};
    }
    return {true, it->second};
}

CommandResponse CommandExecutor::HandleDel(const ParsedCommand& command) {
    return {true, store_.erase(command.args[0]) == 1 ? "1" : "0"};
}

CommandResponse CommandExecutor::HandleExists(const ParsedCommand& command) {
    return {true, store_.contains(command.args[0]) ? "1" : "0"};
}

CommandResponse CommandExecutor::HandleIncr(const ParsedCommand& command) {
    const std::string& key = command.args[0];
    const auto key_it = store_.find(key);
    if (key_it == store_.end()) {
        return {false, "ERROR key does not exist"};
    }

    const auto current_value = ParseStrictInteger(key_it->second);
    if (!current_value.has_value()) {
        return {false, "ERROR value is not an integer"};
    }

    long long increment_value = 1;
    if (command.args.size() == 2) {
        const auto parsed_increment = ParseStrictInteger(command.args[1]);
        if (!parsed_increment.has_value()) {
            return {false, "ERROR increment is not an integer"};
        }
        increment_value = *parsed_increment;
    }

    bool overflow = false;
    if ((*current_value ^ increment_value) < 0) overflow = false;
    else if (*current_value > 0)
        overflow = increment_value > std::numeric_limits<long long>::max() - *current_value;
    else overflow = increment_value < std::numeric_limits<long long>::min() - *current_value;
    if (overflow) return {false, "ERROR increment would overflow"};

    const long long updated_value = *current_value + increment_value;
    key_it->second = std::to_string(updated_value);
    return {true, key_it->second};
}

CommandResponse CommandExecutor::HandleExit(const ParsedCommand&) {
    return {true, "Bye.", true};
}

CommandResponse CommandExecutor::HandleQuit(const ParsedCommand&) {
    return {true, "Bye.", true};
}

}  // namespace kvcache::command
