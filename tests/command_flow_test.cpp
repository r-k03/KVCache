#include "command/command_executor.hpp"
#include "command/command_table.hpp"
#include "command/line_parser.hpp"
#include "server/request_dispatcher.hpp"
#include "storage/thread_safe_kv_store.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

using kvcache::command::CommandExecutor;
using kvcache::command::CommandTable;
using kvcache::command::LineParser;
using kvcache::server::RequestDispatcher;
using kvcache::storage::ThreadSafeKVStore;

void Check(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << "\n";
        std::exit(1);
    }
}

void CheckResponse(
    const RequestDispatcher& dispatcher,
    const std::string& line,
    const std::string& expected_response,
    bool expected_close_session = false
) {
    const auto result = dispatcher.HandleLine(line);
    Check(
        result.response_line == expected_response,
        "Expected response '" + expected_response + "' for '" + line + "', got '" +
            result.response_line + "'"
    );
    Check(
        result.close_session == expected_close_session,
        "Unexpected close_session for line '" + line + "'"
    );
}

}  // namespace

int main() {
    const CommandTable table;
    ThreadSafeKVStore store;
    CommandExecutor executor(store);
    const LineParser parser(table);
    const RequestDispatcher dispatcher(parser, executor);

    // Basic CRUD + increment flow.
    CheckResponse(dispatcher, "SET name Alice", "OK");
    CheckResponse(dispatcher, "GET name", "Alice");
    CheckResponse(dispatcher, "EXISTS name", "1");
    CheckResponse(dispatcher, "DEL name", "1");
    CheckResponse(dispatcher, "GET name", "(nil)");

    CheckResponse(dispatcher, "SET counter 10", "OK");
    CheckResponse(dispatcher, "INCR counter", "11");
    CheckResponse(dispatcher, "INCR counter 9", "20");
    CheckResponse(dispatcher, "INCR counter nope", "ERROR increment is not an integer");

    // Parser and session-close behavior.
    CheckResponse(dispatcher, "", "");
    CheckResponse(dispatcher, "BOGUS", "ERROR invalid command");
    CheckResponse(dispatcher, "QUIT", "Bye.", true);

    std::cout << "PASS: command_flow_test\n";
    return 0;
}
