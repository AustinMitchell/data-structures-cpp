// Copyright 2020 Nathaniel Mitchell

#pragma once

#include "SplayTree.hpp"
#include <iostream>
#include <memory>
#include <utility>
#include <sstream>
#include <string>

////////////////////////////////////////////////////////////////
// ------------------------- Node<T> ------------------------ //
////////////////////////////////////////////////////////////////

/* ========================================================== */
/* ======================  SUBSECTION  ====================== */


template<typename T>
std::string to_string(const Node<T>& node) {
    using std::to_string;

    std::stringstream ss;
    ss << node.data_;

    if (node.left_ || node.right_) {
        ss << " {";
        if (node.left_) {
            ss << std::to_string(node.left_);
        } else {
            ss << "-";
        }
        ss << " ";
        if (node.right_) {
            ss << std::to_string(node.right_);
        } else {
            ss << "-";
        }
        ss << "}";
    }
    return ss.str();
}

////////////////////////////////////////////////////////////////
// ---------------------- SplayTree<T> ---------------------- //
////////////////////////////////////////////////////////////////

/* ========================================================== */
/* ======================  OPERATIONS  ====================== */

template<typename T>
auto SplayTree<T>::insert(T data) -> void {
    if (root) {
        root->insert(std::move(data));
    } else {
        root = std::make_unique<Node<T>>(std::move(data));
    }
}

template<typename T>
std::string to_string(const SplayTree<T> tree) {
    using std::to_string;
    if (tree.root) {
        return std::to_string(tree.root);
    } else {
        return "";
    }
}