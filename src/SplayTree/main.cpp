#include <iostream>
#include "SplayTree.hpp"

int main() {
    auto x = 5;
    auto node = Node<int>{x};
    node.insert(1);
    node.insert(7);
    node.insert(3);
    node.insert(6);

    std::cout << node.to_string() << std::endl;

    return 0;
}