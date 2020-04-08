// Copyright 2020 Nathaniel Mitchell
// Original source code derived from here: https://web.archive.org/web/20090617110918/http://www.openasthra.com/c-tidbits/printing-binary-trees-in-ascii/
// Converted into C++17 code

#include <stdio.h>
#include <optional>
#include <memory>
#include <algorithm>

#include "splay_tree_node.hpp"

namespace dsc {
    // prints ascii tree for given Tree structure
    void print_ascii_tree(splay_tree_node<int> const* root);
}
