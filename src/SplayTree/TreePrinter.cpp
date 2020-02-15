// Copyright 2020 Nathaniel Mitchell
// Original source code derived from here: https://web.archive.org/web/20090617110918/http://www.openasthra.com/c-tidbits/printing-binary-trees-in-ascii/
// Converted into C++17 code

#include <stdio.h>
#include <optional>
#include <memory>
#include <algorithm>

#include "Node.hpp"

#define MAX_HEIGHT 1000
#define INFINITY (1 << 20)

typedef struct asciinode_struct asciinode_t;

// Renaming to reduce gunk
using opt_node  = std::optional<Node<int>*>;
using ascii_ptr = std::unique_ptr<asciinode_t>;

// printing tree in ascii
struct asciinode_struct {
    ascii_ptr left, right;

    // length of the edge from this node to its children
    int edge_length;

    int height;

    int lablen;

    //-1=I am left, 0=I am root, 1=right
    int parent_dir;

    // max supported unit32 in dec, 10 digits max
    std::string label;
};

typedef struct print_info_ {
    int lprofile[MAX_HEIGHT];
    int rprofile[MAX_HEIGHT];
    int gap; // adjust gap between left and right nodes
    int print_next; // used for printing next node in the same level, this is the x coordinate of the next char printed

    print_info_(): gap(3) {}
} print_info_t;


static auto left(opt_node node) -> opt_node {
    if (node) {
        return (*node)->left();
    } else {
        return {std::nullopt};
    }
}


static auto right(opt_node node) -> opt_node {
    return node ? (*node)->right() : std::nullopt;
}


static auto data(opt_node node) -> int {
    return (*node)->data();
}


// Find the smallest node of a tree
static auto find_min(opt_node node) -> opt_node {
    if (!node) {
        return std::nullopt;
    } else if (!left(node)) {
        return node;
    } else {
        return find_min(left(node));
    }
}


// Find the largest node of a tree
static auto find_max(opt_node node) -> opt_node {
    if (!node) {
        return std::nullopt;
    } else if (!right(node)) {
        return node;
    } else {
        return find_max(right(node));
    }
}

// Search for the node with value equal to elem
static auto find(int elem, opt_node node) -> opt_node {
    if (!node) {
        return std::nullopt;
    }

    if (elem < data(node)) {
        return find(elem, left(node));
    } else if (elem > data(node)) {
        return find(elem, right(node));
    } else {
        return node;
    }
}


static auto build_ascii_tree_recursive(opt_node root) -> ascii_ptr {
    if (!root) return {};

    auto node = ascii_ptr{};

    node = std::make_unique<asciinode_t>();
    node->left = build_ascii_tree_recursive(left(root));
    node->right = build_ascii_tree_recursive(right(root));

    if (node->left) {
        node->left->parent_dir = -1;
    }

    if (node->right) {
        node->right->parent_dir = 1;
    }

    node->label = std::to_string(data(root));
    node->lablen = node->label.size();

    return node;
}

// Copy the tree into the ascii node structre
static auto build_ascii_tree(opt_node root) -> ascii_ptr {
    auto node = ascii_ptr{};

    if (!root)
        return {};

    node = build_ascii_tree_recursive(root);
    node->parent_dir = 0;
    return node;
}

// The following function fills in the lprofile array for the given tree.
// It assumes that the center of the label of the root of this tree
// is located at a position (x,y).  It assumes that the edge_length
// fields have been computed for this tree.
static void compute_lprofile(print_info_t& pinfo, ascii_ptr& node, int x, int y) {
    int i, isleft;

    if (!node)
        return;

    isleft = (node->parent_dir == -1);
    pinfo.lprofile[y] = std::min(pinfo.lprofile[y], x - ((node->lablen - isleft) / 2));

    if (node->left != NULL) {
        for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
            pinfo.lprofile[y + i] = std::min(pinfo.lprofile[y + i], x - i);
        }
    }

    compute_lprofile(pinfo, node->left, x - node->edge_length - 1,
                     y + node->edge_length + 1);
    compute_lprofile(pinfo, node->right, x + node->edge_length + 1,
                     y + node->edge_length + 1);
}

static void compute_rprofile(print_info_t& pinfo, ascii_ptr& node, int x, int y) {
    int i, notleft;

    if (!node)
        return;

    notleft = (node->parent_dir != -1);
    pinfo.rprofile[y] = std::max(pinfo.rprofile[y], x + ((node->lablen - notleft) / 2));

    if (node->right != NULL) {
        for (i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
            pinfo.rprofile[y + i] = std::max(pinfo.rprofile[y + i], x + i);
        }
    }

    compute_rprofile(pinfo, node->left, x - node->edge_length - 1,
                     y + node->edge_length + 1);
    compute_rprofile(pinfo, node->right, x + node->edge_length + 1,
                     y + node->edge_length + 1);
}

// This function fills in the edge_length and
// height fields of the specified tree
static void compute_edge_lengths(print_info_t& pinfo, ascii_ptr& node) {
    auto h      = int{};
    auto hmin   = int{};
    auto i      = int{};
    auto delta  = int{};

    if (!node)
        return;

    compute_edge_lengths(pinfo, node->left);
    compute_edge_lengths(pinfo, node->right);

    /* first fill in the edge_length of node */
    if (!node->right && !node->left) {
        node->edge_length = 0;
    } else {
        if (node->left != NULL) {
            for (i = 0; i < node->left->height && i < MAX_HEIGHT; i++) {
                pinfo.rprofile[i] = -INFINITY;
            }
            compute_rprofile(pinfo, node->left, 0, 0);
            hmin = node->left->height;
        } else {
            hmin = 0;
        }
        if (node->right != NULL) {
            for (i = 0; i < node->right->height && i < MAX_HEIGHT; i++) {
                pinfo.lprofile[i] = INFINITY;
            }
            compute_lprofile(pinfo, node->right, 0, 0);
            hmin = std::min(node->right->height, hmin);
        } else {
            hmin = 0;
        }
        delta = 4;
        for (i = 0; i < hmin; i++) {
            delta = std::max(delta, pinfo.gap + 1 + pinfo.rprofile[i] - pinfo.lprofile[i]);
        }

        // If the node has two children of height 1, then we allow the
        // two leaves to be within 1, instead of 2
        if (((node->left != NULL && node->left->height == 1) ||
             (node->right != NULL && node->right->height == 1)) &&
            delta > 4) {
            delta--;
        }

        node->edge_length = ((delta + 1) / 2) - 1;
    }

    // now fill in the height of node
    h = 1;
    if (node->left != NULL) {
        h = std::max(node->left->height + node->edge_length + 1, h);
    }
    if (node->right != NULL) {
        h = std::max(node->right->height + node->edge_length + 1, h);
    }
    node->height = h;
}

// This function prints the given level of the given tree, assuming
// that the node has the given x cordinate.
static void print_level(print_info_t& pinfo, ascii_ptr& node, int x, int level) {
    auto i      = int{};
    auto isleft = int{};

    if (!node)
        return;

    isleft = (node->parent_dir == -1);

    if (level == 0) {
        for (i = 0; i < (x - pinfo.print_next - ((node->lablen - isleft) / 2)); i++) {
            std::cout << " ";
        }
        pinfo.print_next += i;
        std::cout << node->label;
        pinfo.print_next += node->lablen;
    } else if (node->edge_length >= level) {
        if (node->left != NULL) {
            for (i = 0; i < (x - pinfo.print_next - (level)); i++) {
                std::cout << " ";
            }
            pinfo.print_next += i;
            std::cout << "/";
            pinfo.print_next++;
        }
        if (node->right != NULL) {
            for (i = 0; i < (x - pinfo.print_next + (level)); i++) {
                printf(" ");
            }
            pinfo.print_next += i;
            std::cout << "\\";
            pinfo.print_next++;
        }
    } else {
        print_level(pinfo, node->left, x - node->edge_length - 1,
                    level - node->edge_length - 1);
        print_level(pinfo, node->right, x + node->edge_length + 1,
                    level - node->edge_length - 1);
    }
}

// prints ascii tree for given Tree structure
void print_ascii_tree(opt_node t) {
    auto proot  = ascii_ptr{};
    auto xmin   = int{};
    auto i      = int{};
    auto pinfo  = print_info_t{};

    if (!t)
        return;

    proot = build_ascii_tree(t);

    compute_edge_lengths(pinfo, proot);
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
        pinfo.lprofile[i] = INFINITY;
    }

    compute_lprofile(pinfo, proot, 0, 0);
    xmin = 0;
    for (i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
        xmin = std::min(xmin, pinfo.lprofile[i]);
    }
    for (i = 0; i < proot->height; i++) {
        pinfo.print_next = 0;
        print_level(pinfo, proot, -xmin, i);
        std::cout << "\n";
    }
    if (proot->height >= MAX_HEIGHT) {
        std::cout << "(This tree is taller than " << MAX_HEIGHT << " and may be drawn incorrectly.)\n";
    }
}
