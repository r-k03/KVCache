#include "storage/thread_safe_kv_store.hpp"

#include <charconv>
#include <limits>
#include <mutex>
#include <optional>
#include <string_view>

namespace kvcache::storage {

namespace {

std::optional<long long> ParseStrictInteger(std::string_view input) {
    if (input.empty()) {
        return std::nullopt;
    }

    long long value = 0;
    const char* first = input.data();
    const char* last = input.data() + input.size();
    const auto [ptr, ec] = std::from_chars(first, last, value);
    if (ec != std::errc{} || ptr != last) {
        return std::nullopt;
    }
    return value;
}

bool WouldOverflow(long long lhs, long long rhs) {
    if ((lhs ^ rhs) < 0) {
        return false;
    }
    if (lhs > 0) {
        return rhs > std::numeric_limits<long long>::max() - lhs;
    }
    return rhs < std::numeric_limits<long long>::min() - lhs;
}

}  // namespace

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

IncrementResult ThreadSafeKVStore::Increment(std::string_view key, long long amount) {
    std::unique_lock lock(mutex_);
    const auto it = store_.find(std::string(key));
    if (it == store_.end()) {
        return {IncrementStatus::kNotFound, ""};
    }

    const auto current_value = ParseStrictInteger(it->second);
    if (!current_value.has_value()) {
        return {IncrementStatus::kNotInteger, ""};
    }

    if (WouldOverflow(*current_value, amount)) {
        return {IncrementStatus::kOverflow, ""};
    }

    const long long next_value = *current_value + amount;
    it->second = std::to_string(next_value);
    return {IncrementStatus::kOk, it->second};
}

}  // namespace kvcache::storage
