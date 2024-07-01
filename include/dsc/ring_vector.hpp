// Copyright 2020 Nathaniel Mitchell

#pragma once

#include <iterator>
#include <memory>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include <utility>

namespace dsc {

template<typename T, typename Allocator = std::allocator<T>>
class ring_vector {
    using ui32 = size_t;

    Allocator   alloc_;
    ui32        begin_,
                end_,
                size_,
                capacity_,
                capacity_bits_,
                idx_mask_;
    T*          array_;

    /** Resizes array to 1 << new_capacity_bits, copies elements over and deallocates previous array */
    auto resize(ui32 new_capacity_bits) -> void {
        if (new_capacity_bits < 2) { new_capacity_bits=2; }

        ui32 new_begin    = 0;
        ui32 new_end      = size_;
        ui32 new_capacity = 1 << new_capacity_bits;
        ui32 new_idx_mask = new_capacity - 1;

        T* new_array = std::allocator_traits<Allocator>::allocate(alloc_, new_capacity);
        if (size_ != 0) {
            if (begin_ < end_) {
                // All elements are contiguous and in order
                if constexpr (std::is_trivially_copyable_v<T>) {
                    std::memcpy(new_array, array_+begin_, size_*sizeof(T));
                } else {
                    // move construct all elements
                    for (ui32 idx = 0; idx < size_; idx++) {
                        std::allocator_traits<Allocator>::construct(alloc_, new_array+idx, std::move(array_[begin_+idx]));
                    }
                }
            } else {
                // All elements are not contiguous or in order
                if constexpr (std::is_trivially_copyable_v<T>) {
                    std::memcpy(new_array,                      array_ + begin_, sizeof(T) * (capacity_ - begin_));
                    std::memcpy(new_array + (capacity_-begin_), array_,          sizeof(T) * size_ - (capacity_ - begin_));
                } else {
                    // move construct all elements
                    for (ui32 idx = 0; idx < capacity_ - begin_; idx++) {
                        std::allocator_traits<Allocator>::construct(alloc_, new_array+idx, std::move(array_[begin_+idx]));
                    }
                    for (ui32 idx = (capacity_ - begin_); idx < size_; idx++) {
                        std::allocator_traits<Allocator>::construct(alloc_, new_array+idx, std::move(array_[idx - (capacity_ - begin_)]));
                    }
                }
            }
        }

        std::allocator_traits<Allocator>::deallocate(alloc_, array_, capacity_);
        array_         = new_array;
        begin_         = new_begin;
        end_           = new_end;
        capacity_      = new_capacity;
        capacity_bits_ = new_capacity_bits;
        idx_mask_      = new_idx_mask;
    }

    /** Calls destructor on all constructed elements of array, then deallocates array */
    auto destroy() -> void {
        // first destroy all constructed elements
        if (size_ != 0) {
            if (begin_ < end_) {
                // All elements are contiguous and in order
                for (ui32 idx=begin_; idx < begin_+size_; idx++) {
                    std::allocator_traits<Allocator>::destroy(alloc_, array_+idx);
                }
            } else {
                // All elements are not contiguous or in order
                for (ui32 idx=begin_; idx < capacity_; idx++) {
                    std::allocator_traits<Allocator>::destroy(alloc_, array_+idx);
                }
                for (ui32 idx=0; idx < end_; idx++) {
                    std::allocator_traits<Allocator>::destroy(alloc_, array_+idx);
                }
            }
        }

        if (capacity_ > 0) {
            std::allocator_traits<Allocator>::deallocate(alloc_, array_, capacity_);
        }
    }

 public:
    using value_type        = T;
    using allocator_type    = Allocator;
    using size_type         = ui32;
    using difference_type   = std::ptrdiff_t;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;

    /** Allocates an empty ring vector. Reserves 4 spaces by default. */
    ring_vector():  begin_(0),
                    end_(0),
                    size_(0),
                    capacity_(4),
                    capacity_bits_(2),
                    idx_mask_((1<<2)-1) {

        array_ = std::allocator_traits<Allocator>::allocate(alloc_, capacity_);
    }

    /** Allocates an empty ring vector with reserve_space reserved. */
    explicit ring_vector(ui32 reserve_space):  begin_(0), end_(0), size_(0) {
        capacity_bits_ = 2;
        while ((1<<capacity_bits_) < reserve_space) {
            capacity_bits_++;
        }
        capacity_ = 1 << capacity_bits_;
        idx_mask_ = capacity_ - 1;

        array_ = std::allocator_traits<Allocator>::allocate(alloc_, capacity_);
    }

    /** Copy constructor which creates a ring vector with all attributes equal to another vector
     *  and copies all elements over with T's copy constructor */
    ring_vector(ring_vector const& other): begin_(other.begin_),
                                        end_(other.end_),
                                        size_(other.size_),
                                        capacity_(other.capacity_),
                                        capacity_bits_(other.capacity_bits_),
                                        idx_mask_(other.idx_mask_) {

        array_ = std::allocator_traits<Allocator>::allocate(alloc_, capacity_);

        if (size_ != 0) {
            if (begin_ < end_) {
                // All elements are contiguous and in order
                for (ui32 idx=begin_; idx < begin_+size_; idx++) {
                    std::allocator_traits<Allocator>::construct(alloc_, array_+idx, other.array_[idx]);
                }
            } else {
                // All elements are not contiguous or in order
                for (ui32 idx=begin_; idx < capacity_; idx++) {
                    std::allocator_traits<Allocator>::construct(alloc_, array_+idx, other.array_[idx]);
                }
                for (ui32 idx=0; idx < end_; idx++) {
                    std::allocator_traits<Allocator>::construct(alloc_, array_+idx, other.array_[idx]);
                }
            }
        }
    }

    /** Move constructor which creates a ring vector with all attributes equal to another vector
     *  and empties other ring vector */
    ring_vector(ring_vector&& other): begin_(other.begin_),
                                    end_(other.end_),
                                    size_(other.size_),
                                    capacity_(other.capacity_),
                                    capacity_bits_(other.capacity_bits_),
                                    idx_mask_(other.idx_mask_) {

        array_ = other.array_;

        other.begin_           = 0;
        other.end_             = 0;
        other.size_            = 0;
        other.capacity_        = 0;
        other.capacity_bits_   = 0;
        other.idx_mask_        = 0;
        other.array_           = nullptr;
    }

    ~ring_vector() {
        destroy();
    }


    /* ========================================================== */
    /* ====================  ELEMENT ACCESS  ==================== */

    /** Returns reference to an element at the given position */
    auto at(ui32 pos)       -> reference       { return array_[(pos + begin_) & idx_mask_]; }
    /** Returns const reference to an element at the given position */
    auto at(ui32 pos) const -> const_reference { return array_[(pos + begin_) & idx_mask_]; }

    /** Returns reference to an element at the given position */
    auto operator[](ui32 pos)       -> reference       { return array_[(pos + begin_) & idx_mask_]; }
    /** Returns const reference to an element at the given position */
    auto operator[](ui32 pos) const -> const_reference { return array_[(pos + begin_) & idx_mask_]; }

    /** Returns reference to first element in vector */
    auto front() -> reference               { return array_[begin_]; }
    /** Returns const reference to first element in vector */
    auto front() const -> const_reference   { return array_[begin_]; }

    /** Returns reference to last element in vector */
    auto back() -> reference               { return array_[(begin_+size_) & idx_mask_]; }
    /** Returns const reference to first element in vector */
    auto back() const -> const_reference   { return array_[(begin_+size_) & idx_mask_]; }


    /* ========================================================== */
    /* =======================  ITERATORS  ====================== */

    class iterator {
     private:
        ring_vector<T>& m_array;
        ui32 m_idx;

     public:
        using difference_type   = ui32;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::forward_iterator_tag;

        iterator(ring_vector<T>& array, ui32 idx = 0) : m_array(array), m_idx(idx) {}

        auto operator++()    -> iterator& { m_idx++; return *this; }
        auto operator++(int) -> iterator  { iterator retval = *this; ++(*this); return retval; }
        auto operator+=(int offset) -> iterator  {
            m_idx = std::min(m_idx+offset, m_array.size());
            return *this;
        }
        auto operator+(int offset) -> iterator  {
            iterator retval = *this;
            return retval += offset;
        }

        auto operator--()    -> iterator& { m_idx--; return *this; }
        auto operator--(int) -> iterator  { iterator retval = *this; --(*this); return retval; }
        auto operator-=(int offset) -> iterator  {
            m_idx = std::max(static_cast<int_fast64_t>(m_idx)-offset, int_fast64_t{0});
            return *this;
        }
        auto operator-(int offset) -> iterator  {
            iterator retval = *this;
            return retval -= offset;
        }

        auto operator==(iterator other) const -> bool { return m_idx == other.m_idx; }
        auto operator!=(iterator other) const -> bool { return !(*this == other); }

        auto operator> (iterator other) const -> bool { return m_idx > other.m_idx; }
        auto operator>=(iterator other) const -> bool { return m_idx >= other.m_idx; }
        auto operator< (iterator other) const -> bool { return m_idx < other.m_idx; }
        auto operator<=(iterator other) const -> bool { return m_idx <= other.m_idx; }

        auto operator* () -> reference { return  m_array[m_idx]; }
        auto operator->() -> pointer   { return &m_array[m_idx]; }
    };

    class const_iterator {
     private:
        const ring_vector<T>& m_array;
        ui32 m_idx;

     public:
        using difference_type   = ui32;
        using value_type        = T;
        using pointer           = value_type const *;
        using reference         = const value_type&;
        using iterator_category = std::forward_iterator_tag;

        const_iterator(const ring_vector<T>& array, ui32 idx = 0) : m_array(array), m_idx(idx) {}

        auto operator++()    -> iterator& { m_idx++; return *this; }
        auto operator++(int) -> iterator  { iterator retval = *this; ++(*this); return retval; }

        auto operator==(iterator const& other) const -> bool { return m_idx == other.m_idx; }
        auto operator!=(iterator const& other) const -> bool { return !(*this == other); }

        auto operator* () -> reference { return  m_array[m_idx]; }
        auto operator->() -> pointer   { return &m_array[m_idx]; }
    };

    /** Returns forward iterator on this vector starting at front */
    auto begin() -> iterator { return {*this, 0}; }
    /** Returns end position of forward iterator */
    auto end()   -> iterator { return {*this, size_}; }

    /** Returns const forward iterator on this vector starting at front */
    auto begin() const -> const_iterator { return {*this, 0}; }
    /** Returns end position of const forward iterator */
    auto end()   const -> const_iterator { return {*this, size_}; }

    /** Returns const forward iterator on this vector starting at front */
    auto cbegin() const -> const_iterator { return {*this, 0}; }
    /** Returns end position of const forward iterator */
    auto cend()   const -> const_iterator { return {*this, size_}; }


    /* ========================================================== */
    /* ========================  CAPACITY  ====================== */

    /** Returns true if size of vector is 0 */
    auto empty()    const -> bool   { return size_==0; }

    /** Returns number of elements being used in array */
    auto size()     const -> ui32 { return size_; }

    /** Returns number of reserved array elements */
    auto capacity() const -> ui32 { return capacity_; }

    /** Causes the vector to reallocate space, rounded up to the neareast power of 2 and copy over currently existing elements.
     * Will do nothing if the new capacity is smaller than size. This operation could increase or reduce capacity.
     * */
    auto reserve(ui32 new_capacity) -> void {
        if (new_capacity <= size_) {
            return;
        }

        ui32 new_capacity_bits = 2;
        while ((1<<new_capacity_bits) < new_capacity) {
            new_capacity_bits++;
        }

        if (new_capacity_bits != capacity_bits_) {
            resize(new_capacity_bits);
        }
    }

    /** Attempts to return memory to the system by shrinking array capacity, to a minimum of 4. Equivalent to calling reserve(size) */
    auto shrink_to_fit() -> void {
        reserve(std::max<ui32>(size_, 4));
    }


    /* ========================================================== */
    /* ========================  MODIFIERS  ===================== */

    /** Remove all elements from the vector. */
    auto clear() -> void {
        if (size_ != 0) {
            if (begin_ < end_) {
                // All elements are contiguous and in order
                for (ui32 idx=begin_; idx < begin_+size_; idx++) {
                    std::allocator_traits<Allocator>::destroy(alloc_, array_+idx);
                }
            } else {
                // All elements are not contiguous or in order
                for (ui32 idx=begin_; idx < capacity_; idx++) {
                    std::allocator_traits<Allocator>::destroy(alloc_, array_+idx);
                }
                for (ui32 idx=0; idx < end_; idx++) {
                    std::allocator_traits<Allocator>::destroy(alloc_, array_+idx);
                }
            }
        }

        begin_ = 0;
        end_   = 0;
        size_  = 0;
    }

    /** Place one element at the end of the vector. May invoke resizing if capacity is reached. */
    auto push_back(T value) -> void {
        if (size_ >= capacity_) {
            resize(capacity_bits_+1);
        }

        std::allocator_traits<Allocator>::construct(alloc_, array_+end_, std::move(value));
        end_ = (end_+1) & idx_mask_;
        size_++;
    }

    template<typename... Args>
    auto emplace_back(Args&&... args) -> void {
        if (size_ >= capacity_) {
            resize(capacity_bits_+1);
        }

        std::allocator_traits<Allocator>::construct(alloc_, array_+end_, std::forward<Args>(args)...);
        end_ = (end_+1) & idx_mask_;
        size_++;
    }

    /** Place one element at the beginning of the vector. May invoke resizing if capacity is reached. */
    auto push_front(T value) -> void {
        if (size_ >= capacity_) {
            resize(capacity_bits_+1);
        }

        begin_ = (begin_-1) & idx_mask_;
        std::allocator_traits<Allocator>::construct(alloc_, array_+begin_, std::move(value));
        size_++;
    }

    template<typename... Args>
    auto emplace_front(Args&&... args) -> void {
        if (size_ >= capacity_) {
            resize(capacity_bits_+1);
        }

        begin_ = (begin_-1) & idx_mask_;
        std::allocator_traits<Allocator>::construct(alloc_, array_+begin_, std::forward<Args>(args)...);
        size_++;
    }

    /** Remove one element from the end of the vector */
    auto pop_back() -> void {
        end_ = (end_-1) & idx_mask_;
        size_--;
        std::allocator_traits<Allocator>::destroy(alloc_, array_+end_);
    }

    /** Remove one element from the end of the vector and return the element */
    auto pop_back_get() -> T {
        end_ = (end_-1) & idx_mask_;
        size_--;

        T temp = std::move(array_[end_]);
        std::allocator_traits<Allocator>::destroy(alloc_, array_+end_);
        return temp;
    }

    /** Remove one element from the beginning of the vector */
    auto pop_front() -> void {
        std::allocator_traits<Allocator>::destroy(alloc_, array_+begin_);
        begin_ = (begin_+1) & idx_mask_;
        size_--;
    }

    /** Remove one element from the begninning of the vector and return the element */
    auto pop_front_get() -> T {
        T temp = std::move(array_[begin_]);
        std::allocator_traits<Allocator>::destroy(alloc_, array_+begin_);

        begin_ = (begin_+1) & idx_mask_;
        size_--;
        return temp;
    }

    auto insert(ui32 pos, T value)  -> iterator {
        if (pos == size_) {
            push_back(std::move(value));
            return {*this, size_-1};
        } else if (pos == 0) {
            push_front(std::move(value));
            return {*this, 0};
        } else if (size_ >= capacity_) {
            resize(capacity_bits_+1);
        }

        ui32 arr_idx = 0;

        // Selects which direction to offset. This makes it so worst case you only ever move half the elements of the array.
        if (pos <= size_/2) {
            // Push everything at pos left

            begin_ = (begin_ - 1) & idx_mask_;
            arr_idx = (pos + begin_) & idx_mask_;

            if (begin_ <= arr_idx) {
                // All elements to be shifted contiguous and in order
                for (ui32 idx=begin_; idx < arr_idx; idx++) {
                    array_[idx] = std::move(array_[idx+1]);
                }
            } else {
                // All elements to be shifted are not contiguous or in order
                for (ui32 idx=begin_; idx < capacity_-1; idx++) {
                    array_[idx] = std::move(array_[idx+1]);
                }
                array_[capacity_-1] = std::move(array_[0]);
                for (ui32 idx=0; idx < arr_idx; idx++) {
                    array_[idx] = std::move(array_[idx+1]);
                }
            }

        } else {
            // Push everything at pos right

            arr_idx = (pos + begin_) & idx_mask_;

            if (end_ >= arr_idx) {
                // All elements to be shifted contiguous and in order
                for (ui32 idx=end_; idx > arr_idx; idx--) {
                    array_[idx] = std::move(array_[idx-1]);
                }
            } else {
                // All elements to be shifted are not contiguous or in order
                for (ui32 idx=end_; idx > 0; idx--) {
                    array_[idx] = std::move(array_[idx-1]);
                }
                array_[0] = std::move(array_[capacity_-1]);
                for (ui32 idx=capacity_-1; idx > arr_idx; idx--) {
                    array_[idx] = std::move(array_[idx-1]);
                }
            }

            end_ = (end_ + 1) & idx_mask_;
        }

        std::allocator_traits<Allocator>::construct(alloc_, array_+arr_idx, std::move(value));
        size_++;

        return {*this, pos};
    }


    /* ========================================================== */
    /* =======================  OPERATIONS  ===================== */

    auto operator=(const ring_vector& other) -> ring_vector& {

        destroy();

        begin_         = other.begin_;
        end_           = other.end_;
        size_          = other.size_;
        capacity_      = other.capacity_;
        capacity_bits_ = other.capacity_bits_;
        idx_mask_      = other.idx_mask_;

        array_ = std::allocator_traits<Allocator>::allocate(alloc_, capacity_);

        if (size_ != 0) {
            if (begin_ < end_) {
                // All elements are contiguous and in order
                for (ui32 idx=begin_; idx < begin_+size_; idx++) {
                    std::allocator_traits<Allocator>::construct(alloc_, array_+idx, other.array_[idx]);
                }
            } else {
                // All elements are not contiguous or in order
                for (ui32 idx=begin_; idx < capacity_; idx++) {
                    std::allocator_traits<Allocator>::construct(alloc_, array_+idx, other.array_[idx]);
                }
                for (ui32 idx=0; idx < end_; idx++) {
                    std::allocator_traits<Allocator>::construct(alloc_, array_+idx, other.array_[idx]);
                }
            }
        }

        return *this;
    }

    auto operator=(ring_vector&& other) -> ring_vector& {

        destroy();

        begin_         = other.begin_;
        end_           = other.end_;
        size_          = other.size_;
        capacity_      = other.capacity_;
        capacity_bits_ = other.capacity_bits_;
        idx_mask_      = other.idx_mask_;

        array_ = other.array_;

        other.begin_           = 0;
        other.end_             = 0;
        other.size_            = 0;
        other.capacity_        = 0;
        other.capacity_bits_   = 0;
        other.idx_mask_        = 0;
        other.array_           = nullptr;

        return *this;
    }

    auto swap(ring_vector& other) -> void {
        std::swap(array_,          other.array_);
        std::swap(begin_,          other.begin_);
        std::swap(end_,            other.end_);
        std::swap(size_,           other.size_);
        std::swap(capacity_,       other.capacity_);
        std::swap(capacity_bits_,  other.capacity_bits_);
        std::swap(idx_mask_,       other.idx_mask_);
    }
};

}  // namespace dsc
