#pragma once

#include <optional>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>

namespace kvcache::storage {

class ThreadSafeKVStore {
public:
    ThreadSafeKVStore() = default;

    void Set(std::string key, std::string value);
    std::optional<std::string> Get(std::string_view key) const;
    bool Del(std::string_view key);
    bool Exists(std::string_view key) const;

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, std::string> store_;
};

}  // namespace kvcache::storage
