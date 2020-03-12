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
    ring_vector<Test> vec;

    for (int i=1; i<=5; i++) {
        vec.push_back({i});
        vec.push_front({-i});
    }

    std::cout << "Size:     " << vec.size() << '\n';
    std::cout << "Capacity: " << vec.capacity() << '\n';

    for (;!vec.empty();) {
        std::cout << vec[0].val() << '\n';
        vec.pop_front();
    }
}