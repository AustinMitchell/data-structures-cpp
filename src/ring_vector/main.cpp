// Copyright 2020 Nathaniel Mitchell

#include "ring_vector.hpp"
#include <iostream>

class Test {
    int * allocated;

 public:
    Test(int i) { allocated = new int(i); }
    Test(Test&& t) {
        allocated   = t.allocated;
        t.allocated = nullptr;
    }
    Test(const Test& t) {
        allocated = new int(*(t.allocated));
    }

    ~Test() { delete allocated; }

    int val() { return *allocated; }

    Test& operator=(Test&& t) {
        allocated   = t.allocated;
        t.allocated = nullptr;
        return *this;
    }
    Test& operator=(const Test& t) {
        delete allocated;
        allocated = new int(*(t.allocated));
        return *this;
    }
};

int main() {
    std::cout << "Creating vector with reserve space of 50\n";
    auto vec = ring_vector<Test>{50};

    std::cout << "Size:     " << vec.size() << "\n";
    std::cout << "Capacity: " << vec.capacity() << "\n";
    std::cout << "\n";

    std::cout << "Reserving space for 30 elements...\n";
    vec.reserve(30);

    std::cout << "Adding 10 elements with push front/back...\n";
    for (int i=1; i<=5; i++) {
        auto t1 = Test{i};
        auto t2 = Test{-i};
        vec.push_back(std::move(t1));
        vec.push_front(t2);
    }

    std::cout << "Values: ";
    for(auto& v: vec) {
        std::cout << v.val() << " ";
    }
    std::cout << "\n\n";

    std::cout << "Size:     " << vec.size() << "\n";
    std::cout << "Capacity: " << vec.capacity() << "\n";

    std::cout << "\n";
    while (!vec.empty()) {
        std::cout << "Popping front: " << vec.pop_front_get().val() << "\n";
        std::cout << "Size:          " << vec.size() << "\n";
        std::cout << "Capacity:      " << vec.capacity() << "\n";
        std::cout << "\n";
        if (!vec.empty()) {
            std::cout << "Popping back:  " << vec.pop_back_get().val() << "\n";
            std::cout << "Size:          " << vec.size() << "\n";
            std::cout << "Capacity:      " << vec.capacity() << "\n";
            std::cout << "\n";
        }
    }
    std::cout << "\n";

    std::cout << "Adding 10 elements to vector and shrinking...\n";
    for (int i=1; i<=10; i++) {
        vec.push_back({i});
    }
    vec.shrink_to_fit();

    std::cout << "Size:     " << vec.size() << "\n";
    std::cout << "Capacity: " << vec.capacity() << "\n";

    std::cout << "\n";
    std::cout << "Clearing and shrinking vector...\n";
    vec.clear();
    vec.shrink_to_fit();
    std::cout << "Size:     " << vec.size() << "\n";
    std::cout << "Capacity: " << vec.capacity() << "\n";

    std::cout << "\n";
    std::cout << "Using insert() to insert 1 at the beginning, 2 at the end, and 3 in the middle of vector of 0's...\n";

    for (int i=0; i<10; i++) {
        vec.push_back({0});
    }

    vec.insert(0, {1});
    std::cout << "Values: ";
    for(auto& v: vec) {
        std::cout << v.val() << " ";
    }
    std::cout << "\n";

    vec.insert(11, {1});
    std::cout << "Values: ";
    for(auto& v: vec) {
        std::cout << v.val() << " ";
    }
    std::cout << "\n";

    vec.insert(6, {1});
    std::cout << "Values: ";
    for(auto& v: vec) {
        std::cout << v.val() << " ";
    }
    std::cout << "\n";
}