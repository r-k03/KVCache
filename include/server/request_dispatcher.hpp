#pragma once

#include "command/command_executor.hpp"
#include "command/line_parser.hpp"

#include <string>
#include <string_view>

namespace kvcache::server {

struct DispatchResult {
    std::string response_line;
    bool close_session{false};
};

class RequestDispatcher {
public:
    RequestDispatcher(const command::LineParser& parser, command::CommandExecutor& executor);

    DispatchResult HandleLine(std::string_view input) const;

private:
    const command::LineParser& parser_;
    command::CommandExecutor& executor_;
};

}  // namespace kvcache::server
