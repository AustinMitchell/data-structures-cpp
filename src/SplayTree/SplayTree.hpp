#include <iostream>
#include <memory>
#include <utility>
#include <sstream>

template<typename T>
class Node {
 private:
    T                        data_;
    Node<T>                 *parent_;
    std::unique_ptr<Node<T>> left_, right_;

    /**
     * Rotates right assuming this node is the left child of parent
     * @return  reference to where this node was moved to
     */
    auto zig() -> Node<T>& {
        auto p  = parent_;

        std::swap(p->left, p->right_);
        std::swap(left_,   right_);
        std::swap(right_,  p->left);

        std::swap(data_, p->data);

        return *p
    }

    /**
     * Rotates left assuming this node is the right child of parent
     * @return  reference to where this node was moved to
     */
    auto zag() -> Node<T>& {
        auto p  = parent_;

        std::swap(p->left, p->right_);
        std::swap(left_,   right_);
        std::swap(left_,   p->right_)

        std::swap(data_, p->data);

        return *p
    }

    /**
     * Performs two right rotations assuming this node is left-left child of grandparent
     * @return  reference to where this node was moved to
     */
    auto zigzig() -> Node<T>& {
        auto p  = parent_;
        auto gp = parent_->parent_;

        std::swap(gp->left_, gp->right_);
        std::swap(p->left_,  p->right_);
        std::swap(gp->left_, left_);
        std::swap(p->left_,  right_);
        std::swap(left_,     right_);

        std::swap(data_, gp->data);

        return *gp
    }

    /**
     * Performs two left rotations assuming this node is right-right child of grandparent
     * @return  reference to where this node was moved to
     */
    auto zagzag() -> Node<T>& {
        auto p  = parent_;
        auto gp = parent_->parent_;

        std::swap(gp->left_,  gp->right_);
        std::swap(p->left_,   p->right_);
        std::swap(gp->right_, right_);
        std::swap(p->right_,  left_);
        std::swap(left_,      right_);

        std::swap(data_, gp->data);

        return *gp
    }

    /**
     * Performs a right then left rotation assuming this node is right-left child of grandparent
     * @return  reference to where this node was moved to
     */
    auto zigzag() -> Node<T>& {
        auto& p  = parent_;
        auto& gp = parent_->parent_;

        std::swap(left_,    right_);
        std::swap(left_,    gp->left_);
        std::swap(p->left_, gp->left_);

        std::swap(data_, gp->data);

        gp->right_->parent_ = gp;
        p = gp;

        return *p
    }

    /**
     * Performs a left then right rotation assuming this node is left-right child of grandparent
     * @return  reference to where this node was moved to
     */
    auto zagzig() -> Node<T>& {
        auto p  = parent_;
        auto gp = parent_->parent_;

        std::swap(left_,     right_);
        std::swap(right_,    gp->right_);
        std::swap(p->right_, gp->right_);

        std::swap(data_, gp->data);

        gp->left_->parent_ = gp;
        p = gp;

        return *p
    }

    auto splay() -> void {
        while (parent) {

        }
    }

 public:
    auto data()   -> T&       { return  data_; }
    auto left()   -> Node<T>& { return *left_; }
    auto right()  -> Node<T>& { return *right_; }
    auto parent() -> Node<T>& { return *parent_; }

    template<typename U>
    Node(U&& data) : data_{std::forward<U>(data)}, parent_(nullptr) {}

    template<typename U>
    auto insert(U&& data) -> void {
        std::unique_ptr<Node<T>> *child = (data <= data_) ? &left_
                                                          : &right_;

        if (*child) {
            (*child)->insert(data);
        } else {
            *child = std::make_unique<Node<T>>(data);
        }
    }

    auto to_string() -> std::string {
        std::stringstream ss;
        ss << data_;

        if (left_ || right_) {
            ss << " {";
            if (left_) {
                ss << left_->to_string();
            } else {
                ss << " ";
            }
            ss << " ";
            if (right_) {
                ss << right_->to_string();
            } else {
                ss << " ";
            }
            ss << "}";
        }
        return ss.str();
    }
};

template<typename T>
class SplayTree {
 private:
    std::unique_ptr<Node<T>> root;

 public:
    template<typename U>
    auto insert(U&& data) -> void {
        
    }
};