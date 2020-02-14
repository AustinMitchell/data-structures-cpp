// Copyright 2019 Nathaniel Mitchell

#include "SplayTree.hpp"
#include <iostream>
#include <string>

int main() {
    auto tree = SplayTree<int>();
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);
    tree.insert(4);
    tree.insert(5);
    tree.insert(6);
    tree.insert(7);
    tree.insert(8);
    tree.insert(9);
    tree.insert(0);
    tree.insert(1);

    std::cout << "Current tree:\n";
    std::cout << tree.to_string() << '\n';

    std::cout << std::endl;
    return 0;
}