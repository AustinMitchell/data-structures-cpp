// Copyright 2020 Nathaniel Mitchell

#pragma once

#include <utility>
#include <mutex>
#include <condition_variable>

namespace dsc {

template<typename T>
class box {
    T                       data_;
    bool                    has_data_;
    std::mutex              mutex_;
    std::condition_variable cv_;

 public:
    box(): data_(), mutex_() {}
    explicit box(T init_data): data_(std::move(init_data)), mutex_() {}

    auto get() -> T {
        auto lock = std::unique_lock{mutex_, std::defer_lock};
        while (true) {
            if (has_data_ && lock.try_lock()) {
                has_data_ = false;
                auto ret = T{std::move(data_)};
                cv_.notify_all();
                return ret;
            }

            cv_.wait(lock);
        }
    }

    auto put(T new_data) -> void {
        auto lock = std::unique_lock{mutex_, std::defer_lock};
        while (true) {
            if (!has_data_ && lock.try_lock()) {
                has_data_ = true;
                data_ = std::move(new_data);
                cv_.notify_all();
                return;
            }

            cv_.wait(lock);
        }
    }
};

}  // namespace dsc
