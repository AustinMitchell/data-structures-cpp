// Copyright Nathaniel Mitchell 2020

#include <iostream>
#include <thread>
#include <mutex>
#include "dsc/box.hpp"

using std::cout;

std::mutex cout_mutex;

void getter(dsc::box<int>& box, int idx, int rounds) {
    {
        auto lock = std::lock_guard{cout_mutex};
        cout << "Getter " << idx << " started\n";
    }
    for (int i=0; i<rounds; i++) {
        auto v = box.get();
        {
            auto lock = std::lock_guard{cout_mutex};
            cout << "Getter " << idx << " recieved " << v << "\n";
        }
    }
}

void putter(dsc::box<int>& box, int idx, int rounds) {
    {
        auto lock = std::lock_guard{cout_mutex};
        cout << "Setter " << idx << " started\n";
    }
    for (int i=0; i<rounds; i++) {
        box.put(idx);
        {
            auto lock = std::lock_guard{cout_mutex};
            cout << "Setter " << idx << " placed value\n";
        }
    }
}

int main() {
    auto box = dsc::box<int>{};

    auto getter1 = std::thread{getter, std::ref(box), 1, 5};
    auto getter2 = std::thread{getter, std::ref(box), 2, 5};
    auto getter3 = std::thread{getter, std::ref(box), 3, 5};

    auto putter1 = std::thread{putter, std::ref(box), 1, 5};
    auto putter2 = std::thread{putter, std::ref(box), 2, 5};
    auto putter3 = std::thread{putter, std::ref(box), 3, 5};

    getter1.join();
    getter2.join();
    getter3.join();

    putter1.join();
    putter2.join();
    putter3.join();
}
