// Copyright 2020 Nathaniel Mitchell

#pragma once

#include <utility>
#include <mutex>
#include <condition_variable>

namespace dsc {


/** Thread-safe storage object which can store a single item through get(), which can be retrieved with put(). */
template<typename T>
class box {
    T                       data_;
    bool                    has_data_;
    std::mutex              mutex_;
    std::condition_variable get_cv_, put_cv_;

 public:
    /** Constructs an empty box. */
    box():  data_(),
            has_data_(false),
            mutex_() {}

    /** Constructs a box with initial data. */
    explicit box(T init_data):  data_(std::move(init_data)),
                                has_data_(true),
                                mutex_() {}

    /** Retrieves item from box. If there is no item, blocks until an item is placed inside. */
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

    /** Places item within box. If there is already an item, blocks until the box is empty. */
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
