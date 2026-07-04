#include "concurrency/thread_pool.hpp"

namespace kvcache::concurrency {

ThreadPool::ThreadPool(std::size_t thread_count) : thread_count_(thread_count) {}

ThreadPool::~ThreadPool() {
    Stop();
}

void ThreadPool::Start() {
    if (thread_count_ == 0) {
        return;
    }
    running_.store(true);
}

void ThreadPool::Stop() {
    running_.store(false);
}

bool ThreadPool::Submit(std::function<void()> task) {
    if (!running_.load() || !task) {
        return false;
    }
    task();
    return true;
}

}  // namespace kvcache::concurrency
