// Copyright Nathaniel Mitchell 2020

#include <iostream>
#include <thread>
#include "dsc/safe_box.hpp"

using std::cout;

void getter(safe_box<int>& box, int idx) {
    for (int i=0; i<10; i++) {
        auto v = box.get();
        cout << "Getter " << idx << " recieved " << v << "\n";
    }
}

void putter(safe_box<int>& box, int idx) {
    for (int i=0; i<10; i++) {
        box.put(idx);
        cout << "Setter " << idx << " placed value\n";
    }
}

int main() {
    auto box = safe_box<int>{};

    auto getter1 = std::thread{getter, box, 1};
    auto getter2 = std::thread{getter, box, 2};
    auto getter3 = std::thread{getter, box, 3};

    auto putter1 = std::thread{putter, box, 1};
    auto putter2 = std::thread{putter, box, 2};
    auto putter3 = std::thread{putter, box, 3};

    getter1.join();
    getter2.join();
    getter3.join();

    putter1.join();
    putter2.join();
    putter3.join();
}
