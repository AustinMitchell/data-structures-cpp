// Copyright 2020 Nathaniel Mitchell

#include <memory>
#include <cstddef>
#include <cstring>
#include <vector>
#include <algorithm>


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
        ring_vector<T>& m_array;
        size_t m_idx;

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

        auto operator* () -> reference { return  m_array[m_idx]; }
        auto operator->() -> pointer   { return &m_array[m_idx]; }
    };

    class const_iterator {
     private:
        const ring_vector<T>& m_array;
        size_t m_idx;

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

        auto operator* () -> reference { return  m_array[m_idx]; }
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

    ring_vector(): m_begin(0), m_end(0), m_size(0), m_capacity(4), m_capacity_bits(2), m_idx_mask((1<<2)-1) {
        m_array = std::allocator_traits<Allocator>::allocate(m_alloc, m_capacity);
    }

    ring_vector(size_t reserve_space): m_begin(0), m_end(0), m_size(0) {
        m_capacity_bits = 2;
        while((1<<m_capacity_bits) < reserve_space) {
            m_capacity_bits++;
        }
        m_capacity = 1 << m_capacity_bits;
        m_idx_mask = m_capacity - 1;

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

    /** Returns reference to an element at the given position */
    auto at(size_type pos)       -> reference       { return m_array[(pos + m_begin) & m_idx_mask]; }
    /** Returns const reference to an element at the given position */
    auto at(size_type pos) const -> const_reference { return m_array[(pos + m_begin) & m_idx_mask]; }

    /** Returns reference to an element at the given position */
    auto operator[](size_type pos)       -> reference       { return m_array[(pos + m_begin) & m_idx_mask]; }
    /** Returns const reference to an element at the given position */
    auto operator[](size_type pos) const -> const_reference { return m_array[(pos + m_begin) & m_idx_mask]; }

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
    auto size()     const -> size_t { return m_size; }

    /** Returns number of reserved array elements */
    auto capacity() const -> size_t { return m_capacity; }

    /** Causes the vector to reallocate space, rounded up to the neareast power of 2 and copy over currently existing elements.
     * Will do nothing if the new capacity is smaller than size. This operation could increase or reduce capacity.
     * */
    auto reserve(size_t capacity) -> void {
        if (capacity < m_size) {
            return;
        }
        size_t new_capacity_bits = 2;
        while((1<<new_capacity_bits) < capacity) {
            new_capacity_bits++;
        }
        if (new_capacity_bits != m_capacity_bits) {
            resize(new_capacity_bits);
        }
    }

    /** Attempts to return memory to the system by shrinking array capacity, to a minimum of 4. Equivalent to calling reserve(size) */
    auto shrink_to_fit() -> void {
        reserve(std::max<size_t>(m_size, 4));
    }


    /* ========================================================== */
    /* ========================  MODIFIERS  ===================== */

    /** Remove all elements from the vector. */
    auto clear() -> void {
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

        m_begin = 0;
        m_end   = 0;
        m_size  = 0;
    }

    /** Place one element at the end of the vector. May invoke resizing if capacity is reached. */
    template<typename U=T>
    auto push_back(U&& value)  -> void {
        if (m_size == m_capacity) {
            resize(m_capacity_bits+1);
        }

        std::allocator_traits<Allocator>::construct(m_alloc, m_array+m_end, std::forward<U>(value));
        m_end = (m_end+1) & m_idx_mask;
        m_size++;
    }

    /** Place one element at the beginning of the vector. May invoke resizing if capacity is reached. */
    template<typename U=T>
    auto push_front(U&& value) -> void {
        if (m_size == m_capacity) {
            resize(m_capacity_bits+1);
        }

        m_begin = (m_begin-1) & m_idx_mask;
        std::allocator_traits<Allocator>::construct(m_alloc, m_array+m_begin, std::forward<U>(value));
        m_size++;
    }

    /** Remove one element from the end of the vector */
    auto pop_back()  -> void {
        m_end = (m_end-1) & m_idx_mask;
        m_size--;
        std::allocator_traits<Allocator>::destroy(m_alloc, m_array+m_end);
    }

    /** Remove one element from the end of the vector and return the element */
    auto pop_back_get()  -> T {
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
    auto insert(size_t pos, U&& value)  -> iterator {
        if (m_size == 0) {
            push_back(std::forward<U>(value));
            return begin();
        } else if (m_size == m_capacity) {
            resize(m_capacity_bits+1);
        }

        size_t arr_idx = 0;

        // Selects which direction to offset. This makes it so worst case you only ever move half the elements of the array.
        if (pos <= m_size/2) {
            // Push everything at pos left

            m_begin = (m_begin - 1) & m_idx_mask;
            arr_idx = (pos + m_begin) & m_idx_mask;

            if (m_begin <= arr_idx) {
                // All elements to be shifted contiguous and in order
                for (size_t idx=m_begin; idx < arr_idx; idx++) {
                    m_array[idx] = std::move(m_array[idx+1]);
                }
            } else {
                // All elements to be shifted are not contiguous or in order
                for (size_t idx=m_begin; idx < m_capacity-1; idx++) {
                    m_array[idx] = std::move(m_array[idx+1]);
                }
                m_array[m_capacity-1] = std::move(m_array[0]);
                for (size_t idx=0; idx < arr_idx; idx++) {
                    m_array[idx] = std::move(m_array[idx+1]);
                }
            }

        } else {
            // Push everything at pos right

            arr_idx = (pos + m_begin) & m_idx_mask;

            if (m_end >= arr_idx) {
                // All elements to be shifted contiguous and in order
                for (size_t idx=m_end; idx > arr_idx; idx--) {
                    m_array[idx] = std::move(m_array[idx-1]);
                }
            } else {
                // All elements to be shifted are not contiguous or in order
                for (size_t idx=m_end; idx > 0; idx--) {
                    m_array[idx] = std::move(m_array[idx-1]);
                }
                m_array[0] = std::move(m_array[m_capacity-1]);
                for (size_t idx=m_capacity-1; idx > arr_idx; idx--) {
                    m_array[idx] = std::move(m_array[idx-1]);
                }
            }

            m_end = (m_end + 1) & m_idx_mask;
        }

        std::allocator_traits<Allocator>::construct(m_alloc, m_array+arr_idx, std::forward<U>(value));
        m_size++;

        return {*this, pos};
    }
};