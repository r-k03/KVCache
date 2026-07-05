#pragma once

#include "command/command_types.hpp"
#include "storage/thread_safe_kv_store.hpp"

#include <string>
#include <unordered_map>

namespace kvcache::command {

class CommandExecutor {
public:
    explicit CommandExecutor(storage::ThreadSafeKVStore& store);

    CommandResponse Execute(const ParsedCommand& command);

private:
    using Handler = CommandResponse (CommandExecutor::*)(const ParsedCommand&);

    CommandResponse HandleSet(const ParsedCommand& command);
    CommandResponse HandleGet(const ParsedCommand& command);
    CommandResponse HandleDel(const ParsedCommand& command);
    CommandResponse HandleExists(const ParsedCommand& command);
    CommandResponse HandleIncr(const ParsedCommand& command);
    CommandResponse HandleExit(const ParsedCommand& command);
    CommandResponse HandleQuit(const ParsedCommand& command);

    std::unordered_map<std::string, Handler> handlers_;
    storage::ThreadSafeKVStore& store_;
};

}  // namespace kvcache::command
