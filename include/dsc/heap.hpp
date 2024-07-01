// Copyright 2024 Nathaniel Mitchell

#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <compare>
#include <print>

namespace dsc {

struct min_heap {};
struct max_heap {};

template <typename T, typename HeapType, typename Allocator = std::allocator<T>>
requires std::three_way_comparable<T> &&
         std::disjunction_v<std::is_same<HeapType, min_heap>,
                            std::is_same<HeapType, max_heap>>
class heap {
public:

    using value_type        = T;
    using allocator_type    = Allocator;
    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;

private:
    using idx_t = size_type;
    using allc_tr = std::allocator_traits<Allocator>;

    T* elems_;
    idx_t size_;
    idx_t capacity_;
    idx_t min_capacity_;
    Allocator alloc_;

    /* Determines higher priority element given the heap type */
    auto is_better(T& a, T& b) const {
        if constexpr(std::is_same_v<HeapType, min_heap>) {
            return a < b;
        } else {
            return a > b;
        }
    }

    auto parent(idx_t child) -> idx_t {
        return (child-1) / 2;
    }

    auto children(idx_t parent) -> std::tuple<idx_t, idx_t>{
        return std::make_tuple<idx_t, idx_t>(parent*2 + 1, parent*2 + 2);
    }

    static auto mem_size(idx_t count) { return sizeof(T)*count; }

    static auto move_array(Allocator& src_alloc, T* src, T* dest, idx_t count) -> void {
        if constexpr(std::is_trivially_copyable_v<T>) {
            std::memcpy(dest, src, mem_size(count));
        } else {
            for (idx_t idx=0; idx < count; idx++) {
                allc_tr::construct(src_alloc, dest+idx, std::move(src[idx]));
            }
        }
    }

    static auto copy_array(Allocator& src_alloc, T* src, T* dest, idx_t count) -> void {
        if constexpr(std::is_trivially_copyable_v<T>) {
            std::memcpy(dest, src, mem_size(count));
        } else {
            for (idx_t idx=0; idx < count; idx++) {
                allc_tr::construct(src_alloc, dest+idx, src[idx]);
            }
        }
    }


    auto resize(idx_t new_capacity) -> void {
        new_capacity = std::max(new_capacity, min_capacity_);

        if (new_capacity == capacity_) {
            return;
        }

        auto old_capacity = capacity_;
        capacity_ = new_capacity;
        T* new_array = allc_tr::allocate(alloc_, capacity_);

        move_array(alloc_, elems_, new_array, size_);

        allc_tr::deallocate(alloc_, elems_, old_capacity);

        elems_ = new_array;
    }

public:
    /** Construct an empty heap with a minimum memory capacity **/
    heap(idx_t min_capacity = 16): size_(0),
                                   capacity_(min_capacity),
                                   min_capacity_(min_capacity) {
        elems_ = allc_tr::allocate(alloc_, capacity_);
    }

    heap(heap<T, HeapType>& other): size_(other.size_),
                                    capacity_(other.capacity_),
                                    min_capacity_(other.min_capacity_) {
        elems_ = allc_tr::allocate(alloc_, capacity_);
        copy_array(alloc_, other.elems_, elems_, size_);
    }

    heap(heap<T, HeapType>&& other): size_(other.size_),
                                     capacity_(other.capacity_),
                                     min_capacity_(other.min_capacity_) {
        elems_ = allc_tr::allocate(alloc_, capacity_);
        move_array(alloc_, other.elems_, elems_, size_);
        other.size_ = 0;
    }

    virtual ~heap() {
        allc_tr::deallocate(alloc_, elems_, capacity_);
    }


    /* ========================================================== */
    /* ====================  ELEMENT ACCESS  ==================== */

    /* Return pointer to backing array for heap */
    auto data()       -> pointer       { return elems_; }
    /* Return const pointer to first element in heap */
    auto data() const -> const_pointer { return elems_; }

    /* Return reference to first element in heap */
    auto front()       -> reference { return elems_[0]; }
    /* Return const reference to first element in heap */
    auto front() const -> const_reference { return elems_[0]; }

    /** Returns reference to an element at the given position */
    auto at(idx_t pos)       -> reference       { return elems_[pos]; }
    /** Returns const reference to an element at the given position */
    auto at(idx_t pos) const -> const_reference { return elems_[pos]; }

    /** Returns reference to an element at the given position */
    auto operator[](idx_t pos)       -> reference       { return elems_[pos]; }
    /** Returns const reference to an element at the given position */
    auto operator[](idx_t pos) const -> const_reference { return elems_[pos]; }


     /* ========================================================== */
    /* =======================  ITERATORS  ====================== */
    class const_iterator;

    class iterator {
     private:
        T* data_;
        idx_t idx_;

     public:
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(heap<T, HeapType>& heap, idx_t idx = 0) : data_(heap.data()), idx_(idx) {}

        auto operator++()    -> iterator& { idx_++; return *this; }
        auto operator++(int) -> iterator  { iterator retval = *this; ++(*this); return retval; }


        auto operator--()    -> iterator& { idx_--; return *this; }
        auto operator--(int) -> iterator  { iterator retval = *this; --(*this); return retval; }


        auto operator<=>(iterator const& other) const = default;

        auto operator* () -> reference { return  data_[idx_]; }
        auto operator->() -> pointer   { return &data_[idx_]; }
    };

    class const_iterator {
     private:
        const T* const data_;
        idx_t idx_;

     public:
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = const value_type * const;
        using reference         = const value_type&;
        using iterator_category = std::bidirectional_iterator_tag;

        const_iterator(const heap<T, HeapType>& heap, idx_t idx = 0) : data_(heap.data()), idx_(idx) {}

        auto operator++()    -> const_iterator& { idx_++; return *this; }
        auto operator++(int) -> const_iterator  { const_iterator retval = *this; ++(*this); return retval; }

        auto operator<=>(iterator const& other) const = default;

        auto operator* () -> reference { return  data_[idx_]; }
        auto operator->() -> pointer   { return &data_[idx_]; }
    };

    /** Returns bacwards iterator on this vector starting at front */
    auto begin() -> iterator { return {*this, 0}; }
    /** Returns end position of bacwards iterator */
    auto end()   -> iterator { return {*this, size_}; }

    /** Returns const bacwards iterator on this vector starting at front */
    auto begin() const -> const_iterator { return {*this, 0}; }
    /** Returns end position of const bacwards iterator */
    auto end()   const -> const_iterator { return {*this, size_}; }

    /** Returns const bacwards iterator on this vector starting at front */
    auto cbegin() const -> const_iterator { return {*this, 0}; }
    /** Returns end position of const bacwards iterator */
    auto cend()   const -> const_iterator { return {*this, size_}; }

    /** Returns bacwards iterator on this vector starting at front */
    auto rbegin() -> iterator { return {*this, size_-1}; }
    /** Returns end position of bacwards iterator */
    auto rend()   -> iterator { return {*this, -1}; }

    /** Returns const bacwards iterator on this vector starting at front */
    auto rbegin() const -> const_iterator { return {*this, size_-1}; }
    /** Returns end position of const bacwards iterator */
    auto rend()   const -> const_iterator { return {*this, -1}; }

    /** Returns const bacwards iterator on this vector starting at front */
    auto rcbegin() const -> const_iterator { return {*this, size_-1}; }
    /** Returns end position of const bacwards iterator */
    auto rcend()   const -> const_iterator { return {*this, -1}; }


    /* ========================================================== */
    /* ========================  CAPACITY  ====================== */

     /** Returns true if size of vector is 0 */
    auto empty()    const -> bool  { return size_==0; }

    /** Returns number of elements being used in array */
    auto size()     const -> idx_t { return size_; }

    /** Returns number of reserved array elements */
    auto capacity() const -> idx_t { return capacity_; }

    /** Causes the heap to reallocate space and copy over currently existing elements.
     * Will do nothing if the new capacity is smaller than size. This operation could increase or reduce capacity.
     */
    auto reserve(idx_t new_capacity) -> void {
        if (new_capacity <= size_) {
            return;
        }

        resize(new_capacity);
    }

    /** Attempts to return memory to the system by shrinking array capacity, to a minimum. Equivalent to calling reserve(size) */
    auto shrink_to_fit() -> void {
        reserve(std::max<idx_t>(size_, min_capacity_));
    }


    /* ========================================================== */
    /* ========================  MODIFIERS  ===================== */

    auto pop() -> T {
        auto popped = std::move(elems_[0]);
        elems_[0] = std::move(elems_[size_-1]);
        size_--;

        // Take the last element, move it to the front, and bubble it back to the bottom, pulling up the next best element
        idx_t current_idx = 0;
        while(true) {
            auto [left_idx, right_idx] = children(current_idx);
            reference current = elems_[current_idx];
            if (right_idx >= size_) {
                if (left_idx >= size_) {
                    // no children, we're done.
                    break;
                } else {
                    // Only a left child
                    reference left = elems_[left_idx];
                    if (is_better(left, current)) {
                        std::swap(left, current);
                        current_idx = left_idx;
                    } else {
                        // No more better elements
                        break;
                    }
                }
            } else {
                // Has two children
                reference left  = elems_[left_idx];
                reference right = elems_[right_idx];
                idx_t next_idx;
                reference next  = is_better(left, right) ? (next_idx=left_idx, left)
                                                                : (next_idx=right_idx, right);
                if (is_better(next, current)) {
                    std::swap(next, current);
                    current_idx = next_idx;
                } else {
                    // No more better elements
                    break;
                }
            }
        }

        return popped;
    }

    template <typename... Params>
    auto push(Params &&... params) -> void {
        if (capacity_ <= size_) {
            resize(capacity_*2);
        }

        allc_tr::construct(alloc_, elems_+size_, std::forward<Params&&>(params)...);

        for (idx_t current_idx = size_, parent_idx = parent(current_idx);
             current_idx > 0 && is_better(elems_[current_idx], elems_[parent_idx]);
             current_idx = parent(current_idx), parent_idx = parent(current_idx)) {
            // bubble element to the top as long as it's better
            std::swap(elems_[current_idx], elems_[parent_idx]);
        }

        size_++;
    }

    /** Remove all elements from the vector. */
    auto clear() -> void {
        if constexpr(!std::is_trivially_destructible_v<T>) {
            for (idx_t idx=0; idx < size_; idx++) {
                allc_tr::destroy(alloc_, elems_+idx);
            }
        }

        size_  = 0;
    }


    /* ========================================================== */
    /* =======================  OPERATIONS  ===================== */

};

}