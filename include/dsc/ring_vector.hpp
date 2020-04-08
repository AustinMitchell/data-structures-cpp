// Copyright 2020 Nathaniel Mitchell

#pragma once

#include <memory>
#include <cstddef>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace dsc {
    template<typename T, typename Allocator = std::allocator<T>>
    class ring_vector {
    private:
        using ui32 = uint_fast32_t;

        Allocator   m_alloc;
        ui32        m_begin,
                    m_end,
                    m_size,
                    m_capacity,
                    m_capacity_bits,
                    m_idx_mask;
        T*          m_array;

        /** Resizes array to 1 << new_capacity_bits, copies elements over and deallocates previous array */
        auto resize(ui32 new_capacity_bits) -> void {
            if (new_capacity_bits < 2) { new_capacity_bits=2; }

            ui32 new_begin    = 0;
            ui32 new_end      = m_size;
            ui32 new_capacity = 1 << new_capacity_bits;
            ui32 new_idx_mask = new_capacity - 1;

            T*     new_array = std::allocator_traits<Allocator>::allocate(m_alloc, new_capacity);
            if (m_size != 0) {
                if (m_begin < m_end) {
                    // All elements are contiguous and in order
                    std::memcpy(new_array,
                                m_array + m_begin,
                                sizeof(T) * (m_size));
                } else {
                    // All elements are not contiguous or in order
                    std::memcpy(new_array,
                                m_array + m_begin,
                                sizeof(T) * (m_capacity - m_begin));

                    std::memcpy(new_array + (m_capacity - m_begin),
                                m_array,
                                sizeof(T) * m_size - (m_capacity - m_begin));
                }
            }

            std::allocator_traits<Allocator>::deallocate(m_alloc, m_array, m_capacity);
            m_array         = new_array;
            m_begin         = new_begin;
            m_end           = new_end;
            m_capacity      = new_capacity;
            m_capacity_bits = new_capacity_bits;
            m_idx_mask      = new_idx_mask;
        }

        /** Calls destructor on all constructed elements of array, then deallocates array */
        auto destroy() -> void {
            // first destroy all constructed elements
            if (m_size != 0) {
                if (m_begin < m_end) {
                    // All elements are contiguous and in order
                    for (ui32 idx=m_begin; idx < m_begin+m_size; idx++) {
                        std::allocator_traits<Allocator>::destroy(m_alloc, m_array+idx);
                    }
                } else {
                    // All elements are not contiguous or in order
                    for (ui32 idx=m_begin; idx < m_capacity; idx++) {
                        std::allocator_traits<Allocator>::destroy(m_alloc, m_array+idx);
                    }
                    for (ui32 idx=0; idx < m_end; idx++) {
                        std::allocator_traits<Allocator>::destroy(m_alloc, m_array+idx);
                    }
                }
            }

            std::allocator_traits<Allocator>::deallocate(m_alloc, m_array, m_capacity);
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
        ring_vector():  m_begin(0),
                        m_end(0),
                        m_size(0),
                        m_capacity(4),
                        m_capacity_bits(2),
                        m_idx_mask((1<<2)-1) {

            m_array = std::allocator_traits<Allocator>::allocate(m_alloc, m_capacity);
        }

        /** Allocates an empty ring vector with reserve_space reserved. */
        ring_vector(ui32 reserve_space):  m_begin(0),
                                            m_end(0),
                                            m_size(0) {

            m_capacity_bits = 2;
            while((1<<m_capacity_bits) < reserve_space) {
                m_capacity_bits++;
            }
            m_capacity = 1 << m_capacity_bits;
            m_idx_mask = m_capacity - 1;

            m_array = std::allocator_traits<Allocator>::allocate(m_alloc, m_capacity);
        }

        /** Copy constructor which creates a ring vector with all attributes equal to another vector
         *  and copies all elements over with T's copy constructor */
        ring_vector(const ring_vector& other): m_begin(other.m_begin),
                                            m_end(other.m_end),
                                            m_size(other.m_size),
                                            m_capacity(other.m_capacity),
                                            m_capacity_bits(other.m_capacity_bits),
                                            m_idx_mask(other.m_idx_mask) {

            m_array = std::allocator_traits<Allocator>::allocate(m_alloc, m_capacity);

            if (m_size != 0) {
                if (m_begin < m_end) {
                    // All elements are contiguous and in order
                    for (ui32 idx=m_begin; idx < m_begin+m_size; idx++) {
                        std::allocator_traits<Allocator>::construct(m_alloc, m_array+idx, other.m_array[idx]);
                    }
                } else {
                    // All elements are not contiguous or in order
                    for (ui32 idx=m_begin; idx < m_capacity; idx++) {
                        std::allocator_traits<Allocator>::construct(m_alloc, m_array+idx, other.m_array[idx]);
                    }
                    for (ui32 idx=0; idx < m_end; idx++) {
                        std::allocator_traits<Allocator>::construct(m_alloc, m_array+idx, other.m_array[idx]);
                    }
                }
            }
        }

        /** Move constructor which creates a ring vector with all attributes equal to another vector
         *  and empties other ring vector */
        ring_vector(ring_vector&& other): m_begin(other.m_begin),
                                        m_end(other.m_end),
                                        m_size(other.m_size),
                                        m_capacity(other.m_capacity),
                                        m_capacity_bits(other.m_capacity_bits),
                                        m_idx_mask(other.m_idx_mask) {

            m_array = other.m_array;

            other.m_begin           = 0;
            other.m_end             = 0;
            other.m_size            = 0;
            other.m_capacity        = 1;
            other.m_capacity_bits   = 0;
            other.m_idx_mask        = 0;
            other.m_array           = nullptr;
        }

        ~ring_vector() {
            destroy();
        }


        /* ========================================================== */
        /* ====================  ELEMENT ACCESS  ==================== */

        /** Returns reference to an element at the given position */
        auto at(ui32 pos)       -> reference       { return m_array[(pos + m_begin) & m_idx_mask]; }
        /** Returns const reference to an element at the given position */
        auto at(ui32 pos) const -> const_reference { return m_array[(pos + m_begin) & m_idx_mask]; }

        /** Returns reference to an element at the given position */
        auto operator[](ui32 pos)       -> reference       { return m_array[(pos + m_begin) & m_idx_mask]; }
        /** Returns const reference to an element at the given position */
        auto operator[](ui32 pos) const -> const_reference { return m_array[(pos + m_begin) & m_idx_mask]; }

        /** Returns reference to first element in vector */
        auto front() -> reference               { return m_array[m_begin]; }
        /** Returns const reference to first element in vector */
        auto front() const -> const_reference   { return m_array[m_begin]; }

        /** Returns reference to last element in vector */
        auto back() -> reference               { return m_array[(m_begin+m_size) & m_idx_mask]; }
        /** Returns const reference to first element in vector */
        auto back() const -> const_reference   { return m_array[(m_begin+m_size) & m_idx_mask]; }


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
                m_idx += offset;
                m_idx = m_array.size() ^ ((m_idx ^ m_array.size()) & -(m_idx < m_array.size())); // min(x, y)
                return *this;
            }
            auto operator+(int offset) -> iterator  {
                iterator retval = *this;
                return retval += offset;
            }

            auto operator--()    -> iterator& { m_idx--; return *this; }
            auto operator--(int) -> iterator  { iterator retval = *this; --(*this); return retval; }
            auto operator-=(int offset) -> iterator  {
                int_fast64_t result = m_idx;
                result -= offset;
                m_idx = result ^ (result & -(result < 0)); // max(x, y)
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
        auto end()   -> iterator { return {*this, m_size}; }

        /** Returns const forward iterator on this vector starting at front */
        auto begin() const -> const_iterator { return {*this, 0}; }
        /** Returns end position of const forward iterator */
        auto end()   const -> const_iterator { return {*this, m_size}; }

        /** Returns const forward iterator on this vector starting at front */
        auto cbegin() const -> const_iterator { return {*this, 0}; }
        /** Returns end position of const forward iterator */
        auto cend()   const -> const_iterator { return {*this, m_size}; }


        /* ========================================================== */
        /* ========================  CAPACITY  ====================== */

        /** Returns true if size of vector is 0 */
        auto empty()    const -> bool   { return m_size==0; }

        /** Returns number of elements being used in array */
        auto size()     const -> ui32 { return m_size; }

        /** Returns number of reserved array elements */
        auto capacity() const -> ui32 { return m_capacity; }

        /** Causes the vector to reallocate space, rounded up to the neareast power of 2 and copy over currently existing elements.
         * Will do nothing if the new capacity is smaller than size. This operation could increase or reduce capacity.
         * */
        auto reserve(ui32 capacity) -> void {
            if (capacity < m_size) {
                return;
            }
            ui32 new_capacity_bits = 2;
            while((1<<new_capacity_bits) < capacity) {
                new_capacity_bits++;
            }
            if (new_capacity_bits != m_capacity_bits) {
                resize(new_capacity_bits);
            }
        }

        /** Attempts to return memory to the system by shrinking array capacity, to a minimum of 4. Equivalent to calling reserve(size) */
        auto shrink_to_fit() -> void {
            reserve(std::max<ui32>(m_size, 4));
        }


        /* ========================================================== */
        /* ========================  MODIFIERS  ===================== */

        /** Remove all elements from the vector. */
        auto clear() -> void {
            if (m_size != 0) {
                if (m_begin < m_end) {
                    // All elements are contiguous and in order
                    for (ui32 idx=m_begin; idx < m_begin+m_size; idx++) {
                        std::allocator_traits<Allocator>::destroy(m_alloc, m_array+idx);
                    }
                } else {
                    // All elements are not contiguous or in order
                    for (ui32 idx=m_begin; idx < m_capacity; idx++) {
                        std::allocator_traits<Allocator>::destroy(m_alloc, m_array+idx);
                    }
                    for (ui32 idx=0; idx < m_end; idx++) {
                        std::allocator_traits<Allocator>::destroy(m_alloc, m_array+idx);
                    }
                }
            }

            m_begin = 0;
            m_end   = 0;
            m_size  = 0;
        }

        /** Place one element at the end of the vector. May invoke resizing if capacity is reached. */
        template<typename U=T>
        auto push_back(U&& value) -> void {
            if (m_size >= m_capacity-1) {
                resize(m_capacity_bits+1);
            }

            std::allocator_traits<Allocator>::construct(m_alloc, m_array+m_end, std::forward<U>(value));
            m_end = (m_end+1) & m_idx_mask;
            m_size++;
        }

        template<typename... Args>
        auto emplace_back(Args&&... args) -> void {
            if (m_size >= m_capacity-1) {
                resize(m_capacity_bits+1);
            }

            std::allocator_traits<Allocator>::construct(m_alloc, m_array+m_end, std::forward<Args>(args)...);
            m_end = (m_end+1) & m_idx_mask;
            m_size++;
        }

        /** Place one element at the beginning of the vector. May invoke resizing if capacity is reached. */
        template<typename U=T>
        auto push_front(U&& value) -> void {
            if (m_size >= m_capacity-1) {
                resize(m_capacity_bits+1);
            }

            m_begin = (m_begin-1) & m_idx_mask;
            std::allocator_traits<Allocator>::construct(m_alloc, m_array+m_begin, std::forward<U>(value));
            m_size++;
        }

        template<typename... Args>
        auto emplace_front(Args&&... args) -> void {
            if (m_size >= m_capacity-1) {
                resize(m_capacity_bits+1);
            }

            m_begin = (m_begin-1) & m_idx_mask;
            std::allocator_traits<Allocator>::construct(m_alloc, m_array+m_begin, std::forward<Args>(args)...);
            m_size++;
        }

        /** Remove one element from the end of the vector */
        auto pop_back() -> void {
            m_end = (m_end-1) & m_idx_mask;
            m_size--;
            std::allocator_traits<Allocator>::destroy(m_alloc, m_array+m_end);
        }

        /** Remove one element from the end of the vector and return the element */
        auto pop_back_get() -> T {
            m_end = (m_end-1) & m_idx_mask;
            m_size--;

            T temp = std::move(m_array[m_end]);
            std::allocator_traits<Allocator>::destroy(m_alloc, m_array+m_end);
            return temp;
        }

        /** Remove one element from the beginning of the vector */
        auto pop_front() -> void {
            std::allocator_traits<Allocator>::destroy(m_alloc, m_array+m_begin);
            m_begin = (m_begin+1) & m_idx_mask;
            m_size--;
        }

        /** Remove one element from the begninning of the vector and return the element */
        auto pop_front_get() -> T {
            T temp = std::move(m_array[m_begin]);
            std::allocator_traits<Allocator>::destroy(m_alloc, m_array+m_begin);

            m_begin = (m_begin+1) & m_idx_mask;
            m_size--;
            return temp;
        }

        template<typename U=T>
        auto insert(ui32 pos, U&& value)  -> iterator {
            if (m_size == 0) {
                push_back(std::forward<U>(value));
                return begin();
            } else if (m_size >= m_capacity-1) {
                resize(m_capacity_bits+1);
            }

            ui32 arr_idx = 0;

            // Selects which direction to offset. This makes it so worst case you only ever move half the elements of the array.
            if (pos <= m_size/2) {
                // Push everything at pos left

                m_begin = (m_begin - 1) & m_idx_mask;
                arr_idx = (pos + m_begin) & m_idx_mask;

                if (m_begin <= arr_idx) {
                    // All elements to be shifted contiguous and in order
                    for (ui32 idx=m_begin; idx < arr_idx; idx++) {
                        m_array[idx] = std::move(m_array[idx+1]);
                    }
                } else {
                    // All elements to be shifted are not contiguous or in order
                    for (ui32 idx=m_begin; idx < m_capacity-1; idx++) {
                        m_array[idx] = std::move(m_array[idx+1]);
                    }
                    m_array[m_capacity-1] = std::move(m_array[0]);
                    for (ui32 idx=0; idx < arr_idx; idx++) {
                        m_array[idx] = std::move(m_array[idx+1]);
                    }
                }

            } else {
                // Push everything at pos right

                arr_idx = (pos + m_begin) & m_idx_mask;

                if (m_end >= arr_idx) {
                    // All elements to be shifted contiguous and in order
                    for (ui32 idx=m_end; idx > arr_idx; idx--) {
                        m_array[idx] = std::move(m_array[idx-1]);
                    }
                } else {
                    // All elements to be shifted are not contiguous or in order
                    for (ui32 idx=m_end; idx > 0; idx--) {
                        m_array[idx] = std::move(m_array[idx-1]);
                    }
                    m_array[0] = std::move(m_array[m_capacity-1]);
                    for (ui32 idx=m_capacity-1; idx > arr_idx; idx--) {
                        m_array[idx] = std::move(m_array[idx-1]);
                    }
                }

                m_end = (m_end + 1) & m_idx_mask;
            }

            std::allocator_traits<Allocator>::construct(m_alloc, m_array+arr_idx, std::forward<U>(value));
            m_size++;

            return {*this, pos};
        }


        /* ========================================================== */
        /* =======================  OPERATIONS  ===================== */

        auto operator=(const ring_vector& other) -> ring_vector& {

            destroy();

            m_begin         = other.m_begin;
            m_end           = other.m_end;
            m_size          = other.m_size;
            m_capacity      = other.m_capacity;
            m_capacity_bits = other.m_capacity_bits;
            m_idx_mask      = other.m_idx_mask;

            m_array = std::allocator_traits<Allocator>::allocate(m_alloc, m_capacity);

            if (m_size != 0) {
                if (m_begin < m_end) {
                    // All elements are contiguous and in order
                    for (ui32 idx=m_begin; idx < m_begin+m_size; idx++) {
                        std::allocator_traits<Allocator>::construct(m_alloc, m_array+idx, other.m_array[idx]);
                    }
                } else {
                    // All elements are not contiguous or in order
                    for (ui32 idx=m_begin; idx < m_capacity; idx++) {
                        std::allocator_traits<Allocator>::construct(m_alloc, m_array+idx, other.m_array[idx]);
                    }
                    for (ui32 idx=0; idx < m_end; idx++) {
                        std::allocator_traits<Allocator>::construct(m_alloc, m_array+idx, other.m_array[idx]);
                    }
                }
            }

            return *this;
        }

        auto operator=(ring_vector&& other) -> ring_vector& {

            destroy();

            m_begin         = other.m_begin;
            m_end           = other.m_end;
            m_size          = other.m_size;
            m_capacity      = other.m_capacity;
            m_capacity_bits = other.m_capacity_bits;
            m_idx_mask      = other.m_idx_mask;

            m_array = other.m_array;

            other.m_begin           = 0;
            other.m_end             = 0;
            other.m_size            = 0;
            other.m_capacity        = 1;
            other.m_capacity_bits   = 0;
            other.m_idx_mask        = 0;
            other.m_array           = nullptr;

            return *this;
        }

        auto swap(ring_vector& other) -> void {
            std::swap(m_array,          other.m_array);
            std::swap(m_begin,          other.m_begin);
            std::swap(m_end,            other.m_end);
            std::swap(m_size,           other.m_size);
            std::swap(m_capacity,       other.m_capacity);
            std::swap(m_capacity_bits,  other.m_capacity_bits);
            std::swap(m_idx_mask,       other.m_idx_mask);
        }
    };
}