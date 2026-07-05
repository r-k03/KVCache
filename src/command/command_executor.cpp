#include "command/command_executor.hpp"

#include <charconv>
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

CommandExecutor::CommandExecutor(storage::ThreadSafeKVStore& store)
    : handlers_{
          {"set", &CommandExecutor::HandleSet},
          {"get", &CommandExecutor::HandleGet},
          {"del", &CommandExecutor::HandleDel},
          {"exists", &CommandExecutor::HandleExists},
          {"incr", &CommandExecutor::HandleIncr},
          {"exit", &CommandExecutor::HandleExit},
          {"quit", &CommandExecutor::HandleQuit},
      },
      store_(store) {}

CommandResponse CommandExecutor::Execute(const ParsedCommand& command) {
    const auto it = handlers_.find(command.name);
    if (it == handlers_.end()) {
        return {false, "ERROR unsupported command"};
    }
    const Handler handler = it->second;
    return (this->*handler)(command);
}

CommandResponse CommandExecutor::HandleSet(const ParsedCommand& command) {
    store_.Set(command.args[0], command.args[1]);
    return {true, "OK"};
}

CommandResponse CommandExecutor::HandleGet(const ParsedCommand& command) {
    const auto value = store_.Get(command.args[0]);
    if (!value.has_value()) {
        return {true, "(nil)"};
    }
    return {true, *value};
}

CommandResponse CommandExecutor::HandleDel(const ParsedCommand& command) {
    return {true, store_.Del(command.args[0]) ? "1" : "0"};
}

CommandResponse CommandExecutor::HandleExists(const ParsedCommand& command) {
    return {true, store_.Exists(command.args[0]) ? "1" : "0"};
}

CommandResponse CommandExecutor::HandleIncr(const ParsedCommand& command) {
    const std::string& key = command.args[0];

    long long increment_value = 1;
    if (command.args.size() == 2) {
        const auto parsed_increment = ParseStrictInteger(command.args[1]);
        if (!parsed_increment.has_value()) {
            return {false, "ERROR increment is not an integer"};
        }
        increment_value = *parsed_increment;
    }

    const auto result = store_.Increment(key, increment_value);
    if (result.status == storage::IncrementStatus::kNotFound) {
        return {false, "ERROR key does not exist"};
    }
    if (result.status == storage::IncrementStatus::kNotInteger) {
        return {false, "ERROR value is not an integer"};
    }
    if (result.status == storage::IncrementStatus::kOverflow) {
        return {false, "ERROR increment would overflow"};
    }
    return {true, result.value};
}

CommandResponse CommandExecutor::HandleExit(const ParsedCommand&) {
    return {true, "Bye.", true};
}

CommandResponse CommandExecutor::HandleQuit(const ParsedCommand&) {
    return {true, "Bye.", true};
}

}  // namespace kvcache::command
