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


 public:
    auto data()   -> T&       { return  data_; }
    auto left()   -> Node<T>& { return *left_; }
    auto right()  -> Node<T>& { return *right_; }
    auto parent() -> Node<T>& { return *parent_; }

    template<typename U>
    Node(U&& data) : data_{std::forward<U>(data)} {}

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

    auto zig() -> Node<T>& {
        std::swap(data_, parent_->data_);
        parent_->left_.swap(parent_->right_);
        left_.swap(right_);
        right_.swap(parent_->left_);

        return *parent_
    }

    auto zag() -> Node<T>& {
        std::swap(data_, parent_->data_);
        parent_->left_.swap(parent_->right_);
        left_.swap(right_);
        left_.swap(parent_->right_);

        return *parent_
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