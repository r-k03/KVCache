#include "concurrency/thread_pool.hpp"

#include <utility>

namespace kvcache::concurrency {

ThreadPool::ThreadPool(std::size_t thread_count) : thread_count_(thread_count) {}

ThreadPool::~ThreadPool() {
    Stop();
}

void ThreadPool::Start() {
    if (thread_count_ == 0 || running_.load()) {
        return;
    }
    running_.store(true);
    workers_.reserve(thread_count_);
    for (std::size_t i = 0; i < thread_count_; ++i) {
        workers_.emplace_back([this]() { WorkerLoop(); });
    }
}

void ThreadPool::Stop() {
    if (!running_.exchange(false)) {
        return;
    }
    cv_.notify_all();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers_.clear();

    std::queue<std::function<void()>> empty;
    {
        std::lock_guard lock(mutex_);
        tasks_.swap(empty);
    }
}

void ThreadPool::WorkerLoop() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock lock(mutex_);
            cv_.wait(lock, [this]() { return !running_.load() || !tasks_.empty(); });
            if (!running_.load() && tasks_.empty()) {
                return;
            }
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        try {
            task();
        } catch (...) {
            // Keep worker threads alive even if a task throws.
        }
    }
}

bool ThreadPool::Submit(std::function<void()> task) {
    if (!task || !running_.load()) {
        return false;
    }
    {
        std::lock_guard lock(mutex_);
        if (!running_.load()) {
            return false;
        }
        tasks_.push(std::move(task));
    }
    cv_.notify_one();
    return true;
}

}  // namespace kvcache::concurrency
