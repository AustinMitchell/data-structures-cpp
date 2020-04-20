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


/** Constructs both trees as perfectly balanced binary trees */
auto construct_trees_balanced() {
    cout << "Constructing balanced trees with values 1 to " << NUM_VALUES << "...\n";

    auto list       = std::vector<int>{};
    list.reserve(NUM_VALUES);
    for (auto i=1; i<=NUM_VALUES; i++) {
        list.push_back(i);
    }
    //  to reduce initialization time, create initially balanced trees.
    tree_full = dsc::splay_tree<int>{list};
    tree_semi = dsc::semisplay_tree<int>{list};

    cout << "   Full splay height: " << tree_full.height() << "\n";
    cout << "   Semi splay height: " << tree_semi.height() << "\n";
    cout << "\n";
}


/** Constructs both trees with the values 1 to NUM_VALUES using insertion in random order. */
auto construct_trees() {
    cout << "Constructing trees by insertion with values 1 to " << NUM_VALUES << ", randomly ordered...\n";

    auto list       = std::vector<int>{};
    list.reserve(NUM_VALUES);
    for (auto i=1; i<=NUM_VALUES; i++) {
        list.push_back(i);
    }
    std::random_shuffle(list.begin(), list.end());

    cout << "   Inserting into full splay tree...\n";
    {
        auto start  = timer::now();
        for (auto i: list) {
            tree_full.insert(i);
        }
        auto end    = timer::now();
        cout << "   Elapsed time: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000.0) << "\n";
        cout << "   Height: " << tree_full.height() << "\n";
        cout << "\n";
    }

    cout << "   Inserting into semi splay tree...\n";
    {
        auto start  = timer::now();
        for (auto i: list) {
            tree_semi.insert(i);
        }
        auto end    = timer::now();
        cout << "   Elapsed time: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000.0) << "\n";
        cout << "   Height: " << tree_semi.height() << "\n";
        cout << "\n";
    }
}

/** Tests the performance of {num_operations} contains calls on both tree types with a uniform distribution. */
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

auto test_alternating_normal(unsigned long num_operations, int alternate_length, double stddev) {
    auto list = std::vector<int>{};
    auto range = std::uniform_real_distribution<>{0, 1};

    list.reserve(num_operations);
    while (list.size() < num_operations) {
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

auto test_uniform_repeating(unsigned long num_operations, int repeat_count, int repeat_set_size) {
    auto list = std::vector<int>{};
    auto next = std::uniform_int_distribution<>{1, NUM_VALUES};

    list.reserve(num_operations);
    while (list.size() < num_operations) {
        auto repeat_list = std::vector<int>{};
        repeat_list.reserve(repeat_set_size);
        for (auto i=0; i<repeat_set_size; i++) {
            repeat_list.push_back(next(gen));
        }

        for (auto i=0; i<repeat_count; i++) {
            for (auto n: repeat_list) {
                list.push_back(n);
                i++;
            }
        }
    }

    cout << "\n";
    cout << "Uniform Distribution, repeating the same " << repeat_set_size << " search values for find for "
         << repeat_count << " operations: " << num_operations << " total find operations\n";

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


auto test_delete_nodes() -> void {
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


auto main() -> int {
    std::srand(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    construct_trees();
    //construct_trees_balanced();

    //test_uniform(1000000);
    //test_binomial(10000000);
    //test_alternating_normal(10000000, 1000, 5);
    //test_uniform_repeating(30000000, 20, 1);
    //test_uniform_repeating(30000000, 50, 3);
    //test_uniform_repeating(30000000, 100, 5);
    //test_uniform_repeating(30000000, 100, 10);
    test_delete_nodes();
}