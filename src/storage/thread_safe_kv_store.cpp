#include "storage/thread_safe_kv_store.hpp"

#include <mutex>

namespace kvcache::storage {

void ThreadSafeKVStore::Set(std::string key, std::string value) {
    std::unique_lock lock(mutex_);
    store_[std::move(key)] = std::move(value);
}

std::optional<std::string> ThreadSafeKVStore::Get(std::string_view key) const {
    std::shared_lock lock(mutex_);
    const auto it = store_.find(std::string(key));
    if (it == store_.end()) {
        return std::nullopt;
    }
    return it->second;
}

bool ThreadSafeKVStore::Del(std::string_view key) {
    std::unique_lock lock(mutex_);
    return store_.erase(std::string(key)) == 1;
}

bool ThreadSafeKVStore::Exists(std::string_view key) const {
    std::shared_lock lock(mutex_);
    return store_.contains(std::string(key));
}

}  // namespace kvcache::storage
