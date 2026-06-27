#pragma once

#include "command/command_types.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace kvcache::command {

class CommandTable {
public:
    CommandTable();

    const CommandSpec* Find(std::string_view command_name) const;
    const std::unordered_map<std::string, CommandSpec>& Commands() const noexcept;

private:
    std::unordered_map<std::string, CommandSpec> commands_;
};

}  // namespace kvcache::command
