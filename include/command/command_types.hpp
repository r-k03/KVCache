#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace kvcache::command {

struct ParsedCommand {
    std::string name;
    std::vector<std::string> args;
};

using KeyValueStore = std::unordered_map<std::string, std::string>;

struct CommandResponse {
    bool success{true};
    std::string message;
    bool should_exit{false};
};

struct CommandSpec {
    std::string_view description;
    std::uint8_t min_args;
    std::uint8_t max_args;
    bool has_optional_args{false};
};

enum class ParseStatus {
    kOk,
    kEmptyInput,
    kUnknownCommand,
    kWrongArity,
};

struct ParseResult {
    ParseStatus status{ParseStatus::kEmptyInput};
    ParsedCommand command{};
    std::string message;

    bool ok() const noexcept { return status == ParseStatus::kOk; }
};

}  // namespace kvcache::command
