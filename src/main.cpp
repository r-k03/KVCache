#include "command/command_table.hpp"
#include "command/command_executor.hpp"
#include "command/line_parser.hpp"
#include "storage/thread_safe_kv_store.hpp"

#include <iostream>
#include <string>

int main() {
    const kvcache::command::CommandTable command_table;
    kvcache::storage::ThreadSafeKVStore store;
    kvcache::command::CommandExecutor command_executor(store);
    const kvcache::command::LineParser parser(command_table);
    bool exited_by_command = false;

    std::cout << "KVCache CLI (type EXIT to quit)\n";

    std::string line;
    while (true) {
        std::cout << "kv> ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        const auto result = parser.ParseAndValidate(line);
        if (result.status == kvcache::command::ParseStatus::kEmptyInput) {
            continue;
        }

        if (!result.ok()) {
            std::cout << result.message << "\n";
            continue;
        }

        const auto response = command_executor.Execute(result.command);
        std::cout << response.message << "\n";
        if (response.should_exit) {
            exited_by_command = true;
            break;
        }
    }

    if (!exited_by_command) {
        std::cout << "Bye.\n";
    }
    return 0;
}
