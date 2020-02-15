// Copyright 2020 Nathaniel Mitchell

#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <sstream>
#include <optional>

#include "Node.hpp"


template<typename T>
class SplayTree {
 private:

    ////////////////////////////////////////////////////////////////
    // ------------------------- FIELDS ------------------------- //
    ////////////////////////////////////////////////////////////////

    std::unique_ptr<Node<T>> root_;

 public:

    ////////////////////////////////////////////////////////////////
    // ----------------------- PROPERTIES ----------------------- //
    ////////////////////////////////////////////////////////////////

    auto root() -> std::optional<Node<T>*> {
        if (root_) {
            return {root_.get()};
        } else {
            return {std::nullopt};
        }
    }


    ////////////////////////////////////////////////////////////////
    // ----------------------- OPERATIONS ----------------------- //
    ////////////////////////////////////////////////////////////////

    auto insert(T data) -> void {
        if (root_) {
            root_->insert(std::move(data));
        } else {
            root_ = std::make_unique<Node<T>>(std::move(data));
        }
    }

    std::string to_string() {
        using std::to_string;
        if (root_) {
            return root_->to_string();
        } else {
            return "";
        }
    }
};
