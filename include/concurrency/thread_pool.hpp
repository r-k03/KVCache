#pragma once

#include <atomic>
#include <cstddef>
#include <functional>

namespace kvcache::concurrency {

class ThreadPool {
public:
    explicit ThreadPool(std::size_t thread_count);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    void Start();
    void Stop();
    bool Submit(std::function<void()> task);

private:
    std::size_t thread_count_;
    std::atomic<bool> running_{false};
};

}  // namespace kvcache::concurrency
