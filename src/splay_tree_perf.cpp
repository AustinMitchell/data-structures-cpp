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
auto rd             = std::random_device{};
auto gen            = std::mt19937 {rd()};


auto construct_trees() {
    cout << "Constructing trees with " << NUM_VALUES << " nodes...\n";

    auto nums       = std::vector<int>{};
    nums.reserve(NUM_VALUES);
    for (auto i=1; i<=NUM_VALUES; i++) {
        nums.push_back(i);
    }

    //  to reduce initialization time, create initially balanced trees.
    tree_full = dsc::splay_tree<int>{nums};
    tree_semi = dsc::semisplay_tree<int>{nums};
}


auto test_uniform(int num_operations) {
    auto next_uniform = std::uniform_int_distribution<>(1, NUM_VALUES);

    auto list = std::vector<int>{};
    list.reserve(num_operations);
    for (auto i=0; i<num_operations; i++) {
        list.push_back(next_uniform(gen));
    }

    cout << "Uniform Distribution: " << num_operations << " find operations\n";

    cout << "   Testing full splay tree...\n";
    {
        auto start  = timer::now();
        for (auto i: list) {
            tree_full.contains(i);
        }
        auto end    = timer::now();
        cout << "   Elapsed time: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000.0) << "\n";
        cout << "   Height: " << tree_full.height() << "\n";
        cout << "\n";
    }

    cout << "   Testing semi splay tree...\n";
    {
        auto start  = timer::now();
        for (auto i: list) {
            tree_semi.contains(i);
        }
        auto end    = timer::now();
        cout << "   Elapsed time: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000.0) << "\n";
        cout << "   Height: " << tree_semi.height() << "\n";
        cout << "\n";
    }
}

auto test_binomial(int num_operations) {
    auto next_binomial = std::binomial_distribution<>(NUM_VALUES, 0.5);

    auto list = std::vector<int>{};
    list.reserve(num_operations);
    for (auto i=0; i<num_operations; i++) {
        list.push_back(next_binomial(gen));
    }

    cout << "\n";
    cout << "Binomial Distribution: " << num_operations << " find operations\n";

    cout << "   Testing full splay tree...\n";
    {
        auto start  = timer::now();
        for (auto i: list) {
            tree_full.contains(i);
        }
        auto end    = timer::now();
        cout << "   Elapsed time: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000.0) << "\n";
        cout << "   Height: " << tree_full.height() << "\n";
        cout << "\n";
    }

    cout << "   Testing semi splay tree...\n";
    {
        auto start  = timer::now();
        for (auto i: list) {
            tree_semi.contains(i);
        }
        auto end    = timer::now();
        cout << "   Elapsed time: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000.0) << "\n";
        cout << "   Height: " << tree_semi.height() << "\n";
        cout << "\n";
    }
}

auto test_alternating_normal(int num_operations, int alternate_length, double stddev) {
    auto list = std::vector<int>{};
    auto range = std::uniform_real_distribution<>{0, 1};

    list.reserve(num_operations);
    while (list.size() < static_cast<std::vector<int>::size_type>(num_operations)) {
        auto next_normal  = std::normal_distribution<>{NUM_VALUES*range(gen), stddev};
        for (auto i=0; i<alternate_length; i++) {
            list.push_back(next_normal(gen));
        }
    }

    cout << "\n";
    cout << "Normal Distribution, std. dev of " << stddev << ", changing mean every "
         << alternate_length << " operations: " << num_operations << " total find operations\n";

    cout << "   Testing full splay tree...\n";
    {
        auto start  = timer::now();
        for (auto i: list) {
            tree_full.contains(i);
        }
        auto end    = timer::now();
        cout << "   Elapsed time: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000.0) << "\n";
        cout << "   Height: " << tree_full.height() << "\n";
        cout << "\n";
    }

    cout << "   Testing semi splay tree...\n";
    {
        auto start  = timer::now();
        for (auto i: list) {
            tree_semi.contains(i);
        }
        auto end    = timer::now();
        cout << "   Elapsed time: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000.0) << "\n";
        cout << "   Height: " << tree_semi.height() << "\n";
        cout << "\n";
    }
}


auto main() -> int {
    construct_trees();

    //test_uniform(3000000);
    //test_binomial(3000000);
    test_alternating_normal(3000000, 10000, 1000);

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