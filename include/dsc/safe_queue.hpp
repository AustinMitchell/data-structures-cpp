#pragma once

#include <optional>
#include <utility>
#include <mutex>
#include <condition_variable>

#include "dsc/ring_vector.hpp"

namespace dsc{

/** Thread-safe storage object which can store a single item through get(), which can be retrieved with put(). */
template<typename T>
class safe_queue {
    ring_vector<T>          queue_;
    std::mutex              mutex_;
    std::condition_variable get_cv_;

 public:
    /** Constructs an empty box. */
    safe_queue():   queue_(),
                    mutex_() {}

    /** Retrieves item from box. If there is no item, blocks until an item is placed inside. */
    auto get() -> T {
        auto lock = std::unique_lock{mutex_};

        while (true) {
            if (!queue_.empty()) {
                auto ret = T{std::move(queue_.pop_front_get())};
                return ret;
            }

            get_cv_.wait(lock);
        }
    }

    /** Retrieves item from box. If there is no item, blocks until an item is placed inside. */
    auto try_get() -> std::optional<T> {
        auto lock = std::unique_lock{mutex_};
        
        if (!queue_.empty()) {
            return std::move(queue_.pop_front_get());
        } else {
            return std::nullopt;
        } 
    }

    /** Places item within box. If there is already an item, blocks until the box is empty. */
    auto put(T new_data) -> void {
        auto lock = std::unique_lock{mutex_};
        queue_.push_back(std::move(new_data));
        get_cv_.notify_all();
    }
};

}