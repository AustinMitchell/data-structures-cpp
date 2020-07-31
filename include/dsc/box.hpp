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
    std::condition_variable get_cv_, put_cv_;

 public:
    box():  data_(),
            has_data_(false),
            mutex_() {}

    explicit box(T init_data):  data_(std::move(init_data)),
                                has_data_(false),
                                mutex_() {}

    auto get() -> T {
        auto lock = std::unique_lock{mutex_};

        while (true) {
            if (has_data_) {
                has_data_ = false;
                auto ret = T{std::move(data_)};
                put_cv_.notify_all();
                return ret;
            }

            get_cv_.wait(lock);
        }
    }

    auto put(T new_data) -> void {
        auto lock = std::unique_lock{mutex_};

        while (true) {
            if (!has_data_) {
                has_data_ = true;
                data_ = std::move(new_data);
                get_cv_.notify_all();
                return;
            }

            // note: wait() will unlock and then reacquire lock after being notified
            put_cv_.wait(lock);
        }
    }
};

}  // namespace dsc
