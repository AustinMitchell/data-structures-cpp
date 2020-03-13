// Copyright 2020 Nathaniel Mitchell

#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <sstream>
#include <optional>
#include <type_traits>

#include "splay_tree_node.hpp"
#include "ring_vector/ring_vector.hpp"

using std::size_t;

// Splay tree tags. Defines how the splay function will operate
struct fullsplay {};
struct semisplay {};

template<typename T, typename splay_type>
class splay_tree;

// Offering type name option for semisplay tree
template<typename T>
using semisplay_tree = splay_tree<T, semisplay>;

template<typename T, typename splay_type=fullsplay>
class splay_tree {
    static_assert(std::disjunction<
                        std::is_same<splay_type, fullsplay>,
                        std::is_same<splay_type, semisplay>
                    >::value,
                  "splay_type must either be fullsplay or semisplay");

 private:
    using stnode = splay_tree_node<T>;

    ////////////////////////////////////////////////////////////////
    // ------------------------- FIELDS ------------------------- //
    ////////////////////////////////////////////////////////////////

    size_t m_size;
    stnode *m_root;


    ////////////////////////////////////////////////////////////////
    // -------------------- SPLAY OPERATIONS -------------------- //
    ////////////////////////////////////////////////////////////////

    /**
     * Rotates right assuming this node is the left child of parent
     * @return  reference to where this nodes data was moved to
     */
    auto zig(stnode* node) -> stnode* {
        //std::cout << "zig\n";
        auto p  = node->m_parent;

        std::swap(p->m_left,        p->m_right);
        std::swap(node->m_left,     node->m_right);
        std::swap(node->m_right,    p->m_left);

        std::swap(node->m_data, p->m_data);

        if (node->m_right)  { node->m_right->m_parent   = node; }
        if (p->m_left)      { p->m_left->m_parent       = p; }

        return node->m_parent;
    }


    /**
     * Rotates left assuming this node is the right child of parent
     * @return  reference to where this nodes data was moved to
     */
    auto zag(stnode* node) -> stnode* {
        //std::cout << "zag\n";
        auto p  = node->m_parent;

        std::swap(p->m_left,        p->m_right);
        std::swap(node->m_left,     node->m_right);
        std::swap(node->m_left,     p->m_right);

        std::swap(node->m_data, p->m_data);

        if (node->m_left)   { node->m_left->m_parent    = node; }
        if (p->m_right)     { p->m_right->m_parent      = p; }

        return node->m_parent;
    }


    /**
     * Performs two right rotations assuming this node is left-left child of grandparent
     * @return  reference to where this nodes data was moved to
     */
    auto zigzig(stnode* node) -> stnode* {
        //std::cout << "zigzig\n";
        auto p  = node->m_parent;
        auto gp = node->m_parent->m_parent;

        std::swap(gp->m_left, gp->m_right);
        std::swap(p->m_left,  p->m_right);
        std::swap(gp->m_left, node->m_left);
        std::swap(p->m_left,  node->m_right);
        std::swap(node->m_left,     node->m_right);

        std::swap(node->m_data, gp->m_data);

        if (node->m_left)   { node->m_left->m_parent    = node; }
        if (node->m_right)  { node->m_right->m_parent   = node; }
        if (p->m_left)      { p->m_left->m_parent       = p; }
        if (gp->m_left)     { gp->m_left->m_parent      = gp; }

        return node->m_parent->m_parent;
    }


    /**
     * Performs two left rotations assuming this node is right-right child of grandparent
     * @return  reference to where this nodes data was moved to
     */
    auto zagzag(stnode* node) -> stnode* {
        //std::cout << "zagzag\n";
        auto p  = node->m_parent;
        auto gp = node->m_parent->m_parent;

        std::swap(gp->m_left,   gp->m_right);
        std::swap(p->m_left,    p->m_right);
        std::swap(gp->m_right,  node->m_right);
        std::swap(p->m_right,   node->m_left);
        std::swap(node->m_left, node->m_right);

        std::swap(node->m_data, gp->m_data);

        if (node->m_left)   { node->m_left->m_parent    = node; }
        if (node->m_right)  { node->m_right->m_parent   = node; }
        if (p->m_right)     { p->m_right->m_parent      = p; }
        if (gp->m_right)    { gp->m_right->m_parent     = gp; }

        return node->m_parent->m_parent;
    }

    /**
     * Performs a right then left rotation assuming this node is right-left child of grandparent
     * @return  reference to where this nodes data was moved to
     */
    auto zigzag(stnode* node) -> stnode* {
        //std::cout << "zigzag\n";
        auto p  = node->m_parent;
        auto gp = node->m_parent->m_parent;

        std::swap(node->m_left, node->m_right);
        std::swap(node->m_left, gp->m_left);
        std::swap(p->m_left,    gp->m_left);

        std::swap(node->m_data, gp->m_data);

        if (node->m_left)   { node->m_left->m_parent    = node; }
        if (p->m_left)      { p->m_left->m_parent       = p; }

        node->m_parent = gp;

        return node->m_parent;
    }


    /**
     * Performs a left then right rotation assuming this node is left-right child of grandparent
     * @return  reference to where this nodes data was moved to
     */
    auto zagzig(stnode* node) -> stnode* {
        //std::cout << "zagzig\n";
        auto p  = node->m_parent;
        auto gp = node->m_parent->m_parent;

        std::swap(node->m_left,     node->m_right);
        std::swap(node->m_right,    gp->m_right);
        std::swap(p->m_right,       gp->m_right);

        std::swap(node->m_data, gp->m_data);

        if (node->m_right)  { node->m_right->m_parent   = node; }
        if (p->m_right)     { p->m_right->m_parent      = p; }

        node->m_parent = gp;

        return node->m_parent;
    }

    auto splay(stnode* node, int distance, fullsplay) -> void {
        execute_splay(node, distance);
    }

    auto splay(stnode* node, int distance, semisplay) -> void {
        execute_splay(node, distance/2);
    }

    auto execute_splay(stnode* node, int distance) -> void {
        //std::cout << "splay on node " << node->m_data << "\n";
        auto current = node;
        auto p       = current->m_parent;

        while (p && distance>0) {
            auto gp = p->m_parent;
            if (gp) {
                if (gp->m_left == p) {
                    if (p->m_left == current) {
                        current = zigzig(current);
                        distance -= 2;
                    } else {
                        current = zagzig(current);
                        distance -= 2;
                    }
                } else {
                    if (p->m_left == current) {
                        current = zigzag(current);
                        distance -= 2;
                    } else {
                        current = zagzag(current);
                        distance -= 2;
                    }
                }
            } else {
                if (p->m_left == current) {
                    current = zig(current);
                    distance -= 1;
                } else {
                    current = zag(current);
                    distance -= 1;
                }
            }

            p = current->m_parent;
        }
    }

    auto splaylargest(const T& data) -> void {
        auto current    = m_root;
        auto height     = size_t{0};
        while (current->m_right) {
            current = current->m_right;
            height++;
        }
        splay(current, height, splay_type{});
    }


 public:

    ////////////////////////////////////////////////////////////////
    // ---------------------- CONSTRUCTORS ---------------------- //
    ////////////////////////////////////////////////////////////////

    splay_tree(): m_size(0), m_root(nullptr) {}

    ~splay_tree() {
        auto node_queue = ring_vector<stnode*>{m_size};
        if (!m_root) {
            return;
        }

        node_queue.push_back(m_root);

        while(!node_queue.empty()) {
            stnode *next = node_queue.pop_front_get();
            if (next->m_left) {
                node_queue.push_back(next->m_left);
            }
            if (next->m_right) {
                node_queue.push_back(next->m_right);
            }
            delete next;
        }
    }


    ////////////////////////////////////////////////////////////////
    // ----------------------- PROPERTIES ----------------------- //
    ////////////////////////////////////////////////////////////////

    auto root() -> std::optional<stnode*> {
        if (m_root) {
            return {m_root};
        } else {
            return {std::nullopt};
        }
    }

    auto size() const -> size_t { return m_size; }


    ////////////////////////////////////////////////////////////////
    // ----------------------- OPERATIONS ----------------------- //
    ////////////////////////////////////////////////////////////////

    template<typename U=T>
    auto insert(U&& data) -> void {
        size_t height=0;

        m_size++;

        if (!m_root) {
            m_root = new stnode(std::forward<U>(data));
            return;
        } else {
            auto current = &m_root;
            auto parent  =  m_root->m_parent;
            while (*current != nullptr) {
                height++;
                if (data < (*current)->m_data) {
                    parent  = *current;
                    current = &(*current)->m_left;
                } else {
                    parent  = *current;
                    current = &(*current)->m_right;
                }
            }
            *current = new stnode(std::forward<T>(data));
            (*current)->m_parent = parent;
            splay(*current, height, splay_type{});
        }
    }

    auto contains(const T& data) -> bool {
        auto current = m_root;
        auto height  = size_t{0};

        while (current != nullptr) {
            if (data == current->m_data) {
                splay(current, height, splay_type{});
                return true;
            } else if (data < current->m_data) {
                current = current->m_left;
            } else {
                current = current->m_right;
            }
            height++;
        }

        return false;
    }

};
