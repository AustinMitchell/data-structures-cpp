// Copyright 2020 Nathaniel Mitchell
// Original source code derived from here: https://web.archive.org/web/20090617110918/http://www.openasthra.com/c-tidbits/printing-binary-trees-in-ascii/
// Converted into C++17 code

#include <stdio.h>
#include <optional>
#include <memory>
#include <algorithm>

#include "splay_tree_node.hpp"

typedef struct asciinode_struct asciinode_t;

typedef struct print_info_ print_info_t;

// prints ascii tree for given Tree structure
void print_ascii_tree(std::optional<splay_tree_node<int>*> root);
