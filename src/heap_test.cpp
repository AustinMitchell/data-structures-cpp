#include <print>
#include <format>
#include <random>
#include <vector>

#include "dsc/heap.hpp"

int main() {
    dsc::heap<int, dsc::max_heap> max_heap;
    dsc::heap<int, dsc::min_heap> min_heap;
    auto list = std::vector<int>{};

    const auto list_size = 100;

    list.reserve(list_size);
    for (int i=1; i<=list_size; i++) {
        list.push_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd()); 
    std::shuffle(list.begin(), list.end(), g);

    for (auto i: list) {
        max_heap.push(i);
        min_heap.push(i);
    }

    std::println("Popping min heap:");
    while (!min_heap.empty()) {
        std::println("  {}", min_heap.pop());
    }
    std::println("");

    std::println("Popping max heap:");
    while (!max_heap.empty()) {
        std::println("  {}", max_heap.pop());
    }

    return 0;
}