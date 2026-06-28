#include "command/line_parser.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace kvcache::command {

LineParser::LineParser(const CommandTable& command_table) : command_table_(command_table) {}

ParseResult LineParser::ParseAndValidate(std::string_view input) const {
    auto tokens = Tokenize(input);
    if (tokens.empty()) {
        return {ParseStatus::kEmptyInput, {}, ""};
    }

    ParsedCommand command;
    command.name = ToLower(tokens.front());
    command.args.assign(tokens.begin() + 1, tokens.end());

    const CommandSpec* spec = command_table_.Find(command.name);
    if (spec == nullptr) {
        return {ParseStatus::kUnknownCommand, {}, "ERROR invalid command"};
    }

    const auto arg_count = static_cast<std::uint8_t>(command.args.size());
    if (arg_count < spec->min_args || arg_count > spec->max_args) {
        std::ostringstream out;
        out << "ERROR wrong number of arguments for " << command.name << " (expected "
            << static_cast<int>(spec->min_args) << "-" << static_cast<int>(spec->max_args)
            << ", got " << static_cast<int>(arg_count) << ")";
        return {ParseStatus::kWrongArity, {}, out.str()};
    }

    return {ParseStatus::kOk, std::move(command), ""};
}

std::vector<std::string> LineParser::Tokenize(std::string_view input) {
    std::istringstream stream{std::string(input)};
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string LineParser::ToLower(std::string_view input) {
    std::string result(input);
    std::transform(result.begin(), result.end(), result.begin(), [](const unsigned char value) {
        return static_cast<char>(std::tolower(value));
    });
    return result;
}

}  // namespace kvcache::command
