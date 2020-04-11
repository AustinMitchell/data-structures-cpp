// Copyright 2019 Nathaniel Mitchell

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

#include <dsc/splay_tree.hpp>

using std::cout;
using timer = std::chrono::high_resolution_clock;

auto const NUM_VALUES = 1000000;
auto tree_full  = dsc::splay_tree<int>{};
auto tree_semi  = dsc::semisplay_tree<int>{};


auto construct_trees() {
    auto nums       = std::vector<int>{};
    nums.reserve(NUM_VALUES);
    for (auto i=1; i<=NUM_VALUES; i++) {
        nums.push_back(i);
    }

    //  to reduce initialization time, create initially balanced trees.
    tree_full = dsc::splay_tree<int>{nums};
    tree_semi = dsc::semisplay_tree<int>{nums};
}


auto main() -> int {
    auto rd             = std::random_device{};
    auto gen            = std::mt19937 {rd()};
    auto next_uniform   = std::uniform_int_distribution<>(1, NUM_VALUES);

    construct_trees();

    cout << "Deleting nodes in order on full splay tree\n";
    {
        auto prev       = tree_full.delete_min_no_splay();
        auto current    = tree_full.delete_min_no_splay();
        auto start      = timer::now();
        while (!tree_full.empty()) {
            prev    = current;
            current = tree_full.delete_min_no_splay();
        }
        auto end        = timer::now();
        cout << "Elapsed time: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000.0) << "\n";
    }

    cout << "\n";
    cout << "Deleting nodes in order on semi splay tree\n";
    {
        auto prev       = tree_semi.delete_min_no_splay();
        auto current    = tree_semi.delete_min_no_splay();
        auto start      = timer::now();
        while (!tree_semi.empty()) {
            prev    = current;
            current = tree_semi.delete_min_no_splay();
        }
        auto end        = timer::now();
        cout << "Elapsed time: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000.0) << "\n";
    }
}