#include "server/request_dispatcher.hpp"

namespace kvcache::server {

RequestDispatcher::RequestDispatcher(
    const command::LineParser& parser,
    command::CommandExecutor& executor
)
    : parser_(parser), executor_(executor) {}

DispatchResult RequestDispatcher::HandleLine(std::string_view input) const {
    const auto parse_result = parser_.ParseAndValidate(input);
    if (parse_result.status == command::ParseStatus::kEmptyInput) {
        return {"", false};
    }

    if (!parse_result.ok()) {
        return {parse_result.message, false};
    }

    const auto command_response = executor_.Execute(parse_result.command);
    return {command_response.message, command_response.should_exit};
}

}  // namespace kvcache::server
