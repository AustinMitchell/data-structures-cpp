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
    template<typename U> friend class splay_tree;

    ////////////////////////////////////////////////////////////////
    // ------------------------- FIELDS ------------------------- //
    ////////////////////////////////////////////////////////////////
    T       data_;
    splay_tree_node<T> *parent_;
    splay_tree_node<T> *left_;
    splay_tree_node<T> *right_;


    ////////////////////////////////////////////////////////////////
    // -------------------- SPLAY OPERATIONS -------------------- //
    ////////////////////////////////////////////////////////////////

    /**
     * Rotates right assuming this node is the left child of parent
     * @return  reference to where this nodes data was moved to
     */
    auto zig() -> splay_tree_node<T>& {
        //std::cout << "zig\n";
        auto p  = parent_;

        std::swap(p->left_, p->right_);
        std::swap(left_,    right_);
        std::swap(right_,   p->left_);

        std::swap(data_, p->data_);

        if (right_)   { right_->parent_     = this; }
        if (p->left_) { p->left_->parent_   = p; }

        return *p;
    }


    /**
     * Rotates left assuming this node is the right child of parent
     * @return  reference to where this nodes data was moved to
     */
    auto zag() -> splay_tree_node<T>& {
        //std::cout << "zag\n";
        auto p  = parent_;

        std::swap(p->left_, p->right_);
        std::swap(left_,    right_);
        std::swap(left_,    p->right_);

        std::swap(data_, p->data_);

        if (left_)     { left_->parent_      = this; }
        if (p->right_) { p->right_->parent_  = p; }

        return *p;
    }


    /**
     * Performs two right rotations assuming this node is left-left child of grandparent
     * @return  reference to where this nodes data was moved to
     */
    auto zigzig() -> splay_tree_node<T>& {
        //std::cout << "zigzig\n";
        auto p  = parent_;
        auto gp = parent_->parent_;

        std::swap(gp->left_, gp->right_);
        std::swap(p->left_,  p->right_);
        std::swap(gp->left_, left_);
        std::swap(p->left_,  right_);
        std::swap(left_,     right_);

        std::swap(data_, gp->data_);

        if (left_)     { left_->parent_     = this; }
        if (right_)    { right_->parent_    = this; }
        if (p->left_)  { p->left_->parent_  = p; }
        if (gp->left_) { gp->left_->parent_ = gp; }

        return *gp;
    }


    /**
     * Performs two left rotations assuming this node is right-right child of grandparent
     * @return  reference to where this nodes data was moved to
     */
    auto zagzag() -> splay_tree_node<T>& {
        //std::cout << "zagzag\n";
        auto p  = parent_;
        auto gp = parent_->parent_;

        std::swap(gp->left_,  gp->right_);
        std::swap(p->left_,   p->right_);
        std::swap(gp->right_, right_);
        std::swap(p->right_,  left_);
        std::swap(left_,      right_);

        std::swap(data_, gp->data_);

        if (left_)      { left_->parent_      = this; }
        if (right_)     { right_->parent_     = this; }
        if (p->right_)  { p->right_->parent_  = p; }
        if (gp->right_) { gp->right_->parent_ = gp; }

        return *gp;
    }


    /**
     * Performs a right then left rotation assuming this node is right-left child of grandparent
     * @return  reference to where this nodes data was moved to
     */
    auto zigzag() -> splay_tree_node<T>& {
        //std::cout << "zigzag\n";
        auto p  = parent_;
        auto gp = parent_->parent_;

        std::swap(left_,    right_);
        std::swap(left_,    gp->left_);
        std::swap(p->left_, gp->left_);

        std::swap(data_, gp->data_);

        if (left_)    { left_->parent_      = this; }
        if (p->left_) { p->left_->parent_   = p; }
        parent_ = gp;

        return *parent_;
    }


    /**
     * Performs a left then right rotation assuming this node is left-right child of grandparent
     * @return  reference to where this nodes data was moved to
     */
    auto zagzig() -> splay_tree_node<T>& {
        //std::cout << "zagzig\n";
        auto p  = parent_;
        auto gp = parent_->parent_;

        std::swap(left_,     right_);
        std::swap(right_,    gp->right_);
        std::swap(p->right_, gp->right_);

        std::swap(data_, gp->data_);

        if (right_)    { right_->parent_      = this; }
        if (p->right_) { p->right_->parent_   = p; }
        parent_ = gp;

        return *parent_;
    }


    auto splay() -> void {
        //std::cout << "splay on node " << data_ << "\n";
        auto p  = parent_;
        if (p) {
            auto gp = parent_->parent_;
            if (gp) {
                if (gp->left_ == p) {
                    if (p->left_ == this) {
                        zigzig().splay();
                    } else {
                        zagzig().splay();
                    }
                } else {
                    if (p->left_ == this) {
                        zigzag().splay();
                    } else {
                        zagzag().splay();
                    }
                }
            } else {
                if (p->left_ == this) {
                    zig();
                } else {
                    zag();
                }
            }
        }
    }


    auto splaylargest() -> void {
        if (right_) {
            right_->largest();
        } else {
            splay();
        }
    }


 public:

    ////////////////////////////////////////////////////////////////
    // ----------------------- PROPERTIES ----------------------- //
    ////////////////////////////////////////////////////////////////

    auto data() -> T& { return  data_; }

    /** Returns the left child if there is a left child, otherwise returns nullopt */
    auto left() -> std::optional<splay_tree_node<T>*> {
        if (left_) {
            return {left_};
        } else {
            return {std::nullopt};
        }
    }

    /** Returns the right child if there is a right child, otherwise returns nullopt */
    auto right() -> std::optional<splay_tree_node<T>*> {
        if (right_) {
            return {right_};
        } else {
            return {std::nullopt};
        }
    }

    /** Returns the parent if there is a parent, otherwise returns nullopt */
    auto parent() -> std::optional<splay_tree_node<T>*> {
        if (parent_) {
            return {parent_};
        } else {
            return {std::nullopt};
        }
    }


    ////////////////////////////////////////////////////////////////
    // ---------------------- CONSTRUCTORS ---------------------- //
    ////////////////////////////////////////////////////////////////

    template<typename U>
    splay_tree_node(U&& data) : data_{std::forward<U>(data)}, parent_(nullptr), left_(nullptr), right_(nullptr) {}


    ////////////////////////////////////////////////////////////////
    // ----------------------- OPERATIONS ----------------------- //
    ////////////////////////////////////////////////////////////////

    template<typename U>
    auto insert(U&& data) -> void {
        splay_tree_node<T> **child = (data < data_) ? &left_
                                         : &right_;

        if (*child) {
            (*child)->insert(data);
        } else {
            *child = new splay_tree_node<T>(data);
            (*child)->parent_ = this;
            (*child)->splay();
        }
    }

    auto find(const T& data) -> bool {
        if (data_ == data) {
            splay();
            return true;
        }

        bool found = false;

        if (left_)            { found = left_->find(data); }
        if (!found && right_) { found = right_->find(data); }

        return found;
    }

    std::string to_string() {
        std::stringstream ss;
        ss << data_;

        if (left_ || right_) {
            ss << " {";
            if (left_) {
                ss << left_->to_string();
            } else {
                ss << "-";
            }
            ss << " ";
            if (right_) {
                ss << right_->to_string();
            } else {
                ss << "-";
            }
            ss << "}";
        }
        return ss.str();
    }

};