// Copyright 2019 Nathaniel Mitchell

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

#include "dsc/splay_tree.hpp"
#include "dsc/tree_printer.hpp"

using std::cout;

int main(int argc, char *argv[]) {
    std::srand(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    auto tree1 = dsc::splay_tree<int>{};
    auto tree2 = dsc::semisplay_tree<int>{};
    auto list = std::vector<int>{};

    const auto list_size = 40;

    list.reserve(list_size);
    for (int i=1; i<=list_size; i++) {
        list.push_back(i);
    }

    std::random_shuffle(list.begin(), list.end());

    for (auto i: list) {
        //cout << "inserting " << list[i] << "...\n";
        tree1.insert(i);
        tree2.insert(i);
        //print_ascii_tree(tree.root());
    }

    cout << "Final splay tree:\n";
    dsc::print_ascii_tree(tree1.root());
    cout << "Height: " << tree1.height() << "\n";
    cout << "\n";
    cout << "Final semisplay tree:\n";
    dsc::print_ascii_tree(tree2.root());
    cout << "Height: " << tree2.height() << "\n";

    cout << "\n";
    cout << "Insert order:                  ";
    for(auto v: list) {
        cout << v << " ";
    }

    cout << "\n";
    cout << "In-order traversal full splay: ";
    for(auto const& v: tree1) {
        cout << v << " ";
    }

    cout << "\n";
    cout << "In-order traversal semi splay: ";
    for(auto const& v: tree2) {
        cout << v << " ";
    }
    cout << "\n\n";

    cout << "Testing full splay tree for all values in order using contains()...\n";
    for(auto i: list) {
        if (!tree1.contains(i)) {
            cout << "Full splay tree does not contain " << i << "\n";
        }
    }
    cout << "\n";
    cout << "Testing semi splay tree for all values in order using contains()...\n";
    for(auto i: list) {
        if (!tree2.contains(i)) {
            cout << "Full splay tree does not contain " << i << "\n";
        }
    }
    cout << "\n";

    cout << "Splay tree after contains operations:\n";
    dsc::print_ascii_tree(tree1.root());
    cout << "\n";
    cout << "Semisplay tree after contains operations:\n";
    dsc::print_ascii_tree(tree2.root());
    cout << "\n";

    cout << "Creating a balanced tree with vector constructor\n";
    auto sorted = std::vector<int>{};
    auto size   = argc >= 2 ? std::atoi(argv[1]) : 15;
    for (auto i=1; i<=size; i++) {
        sorted.push_back(i);
    }
    auto balanced_tree = dsc::splay_tree<int>{sorted};

    cout << "Balanced splay tree:\n";
    dsc::print_ascii_tree(balanced_tree.root());

    return 0;
}