// Copyright 2019 Nathaniel Mitchell

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

#include "dsc/splay_tree.hpp"
#include "dsc/tree_printer.hpp"

int main(int argc, char *argv[]) {
    std::srand(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    auto tree1 = splay_tree<int>{};
    auto tree2 = semisplay_tree<int>{};
    auto list = std::vector<int>{};

    int size = argc >= 2 ? std::atoi(argv[1]) : 20;

    list.reserve(size);
    for (int i=1; i<=size; i++) {
        list.push_back(i);
    }

    std::random_shuffle(list.begin(), list.end());

    for (auto i: list) {
        //std::cout << "inserting " << list[i] << "...\n";
        tree1.insert(i);
        tree2.insert(i);
        //print_ascii_tree(tree.root());
    }

    std::cout << "Final splay tree:\n";
    print_ascii_tree(tree1.root());
    std::cout << "\n";
    std::cout << "Final semisplay tree:\n";
    print_ascii_tree(tree2.root());

    std::cout << "\n";
    std::cout << "Insert order:                  ";
    for(auto v: list) {
        std::cout << v << " ";
    }

    std::cout << "\n";
    std::cout << "In-order traversal full splay: ";
    for(auto const& v: tree1) {
        std::cout << v << " ";
    }

    std::cout << "\n";
    std::cout << "In-order traversal semi splay: ";
    for(auto const& v: tree2) {
        std::cout << v << " ";
    }
    std::cout << "\n";

    return 0;
}