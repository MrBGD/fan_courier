#ifndef ADMINISTRATION_PANEL_SAFE_QUEUE_H
#define ADMINISTRATION_PANEL_SAFE_QUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>

template <typename T>
class Safe_Queue {
private:
    std::queue<T> queuelist;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> is_shutdown{false};

public:
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(mutex);
        queuelist.push(item);
        cv.notify_one();
    }

    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lock(mutex);
        if (queuelist.empty())
            return false;
        item = queuelist.front();
        queuelist.pop();
        return true;
    }

    bool wait_and_pop(T& item) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]() { return !queuelist.empty() || is_shutdown; });
        if (queuelist.empty() && is_shutdown)
            return false;
        item = queuelist.front();
        queuelist.pop();
        return true;
    }

    void shutdown() {
        is_shutdown = true;
        cv.notify_all();
    }
    size_t size() {
        std::lock_guard<std::mutex> lock(mutex);
        return queuelist.size();
    }
};

#endif // ADMINISTRATION_PANEL_SAFE_QUEUE_H