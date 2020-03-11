// Copyright 2020 Nathaniel Mitchell

#include "ring_vector.hpp"
#include <iostream>

int main() {
    ring_vector<int> vec;

    for (int i=1; i<=5; i++) {
        vec.push_back(i);
        vec.push_front(i);
    }

    std::cout << "Size:     " << vec.size() << '\n';
    std::cout << "Capacity: " << vec.capacity() << '\n';

    for (int i=0; i<vec.size(); i++) {
        std::cout << vec[i] << '\n';
    }
}