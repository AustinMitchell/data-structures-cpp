// Copyright 2020 Nathaniel Mitchell

#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <sstream>
#include <optional>
#include <functional>


template<typename T>
class splay_tree_node {
 private:
    template<typename U, typename splay_type, typename Allocator> friend class splay_tree;

    ////////////////////////////////////////////////////////////////
    // ------------------------- FIELDS ------------------------- //
    ////////////////////////////////////////////////////////////////
    T m_data;

    splay_tree_node<T> *m_parent;
    splay_tree_node<T> *m_left;
    splay_tree_node<T> *m_right;


 public:

    ////////////////////////////////////////////////////////////////
    // ----------------------- PROPERTIES ----------------------- //
    ////////////////////////////////////////////////////////////////

    auto data() const -> const T& { return  m_data; }

    /** Returns the left child if there is a left child, otherwise returns nullopt */
    auto left() const -> splay_tree_node<T> const* {
        return m_left;
    }

    /** Returns the right child if there is a right child, otherwise returns nullopt */
    auto right() const -> splay_tree_node<T> const* {
        return m_right;
    }

    /** Returns the parent if there is a parent, otherwise returns nullopt */
    auto parent() const -> splay_tree_node<T> const* {
        return m_parent;
    }

    ////////////////////////////////////////////////////////////////
    // ---------------------- CONSTRUCTORS ---------------------- //
    ////////////////////////////////////////////////////////////////

    template<typename U=T>
    splay_tree_node(U&& data, splay_tree_node<T> *parent=nullptr) : m_data{std::forward<U>(data)}, m_parent(parent), m_left(nullptr), m_right(nullptr) {}


    ////////////////////////////////////////////////////////////////
    // ----------------------- OPERATIONS ----------------------- //
    ////////////////////////////////////////////////////////////////

};