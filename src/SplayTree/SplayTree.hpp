// Copyright 2020 Nathaniel Mitchell

#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <sstream>
#include <optional>

#include "Node.hpp"
#include "ring_vector/ring_vector.hpp"


template<typename T>
class SplayTree {
 private:

    ////////////////////////////////////////////////////////////////
    // ------------------------- FIELDS ------------------------- //
    ////////////////////////////////////////////////////////////////

    std::unique_ptr<Node<T>> m_root;
    std::size_t m_size;

 public:

    ////////////////////////////////////////////////////////////////
    // ---------------------- CONSTRUCTORS ---------------------- //
    ////////////////////////////////////////////////////////////////

    SplayTree(): m_size(0) {}

    ~SplayTree() {
        auto node_queue = ring_vector<Node<T>*>{m_size};
        if (!m_root) {
            return;
        }

        if (m_root->left_) {
            node_queue.push_back(m_root->left_);
        }
        if (m_root->right_) {
            node_queue.push_back(m_root->right_);
        }

        while(!node_queue.empty()) {
            Node<T> *next = node_queue.pop_front_get();
            if (next->left_) {
                node_queue.push_back(next->left_);
            }
            if (next->right_) {
                node_queue.push_back(next->right_);
            }
            delete next;
        }
    }


    ////////////////////////////////////////////////////////////////
    // ----------------------- PROPERTIES ----------------------- //
    ////////////////////////////////////////////////////////////////

    auto root() -> std::optional<Node<T>*> {
        if (m_root) {
            return {m_root.get()};
        } else {
            return {std::nullopt};
        }
    }

    auto size() const -> std::size_t { return m_size; }


    ////////////////////////////////////////////////////////////////
    // ----------------------- OPERATIONS ----------------------- //
    ////////////////////////////////////////////////////////////////

    template<typename U=T>
    auto insert(U&& data) -> void {
        m_size++;
        if (m_root) {
            m_root->insert(std::forward<U>(data));
        } else {
            m_root = std::make_unique<Node<T>>(std::forward<U>(data));
        }
    }

    auto to_string() const -> std::string {
        using std::to_string;
        if (m_root) {
            return m_root->to_string();
        } else {
            return "";
        }
    }
};
