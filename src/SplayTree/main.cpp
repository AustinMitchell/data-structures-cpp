// Copyright 2019 Nathaniel Mitchell

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

#include "SplayTree.hpp"
#include "TreePrinter.hpp"

int main(int argc, char *argv[]) {
    std::srand(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    auto tree = SplayTree<int>{};
    auto list = std::vector<int>{};

    int size = argc >= 2 ? std::atoi(argv[1]) : 20;

    for (int i=1; i<=size; i++) {
        list.push_back(i);
    }

    std::random_shuffle(list.begin(), list.end());

    for (int i=0; i<list.size(); i++) {
        //std::cout << "inserting " << list[i] << "...\n";
        tree.insert(list[i]);
        //print_ascii_tree(tree.root());
    }
    print_ascii_tree(tree.root());

    return 0;
}