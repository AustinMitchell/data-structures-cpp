// Copyright 2020 Nathaniel Mitchell

#include <iostream>
#include <utility>

#include "dsc/ring_vector.hpp"

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
    auto vec = dsc::ring_vector<Test>{50};

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

    std::cout << "Removing 3 elements from vector and shrinking...\n";
    for (int i=1; i<=3; i++) {
        vec.pop_back();
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
    std::cout << "\n\n";

    std::cout << "Copying vector\n";
    auto copyvec = vec;

    std::cout << "Values: ";
    for(auto& v: copyvec) {
        std::cout << v.val() << " ";
    }
    std::cout << "\n";
    std::cout << "Original Vector Size:     " << vec.size() << "\n";
    std::cout << "Original Vector Capacity: " << vec.capacity() << "\n";

    std::cout << "\n";
    std::cout << "Moving vector\n";
    auto movevec = std::move(vec);

    std::cout << "Values: ";
    for(auto& v: movevec) {
        std::cout << v.val() << " ";
    }
    std::cout << "\n";
    std::cout << "Original Vector Size:     " << vec.size() << "\n";
    std::cout << "Original Vector Capacity: " << vec.capacity() << "\n";

    std::cout << "\n";
    std::cout << "Copy-assigning vector\n";
    auto copyassvec = decltype(copyvec){};
    copyassvec      = copyvec;

    std::cout << "Values: ";
    for(auto& v: copyassvec) {
        std::cout << v.val() << " ";
    }
    std::cout << "\n";
    std::cout << "Original Vector Size:     " << copyvec.size() << "\n";
    std::cout << "Original Vector Capacity: " << copyvec.capacity() << "\n";

    std::cout << "\n";
    std::cout << "Moving vector\n";
    auto moveassvec = decltype(copyvec){};
    moveassvec      = std::move(copyvec);

    std::cout << "Values: ";
    for(auto& v: moveassvec) {
        std::cout << v.val() << " ";
    }
    std::cout << "\n";
    std::cout << "Original Vector Size:     " << copyvec.size() << "\n";
    std::cout << "Original Vector Capacity: " << copyvec.capacity() << "\n";


    std::cout << "\n";
    std::cout << "Constructing array of 10 elements to test += / -= on iterator using front/back emplacement...\n";
    for (int i=6; i<=10; i++) {
        vec.emplace_back(i);
    }
    for (int i=5; i>0; i--) {
        vec.emplace_front(i);
    }

    std::cout << "Values: ";
    for(auto& v: vec) {
        std::cout << v.val() << " ";
    }
    std::cout << "\n";

    auto it = vec.begin();
    std::cout << "it = vec.begin()  => Expected: 1,    Actual: " << it->val() << "\n";
    it += 2;
    std::cout << "it += 2           => Expected: 3,    Actual: " << it->val() << "\n";
    it -= 3;
    std::cout << "it -= 3           => Expected: 1,    Actual: " << it->val() << "\n";
    std::cout << "it == vec.begin() => Expected: true, Actual: " << (it==vec.begin() ? "true" : "false") << "\n";
    it += 15;
    std::cout << "it += 15\n";
    std::cout << "it == vec.end()   => Expected: true, Actual: " << (it==vec.end() ? "true" : "false") << "\n";


}