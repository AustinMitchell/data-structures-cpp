// Copyright 2020 Nathaniel Mitchell

#include <memory>
#include <cstddef>
#include <cstring>
#include <vector>

using std::size_t;

template<typename T, typename Allocator = std::allocator<T>>
class ring_vector {
 private:
    Allocator   m_alloc;
    size_t      m_begin,
                m_end,
                m_size,
                m_capacity,
                m_capacity_bits,
                m_capacity_bits_min,
                m_idx_mask;
    T*          m_array;


    auto resize(size_t new_capacity_bits) -> void {
        size_t new_begin    = 0;
        size_t new_end      = m_size;
        size_t new_capacity = 1 << new_capacity_bits;
        size_t new_idx_mask = new_capacity - 1;

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

 public:
    class iterator {
     private:
        size_t m_idx;
        ring_vector<T>& m_array;

     public:
        using difference_type   = size_t;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::forward_iterator_tag;

        iterator(ring_vector<T>& array, size_t idx = 0) : m_array(array), m_idx(idx) {}

        auto operator++()    -> iterator& { m_idx++; return *this; }
        auto operator++(int) -> iterator  { iterator retval = *this; ++(*this); return retval; }

        auto operator==(iterator other) const -> bool { return m_idx == other.m_idx; }
        auto operator!=(iterator other) const -> bool { return !(*this == other); }

        auto operator* () -> reference { return m_array[m_idx]; }
        auto operator->() -> pointer   { return &m_array[m_idx]; }
    };

    class const_iterator {
     private:
        size_t m_idx;
        const ring_vector<T>& m_array;

     public:
        using difference_type   = size_t;
        using value_type        = T;
        using pointer           = value_type const *;
        using reference         = const value_type&;
        using iterator_category = std::forward_iterator_tag;

        const_iterator(const ring_vector<T>& array, size_t idx = 0) : m_array(array), m_idx(idx) {}

        auto operator++()    -> iterator& { m_idx++; return *this; }
        auto operator++(int) -> iterator  { iterator retval = *this; ++(*this); return retval; }

        auto operator==(iterator other) const -> bool { return m_idx == other.m_idx; }
        auto operator!=(iterator other) const -> bool { return !(*this == other); }

        auto operator* () -> reference { return m_array[m_idx]; }
        auto operator->() -> pointer   { return &m_array[m_idx]; }
    };

    using value_type        = T;
    using allocator_type    = Allocator;
    using size_type         = size_t;
    using difference_type   = std::ptrdiff_t;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;

    ring_vector(): m_begin(0), m_end(0), m_size(0), m_capacity(4), m_capacity_bits(2), m_capacity_bits_min(2), m_idx_mask((1<<2)-1) {
        m_array = std::allocator_traits<Allocator>::allocate(m_alloc, m_capacity);
    }

    ~ring_vector() {
        // first destroy all constructed elements
        if (m_size != 0) {
            if (m_begin < m_end) {
                // All elements are contiguous and in order
                for (size_t idx=m_begin; idx < m_begin+m_size; idx++) {
                    std::allocator_traits<Allocator>::destroy(m_alloc, m_array+idx);
                }
            } else {
                // All elements are not contiguous or in order
                for (size_t idx=m_begin; idx < m_capacity; idx++) {
                    std::allocator_traits<Allocator>::destroy(m_alloc, m_array+idx);
                }
                for (size_t idx=0; idx < m_end; idx++) {
                    std::allocator_traits<Allocator>::destroy(m_alloc, m_array+idx);
                }
            }
        }

        std::allocator_traits<Allocator>::deallocate(m_alloc, m_array, m_capacity);
    }

    ////////////////////////////////////////////////////////////////
    // -------------------- MEMBER FUNCTIONS -------------------- //
    ////////////////////////////////////////////////////////////////

    /* ========================================================== */
    /* ====================  ELEMENT ACCESS  ==================== */

    auto at(size_type pos)       -> reference       { return m_array[(pos + m_begin) & m_idx_mask]; }
    auto at(size_type pos) const -> const_reference { return m_array[(pos + m_begin) & m_idx_mask]; }

    auto operator[](size_type pos)       -> reference       { return m_array[(pos + m_begin) & m_idx_mask]; }
    auto operator[](size_type pos) const -> const_reference { return m_array[(pos + m_begin) & m_idx_mask]; }

    auto front() -> reference               { return m_array[m_begin]; }
    auto front() const -> const_reference   { return m_array[m_begin]; }

    auto back() -> reference               { return m_array[(m_begin+m_size) & m_idx_mask]; }
    auto back() const -> const_reference   { return m_array[(m_begin+m_size) & m_idx_mask]; }


    /* ========================================================== */
    /* =======================  ITERATORS  ====================== */

    auto begin() -> iterator { return {*this, 0}; }
    auto end()   -> iterator { return {*this, m_size}; }

    auto begin() const -> const_iterator { return {*this, 0}; }
    auto end()   const -> const_iterator { return {*this, m_size}; }

    auto cbegin() const -> const_iterator { return {*this, 0}; }
    auto cend()   const -> const_iterator { return {*this, m_size}; }


    /* ========================================================== */
    /* ========================  CAPACITY  ====================== */

    auto empty() const -> bool { return m_size==0; }

    auto size() const -> size_t { return m_size; }

    auto capacity() const -> size_t { return m_capacity; }

    auto reserve(size_t capacity) -> void {
        size_t new_capacity_bits_min = 2;
        while((1<<new_capacity_bits_min) < capacity) {
            new_capacity_bits_min++;
        }
        resize(new_capacity_bits_min);
        m_capacity_bits_min = new_capacity_bits_min;
        return;
    }

    auto shrink_to_fit() -> void {
        return;
    }


    /* ========================================================== */
    /* ========================  MODIFIERS  ===================== */

    auto clear() -> void {
        return;
    }

    auto push_back(T value)  -> void {
        if (m_size == m_capacity) {
            resize(m_capacity_bits+1);
        }
        std::allocator_traits<Allocator>::construct(m_alloc, m_array+m_end, value);
        m_end = (m_end+1) & m_idx_mask;
        m_size++;
        return;
    }
    auto push_front(T value) -> void {
        if (m_size == m_capacity) {
            resize(m_capacity_bits+1);
        }
        m_begin = (m_begin-1) & m_idx_mask;
        std::allocator_traits<Allocator>::construct(m_alloc, m_array+m_begin, value);
        m_size++;
        return;
    }

    auto pop_back()  -> void {
        m_end = (m_end-1) & m_idx_mask;
        m_size--;
        std::allocator_traits<Allocator>::destroy(m_alloc, m_array+m_end);
    }
    auto pop_front() -> void {
        std::allocator_traits<Allocator>::destroy(m_alloc, m_array+m_begin);
        m_begin = (m_begin+1) & m_idx_mask;
        m_size--;
    }
};