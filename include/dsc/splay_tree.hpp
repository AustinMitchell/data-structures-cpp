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
#include "ring_vector.hpp"

using std::size_t;

namespace dsc {
    // Splay tree tags. Defines how the splay function will operate
    struct fullsplay {};
    struct semisplay {};

    template<typename T, typename splay_type, typename Allocator>
    class splay_tree;

    // Offering type name option for semisplay tree
    template<typename T, typename Allocator=std::allocator<splay_tree_node<T>>>
    using semisplay_tree = splay_tree<T, semisplay, Allocator>;

    template<typename T, typename splay_type=fullsplay, typename Allocator = std::allocator<splay_tree_node<T>>>
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

        size_t       m_size;
        stnode      *m_root;
        Allocator    m_alloc;


        ////////////////////////////////////////////////////////////////
        // -------------------- SPLAY OPERATIONS -------------------- //
        ////////////////////////////////////////////////////////////////

        /**
         * Rotates right assuming this node is the left child of parent
         *      y          x
         *     / \        / \
         *    x   C  ->  A   y
         *   / \            / \
         *  A   B          B   C
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
         *      x          y
         *     / \        / \
         *    y   C  <-  A   x
         *   / \            / \
         *  A   B          B   C
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
         *       z          x
         *      / \        / \
         *     y   D      A   y
         *    / \            / \
         *   x   C    ->    B   z
         *  / \                / \
         * A   B              C   D
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
         *       x          z
         *      / \        / \
         *     y   D      A   y
         *    / \            / \
         *   z   C    <-    B   x
         *  / \                / \
         * A   B              C   D
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
         *   z                 x
         *  / \               / \
         * A   y             /   \
         *    / \   ->      z     y
         *   x   D         / \   / \
         *  / \           A   B C   D
         * B   C
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
         *     z                 x
         *    / \               / \
         *   y   D             /   \
         *  / \       ->      y     z
         * A   x             / \   / \
         *    / \           A   B C   D
         *   B   C
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

        auto splay(stnode* node, int distance) -> void {
            //std::cout << "splay on node " << node->m_data << "\n";
            auto current = node;
            auto p       = current->m_parent;

            if constexpr(std::is_same<splay_type, semisplay>::value) {
                // Semisplay variant: If access path is odd, begin with a zig/zag
                if (distance%2 == 1) {
                    if (p->m_left == current) {
                        current = zig(current);
                    } else {
                        current = zag(current);
                    }
                    p = current->m_parent;
                }
            }

            while (p) {
                auto gp = p->m_parent;

                if constexpr(std::is_same<splay_type, fullsplay>::value) {
                    if (gp) {
                        if (gp->m_left == p) {
                            if (p->m_left == current) {
                                current = zigzig(current);
                            } else {
                                current = zagzig(current);
                            }
                        } else {
                            if (p->m_left == current) {
                                current = zigzag(current);
                            } else {
                                current = zagzag(current);
                            }
                        }
                    } else {
                        if (p->m_left == current) {
                            current = zig(current);
                        } else  {
                            current = zag(current);
                        }
                    }

                } else {
                    // Since access path is guaranteed even, don't check parent validity
                    if (gp->m_left == p) {
                        if (p->m_left == current) {
                            // Semisplay variant: Perform one rotation on parent for zigzig case
                            current = zig(p);
                        } else {
                            current = zagzig(current);
                        }
                    } else {
                        if (p->m_left == current) {
                            current = zigzag(current);
                        } else {
                            // Semisplay variant: Perform one rotation on parent for zagzag case
                            current = zag(p);
                        }
                    }
                }


                p = current->m_parent;
            }
        }

        /** Constructs a balanced binary tree recursively from a vector */
        auto make_tree_from_vec(stnode* & node, stnode* parent, std::vector<T> const& sorted, int64_t lower, int64_t higher) {
            auto range = higher-lower;
            if (range < 0) {
                return;
            }

            auto mid = lower + range/2;
            node = std::allocator_traits<Allocator>::allocate(m_alloc, 1);
            std::allocator_traits<Allocator>::construct(m_alloc, node, sorted[mid], parent);

            make_tree_from_vec(node->m_left,  node, sorted, lower, mid-1);
            make_tree_from_vec(node->m_right, node, sorted, mid+1, higher);
        }

        auto destroy() -> void {
            auto node_queue = ring_vector<stnode*>{m_size/2 + 2};
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

                std::allocator_traits<Allocator>::destroy(m_alloc, next);
                std::allocator_traits<Allocator>::deallocate(m_alloc, next, 1);
            }
        }

    public:

        using value_type        = T;
        using allocator_type    = Allocator;
        using size_type         = size_t;
        using difference_type   = std::ptrdiff_t;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using pointer           = value_type*;
        using const_pointer     = const value_type*;

        ////////////////////////////////////////////////////////////////
        // ---------------------- CONSTRUCTORS ---------------------- //
        ////////////////////////////////////////////////////////////////

        splay_tree(): m_size(0), m_root(nullptr) {}

        /**
         * Constructs a balanced tree from a sorted vector.
        */
        splay_tree(std::vector<T> const& sorted): m_size(sorted.size()), m_root(nullptr), m_alloc() {
            make_tree_from_vec(m_root, nullptr, sorted, 0, m_size-1);
        }

        ~splay_tree() {
            // Most nodes that need to be stored in a breadth first search is half of the total nodes
            destroy();
        }


        ////////////////////////////////////////////////////////////////
        // ----------------------- PROPERTIES ----------------------- //
        ////////////////////////////////////////////////////////////////

        auto root() -> stnode const* {
            return m_root;
        }

        auto size() const -> size_t { return m_size; }

        auto empty() const -> bool  { return m_root == nullptr; }

        auto max_no_splay() const -> stnode const& {
            auto current    = m_root;
            while (current->m_right) {
                current = current->m_right;
            }
            return *current;
        }

        auto min_no_splay() const -> stnode const&  {
            auto current    = m_root;
            while (current->m_left) {
                current = current->m_left;
            }
            return *current;
        }

        auto height() const -> int  {
            if (!m_root) {
                return 0;
            }

            auto current_layer  = ring_vector<stnode const*>{};
            auto next_layer     = ring_vector<stnode const*>{};
            auto height         = 0;

            current_layer.push_back(m_root);

            while (!current_layer.empty()) {
                height++;
                // Check all nodes on this layer first
                while (!current_layer.empty()) {
                    auto current = current_layer.pop_front_get();
                    if (current->left()) {
                        next_layer.push_back(current->left());
                    }
                    if (current->right()) {
                        next_layer.push_back(current->right());
                    }
                }
                current_layer = std::move(next_layer);
                next_layer = ring_vector<stnode const*>{current_layer.size()*2}; // reserve maximum possible spaces
            }
            return height;
        }


        ////////////////////////////////////////////////////////////////
        // ----------------------- OPERATIONS ----------------------- //
        ////////////////////////////////////////////////////////////////

        template<typename U=T>
        auto insert(U&& data) -> void {
            size_t height=0;

            m_size++;

            if (!m_root) {
                m_root = std::allocator_traits<Allocator>::allocate(m_alloc, 1);
                std::allocator_traits<Allocator>::construct(m_alloc, m_root, std::forward<U>(data));
                return;
            } else {
                auto current = &m_root;
                auto parent  =  m_root->m_parent;
                while (*current != nullptr) {
                    height++;
                    if (data < (*current)->data()) {
                        parent  = *current;
                        current = &(*current)->m_left;
                    } else {
                        parent  = *current;
                        current = &(*current)->m_right;
                    }
                }

                *current = std::allocator_traits<Allocator>::allocate(m_alloc, 1);
                std::allocator_traits<Allocator>::construct(m_alloc, *current, std::forward<U>(data), parent);

                (*current)->m_parent = parent;
                splay(*current, height);
            }
        }

        auto contains(const T& data) -> bool {
            auto current = m_root;
            auto height  = size_t{0};

            while (current != nullptr) {
                if (data == current->data()) {
                    splay(current, height);
                    return true;
                } else if (data < current->data()) {
                    current = current->m_left;
                } else {
                    current = current->m_right;
                }
                height++;
            }

            return false;
        }

        auto max() -> stnode const& {
            auto current    = m_root;
            auto height     = size_t{0};
            while (current->m_right) {
                current = current->m_right;
                if constexpr(std::is_same<splay_type, semisplay>::value) {
                    height++;
                }
            }
            splay(current, height);
            return *current;
        }

        auto min() -> stnode const&  {
            auto current    = m_root;
            auto height     = size_t{0};
            while (current->m_left) {
                current = current->m_left;
                if constexpr(std::is_same<splay_type, semisplay>::value) {
                    height++;
                }
            }
            splay(current, height);
            return *current;
        }

        auto delete_min_no_splay() -> T {
            auto current    = m_root;
            while (current->m_left) {
                current = current->m_left;
            }

            auto ret = std::move(current->m_data);

            if (current == m_root) {
                m_root = m_root->m_right;
            } else {
                current->m_parent->m_left   = current->m_right;
                if (current->m_right) {
                    current->m_right->m_parent  = current->m_parent;
                }
            }

            std::allocator_traits<Allocator>::destroy(m_alloc, current);
            std::allocator_traits<Allocator>::deallocate(m_alloc, current, 1);

            return ret;
        }

        template<typename other_splay_type>
        auto operator=(splay_tree<T, other_splay_type>&& other) -> splay_tree<T, splay_type>& {

            destroy();

            m_root       = other.m_root;
            m_size       = other.m_size;
            other.m_root = nullptr;
            other.m_size = 0;

            return *this;
        }

        ////////////////////////////////////////////////////////////////
        // ------------------------ ITERATORS ----------------------- //
        ////////////////////////////////////////////////////////////////

        /** Iterator which performs an in-order traversal with an O(1) memory footprint. Since tree nodes are immutable
         * due to maintaining a sorted order property, only a const iterator is provided. */
        class iterator {
        private:
            stnode const* m_current;

            auto next() -> void {
                if (!m_current) { return; }

                if (m_current->right()) {
                    // If we have a right child after visiting this node, move to the smallest value of that subtree
                    m_current = m_current->right();
                    while(m_current->left()) {
                        m_current = m_current->left();
                    }
                } else {
                    // Otherwise, keep going up the tree until we reach the first parent to whom current is an element
                    // of its left subtree
                    auto prev {m_current};
                    m_current = m_current->parent();

                    while (m_current && (prev == m_current->right())) {
                        prev      = m_current;
                        m_current = m_current->parent();
                    }
                }
            }

        public:
            using difference_type   = size_t;
            using value_type        = T;
            using pointer           = value_type *;
            using reference         = value_type &;
            using const_pointer     = value_type const*;
            using const_reference   = value_type const&;
            using iterator_category = std::forward_iterator_tag;

            iterator(splay_tree<T, splay_type, Allocator> const& tree, size_t idx = 0) : m_current(&tree.min_no_splay()) {
                for (size_t i=0; i<idx; i++) {
                    next();
                }
            }

            iterator(stnode const* node) : m_current(node) {}

            auto operator++()    -> iterator& { next(); return *this; }
            auto operator++(int) -> iterator  { iterator retval = *this; ++(*this); return retval; }

            auto operator==(iterator const& other) const -> bool { return m_current == other.m_current; }
            auto operator!=(iterator const& other) const -> bool { return !(*this == other); }

            auto operator* () -> const_reference { return  m_current->data(); }
            auto operator->() -> const_pointer   { return &m_current->data(); }
        };

        /** Returns const forward iterator on this vector starting at front */
        auto begin() const -> iterator { return {*this}; }
        /** Returns end position of const forward iterator */
        auto end()   const -> iterator { return {nullptr}; }

        /** Returns const forward iterator on this vector starting at front */
        auto cbegin() const -> iterator { return {*this}; }
        /** Returns end position of const forward iterator */
        auto cend()   const -> iterator { return {nullptr}; }

    };
}