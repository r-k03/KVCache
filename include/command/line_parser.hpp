#pragma once

#include "command/command_table.hpp"

#include <string_view>
#include <vector>

namespace kvcache::command {

class LineParser {
public:
    explicit LineParser(const CommandTable& command_table);

    ParseResult ParseAndValidate(std::string_view input) const;

private:
    static std::vector<std::string> Tokenize(std::string_view input);
    static std::string ToLower(std::string_view input);

    const CommandTable& command_table_;
};

}  // namespace kvcache::command
