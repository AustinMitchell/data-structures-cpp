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
    template<typename U, typename splay_type> friend class splay_tree;

    ////////////////////////////////////////////////////////////////
    // ------------------------- FIELDS ------------------------- //
    ////////////////////////////////////////////////////////////////
    T       m_data;
    splay_tree_node<T> *m_parent;
    splay_tree_node<T> *m_left;
    splay_tree_node<T> *m_right;


 public:

    ////////////////////////////////////////////////////////////////
    // ----------------------- PROPERTIES ----------------------- //
    ////////////////////////////////////////////////////////////////

    auto data() const -> const T& { return  m_data; }

    /** Returns the left child if there is a left child, otherwise returns nullopt */
    auto left() const -> std::optional<const splay_tree_node<T>*> {
        if (m_left) {
            return {m_left};
        } else {
            return {std::nullopt};
        }
    }

    /** Returns the right child if there is a right child, otherwise returns nullopt */
    auto right() const -> std::optional<const splay_tree_node<T>*> {
        if (m_right) {
            return {m_right};
        } else {
            return {std::nullopt};
        }
    }

    /** Returns the parent if there is a parent, otherwise returns nullopt */
    auto parent() const -> std::optional<const splay_tree_node<T>*> {
        if (m_parent) {
            return {m_parent};
        } else {
            return {std::nullopt};
        }
    }


    ////////////////////////////////////////////////////////////////
    // ---------------------- CONSTRUCTORS ---------------------- //
    ////////////////////////////////////////////////////////////////

    template<typename U>
    splay_tree_node(U&& data) : m_data{std::forward<U>(data)}, m_parent(nullptr), m_left(nullptr), m_right(nullptr) {}


    ////////////////////////////////////////////////////////////////
    // ----------------------- OPERATIONS ----------------------- //
    ////////////////////////////////////////////////////////////////

};