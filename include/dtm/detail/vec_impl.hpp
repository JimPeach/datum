// details/vec_impl.hpp
//

#ifndef INCLUDING_DATUM_DETAIL_VEC_IMPL_HPP
#error "Don't include or compile datum/detail/vec_impl.hpp directly." 
#endif

namespace dtm {

template <typename T>
T* vec<T>::allocate(size_t size) {
    T* ptr = reinterpret_cast<T*>(malloc(sizeof(T) * size));
    if (!ptr)
        throw std::bad_alloc();
    return ptr;
}

template <typename T>
void vec<T>::release() {
    if (!m_local_storage) {
        free(m_begin);
    }
    else {
        m_local_storage = false;
    }
}

template <typename T>
vec<T>::vec()
    : m_begin(nullptr), m_end(nullptr), m_capacity(0), m_local_storage(true)
{}

template <typename T>
vec<T>::vec(T* local_store, size_t local_store_capacity)
    : m_begin(local_store), m_end(local_store), m_capacity(local_store_capacity), m_local_storage(true)
{}

template <typename T>
vec<T>::vec(const vec<T>& v) 
    : vec()
{
    assign(v);
}

template <typename T>
vec<T>::vec(T* local_store, size_t local_store_capacity, const vec<T>& v) 
    : vec(local_store, local_store_capacity)
{
    assign(v);
}

template <typename T>
vec<T>::vec(vec<T>&& v)
    : vec()
{
    assign(std::move(v));
}

template <typename T>
vec<T>::vec(T* local_store, size_t local_store_capacity, vec<T>&& v)
    : vec(local_store, local_store_capacity)
{
    assign(std::move(v));
}

template <typename T>
template <typename... Args>
vec<T>::vec(size_t count, Args&&... args)
    : vec()
{
    fill(count, std::forward<Args>(args)...);
}

template <typename T>
template <typename... Args>
vec<T>::vec(T* local_store, size_t local_store_capacity, size_t count, Args&&... args)
    : vec(local_store, local_store_capacity)
{
    fill(count, std::forward<Args>(args)...);
}

template <typename T>
vec<T>::vec(std::initializer_list<T> init)
    : vec()
{
    assign(init);
}

template <typename T>
vec<T>::vec(T* local_store, size_t local_store_capacity, std::initializer_list<T> init)
    : vec(local_store, local_store_capacity)
{
    assign(init);
}

template <typename T>
template <typename It, typename>
vec<T>::vec(It begin, It end) 
    : vec()
{
    assign(begin, end);
}

template <typename T>
template <typename It, typename>
vec<T>::vec(T* local_store, size_t local_store_capacity, It begin, It end)
    : vec(local_store, local_store_capacity)
{
    assign(begin, end);
}

template <typename T>
vec<T>::~vec()
{
    release();
}

// Iterators

template <typename T>
typename vec<T>::iterator vec<T>::begin() noexcept {
    return iterator(m_begin);
}

template <typename T>
typename vec<T>::iterator vec<T>::end() noexcept {
    return iterator(m_end);
}

template <typename T>
typename vec<T>::const_iterator vec<T>::begin() const noexcept {
    return const_iterator(m_begin);
}

template <typename T>
typename vec<T>::const_iterator vec<T>::end() const noexcept {
    return const_iterator(m_end);
}

template <typename T>
typename vec<T>::const_iterator vec<T>::cbegin() const noexcept {
    return const_iterator(m_begin);
}

template <typename T>
typename vec<T>::const_iterator vec<T>::cend() const noexcept {
    return const_iterator(m_end);
}

template <typename T>
typename vec<T>::reverse_iterator vec<T>::rbegin() noexcept {
    return std::make_reverse_iterator(end());
}

template <typename T>
typename vec<T>::reverse_iterator vec<T>::rend() noexcept {
    return std::make_reverse_iterator(begin());
}

template <typename T>
typename vec<T>::const_reverse_iterator vec<T>::rbegin() const noexcept {
    return std::make_reverse_iterator(end());
}

template <typename T>
typename vec<T>::const_reverse_iterator vec<T>::rend() const noexcept {
    return std::make_reverse_iterator(begin());
}

template <typename T>
typename vec<T>::const_reverse_iterator vec<T>::crbegin() const noexcept {
    return std::make_reverse_iterator(end());
}

template <typename T>
typename vec<T>::const_reverse_iterator vec<T>::crend() const noexcept {
    return std::make_reverse_iterator(begin());
}

template <typename T>
vec<T>& vec<T>::operator= (const vec& rhs)
{
    assign(rhs);
    return *this;
}

template <typename T>
vec<T>& vec<T>::operator= (vec&& rhs)
{
    assign(std::move(rhs));
    return *this;
}

template <typename T>
vec<T>& vec<T>::operator= (std::initializer_list<T> init)
{
    assign(init);
    return *this;
}

template <typename T>
T& vec<T>::operator[] (size_t index) noexcept
{
    return m_begin[index];
}

template <typename T>
const T& vec<T>::operator[] (size_t index) const noexcept
{
    return m_begin[index];
}

template <typename T>
T& vec<T>::at(size_t index)
{
    if (index >= size())
        throw std::out_of_range();

    return m_begin[index];
}

template <typename T>
const T& vec<T>::at(size_t index) const
{
    if (index >= size())
        throw std::out_of_range();

    return m_begin[index];
}

template <typename T>
void vec<T>::swap(vec& rhs) noexcept
{
    vec<T> temp(std::move(rhs));
    rhs = std::move(*this);
    *this = std::move(temp);
}

template <typename T>
T& vec<T>::front() noexcept
{
    return *m_begin;
}

template <typename T>
T& vec<T>::back() noexcept
{
    return *(m_end - 1);
}

template <typename T>
size_t vec<T>::size() const noexcept
{
    return m_end - m_begin;
}

template <typename T>
bool vec<T>::empty() const noexcept
{
    return m_end == m_begin;
}

template <typename T>
size_t vec<T>::capacity() const noexcept
{
    return m_capacity;
}

template <typename T>
void vec<T>::clear()
{
    for (T* ptr = m_begin; ptr != m_end; ++ptr)
        ptr->~T();
    m_end = m_begin;
}

template <typename T>
void vec<T>::reserve(size_t new_capacity)
{
    if (new_capacity <= m_capacity)
        return;

    reserve_internal(new_capacity, is_relocatable_t<T>());
}

template <typename T> // XXX not exception safe
void vec<T>::reserve_internal(size_t new_capacity, std::true_type)
{   // Relocatable
    size_t old_size = size();
    if (old_size > 0) {
        T* new_block = reinterpret_cast<T*>(realloc(m_begin, sizeof(T) * new_capacity));
        if (!new_block)
            throw std::bad_alloc();
        m_begin = new_block;
    }
    else {
        release();
        m_begin = allocate(new_capacity);
    }
    m_end = m_begin + old_size;
    m_capacity = new_capacity;
}

template <typename T> // XXX not exception safe
void vec<T>::reserve_internal(size_t new_capacity, std::false_type)
{   // Not relocatable
    T* new_begin = allocate(new_capacity);
    T* new_end = new_begin;

    for (T* ptr = m_begin; ptr != m_end; ++ptr, ++new_end) {
        new (new_end) T(std::move(*ptr));
        ptr->~T();
    }
    release();
    m_begin = new_begin;
    m_end = new_end;
    m_capacity = new_capacity;
}

template <typename T>
void vec<T>::shrink_to_fit()
{
    reserve_internal(size(), is_relocatable_t<T>());
}

template <typename T>
template <typename... Args>
void vec<T>::resize(size_t new_size, Args&&... args)
{
    if (new_size > size()) {
        reserve(new_size);
        T* old_end = m_end;
        m_end = m_begin + new_size;
        for (T* ptr = old_end; ptr != m_end; ++ptr)
            new(ptr) T(std::forward<Args>(args)...);
    }
    else {
        T* old_end = m_end;
        m_end = m_begin + new_size;
        for (T* ptr = m_end; ptr != old_end; ++ptr)
            ptr->~T();        
    }
}

template <typename T>
void vec<T>::assign(const vec<T>& rhs)
{
    size_t rhs_size = rhs.m_end - rhs.m_begin;
    clear();
    reserve(rhs_size);
    for (T* rhs_ptr = rhs.m_begin; rhs_ptr != rhs.m_end; ++rhs_ptr)
        new (m_end++) T(*rhs_ptr);
}

template <typename T>
void vec<T>::assign(vec<T>&& rhs)
{
    size_t rhs_size = rhs.m_end - rhs.m_begin;
    clear();
    if (rhs.m_local_storage) {
        // If RHS has local storage we can't pointer steal. We'll need to do an
        // elementwise move.
        reserve(rhs_size);
        for (T* rhs_ptr = rhs.m_begin; rhs_ptr != rhs.m_end; ++rhs_ptr)
            new (m_end++) T(std::move(*rhs_ptr));
    }
    else {
        release();
        m_begin = rhs.m_begin;
        m_end = rhs.m_end;
        m_capacity = rhs.m_capacity;

        rhs.m_begin = nullptr;
        rhs.m_end = nullptr;
        rhs.m_capacity = 0;
        rhs.m_local_storage = false;
    }
}

template <typename T>
void vec<T>::assign(std::initializer_list<T> init)
{
    assign(init.begin(), init.end());
}

template <typename T>
template <typename It, typename>
void vec<T>::assign(It begin, It end)
{
    using category = typename std::iterator_traits<It>::iterator_category;
    assign_internal(begin, end, category());
}

template <typename T>
template <typename It>
void vec<T>::assign_internal(It begin, It end, std::input_iterator_tag)
{
    clear();
    for (It it = begin; it != end; ++it) {
        push_back(*it);
    }
}

template <typename T>
template <typename It>
void vec<T>::assign_internal(It begin, It end, std::forward_iterator_tag)
{
    clear();
    reserve(std::distance(begin, end));
    for (It it = begin; it != end; ++it)
        new (m_end++) T(*it);
}

template <typename T>
template <typename... Args>
void vec<T>::fill(size_t count, Args&&... args)
{
    clear();
    reserve(count);
    for (size_t i = 0; i < count; i++)
        emplace_back(std::forward<Args>(args)...);
}

template <typename T>
void vec<T>::pop_back()
{
    m_end--;
    m_end->~T();
}

template <typename T>
void vec<T>::push_back(const T& val)
{
    emplace_back(val);
}

template <typename T>
void vec<T>::push_back(T&& val)
{
    emplace_back(std::move(val));
}

template <typename T>
void vec<T>::grow_if_necessary()
{
    if (size() == m_capacity)
        reserve(size() * 1.5 + 4);        
}

template <typename T>
template <typename... Args>
void vec<T>::emplace_back(Args&&... args)
{
    grow_if_necessary();
    new (m_end++) T(std::forward<Args>(args)...);
}

template <typename T>
void vec<T>::insert(const_iterator it, const T& val)
{
    T* ptr;
    bool is_constructed;
    std::tie(ptr, is_constructed) = create_space(it, 1);
    if (is_constructed)
        *ptr = val;
    else
        new (ptr) T(val);
}

template <typename T>
void vec<T>::insert(const_iterator it, T&& val)
{
    T* ptr;
    bool is_constructed;
    std::tie(ptr, is_constructed) = create_space(it, 1);
    if (is_constructed)
        *ptr = std::move(val);
    else
        new (ptr) T(std::move(val));
}

template <typename T>
template <typename... Args>
void vec<T>::emplace(const_iterator it, Args&&... args)
{
    T* ptr;
    bool is_constructed;
    std::tie(ptr, is_constructed) = create_space(it, 1);
    ptr->~T();
    new (ptr) T(std::forward<Args>(args)...);
}

template <typename T>
template <typename It>
void vec<T>::insert_internal(const_iterator pos, It begin, It end, std::input_iterator_tag)
{
    if (pos == end()) {
        for (It it = begin; it != end; ++it)
            push_back(*it);
    }
    else {
        vec<T> temp(begin, end);
        insert_internal(pos,
                        std::make_move_iterator(temp.begin()), 
                        std::make_move_iterator(temp.end()), 
                        std::forward_iterator_tag());
    }
}

template <typename T>
template <typename It>
void vec<T>::insert_internal(const_iterator pos, It begin, It end, std::forward_iterator_tag)
{
    size_t num_new_elements = std::distance(begin, end);
    T* ptr;
    bool is_constructed;
    std::tie(ptr, is_constructed) = create_space(pos, num_new_elements); 
    for (It it = begin; it != end; ++it, ++ptr)
        if (is_constructed)
            *ptr = *it;
        else
            new (ptr) T(*it);
}

template <typename T>
template <typename It, typename>
void vec<T>::insert(const_iterator pos, It begin, It end)
{
    using category = typename std::iterator_traits<It>::iterator_category;
    insert_internal(pos, begin, end, category());
}

template <typename T>
tup<T*, bool> vec<T>::create_space(const_iterator pos, size_t length, std::true_type is_relocatable)
{
    std::ptrdiff_t old_size = size();
    std::ptrdiff_t offset = pos.p - m_begin;

    if (old_size + length > m_capacity) {
        size_t new_capacity = old_size + length;
        T* new_block = reinterpret_cast<T*>(realloc(m_begin, sizeof(T) * new_capacity));
        if (!new_block)
            throw std::bad_alloc();
        m_begin = new_block;
        m_capacity = new_capacity;
    }

    m_end = m_begin + old_size + length;
    memmove(m_begin + offset + length, m_begin + offset, sizeof(T) * (old_size - offset));

    // Returning not constructed her may seem weird, since we just memmoved values out of this
    // location. But conceptually we have now relocated the original value and the residual
    // contents are just garbage. We want the caller to construct in place into the memory
    // rather than assigning into it.
    return std::make_pair(m_begin + offset, false);
}

template <typename T>
tup<T*, bool> vec<T>::create_space(const_iterator pos, size_t length, std::false_type is_not_relocatable)
{
    if (pos == end()) {
        // Special case: when we've been asked to insert into the end, just do a reserve.
        // This is also the only situation when this version of create_space will return
        // false - i.e., that the memory is not currently constructed.
        reserve(m_capacity + length);
        T* original_end = m_end;
        m_end += length;
        return std::make_tuple(original_end, false);        
    }
    std::ptrdiff_t offset = pos.p - m_begin;
    std::ptrdiff_t old_size = size();
    if (old_size + length > m_capacity) {
            size_t new_capacity = old_size + length;
            T* new_begin = allocate(new_capacity);
            T* new_end = new_begin;

            for (T* ptr = m_begin; ptr != m_end; ++ptr, ++new_end) {
                if (ptr == pos.p)
                    new_end += length;

                new (new_end) T(std::move(*ptr));
                ptr->~T();
            }
            release();
            m_begin = new_begin;
            m_end = new_end;
            m_capacity = new_capacity;
    }
    else {
        // We just need to move
        for (std::ptrdiff_t move_from_offset = old_size - 1; move_from_offset >= offset; move_from_offset--) {
            std::ptrdiff_t move_to_offset = move_from_offset + length;
            if (move_to_offset > old_size)
                new (&m_begin[move_to_offset]) T(std::move(m_begin[move_from_offset]));
            else
                m_begin[move_to_offset] = std::move(m_begin[move_from_offset]);
        }
    }

    return std::make_tuple(m_begin + offset, true);
}

template <typename T>
tup<T*, bool> vec<T>::create_space(const_iterator pos, size_t length)
{
    return create_space(pos, length, is_relocatable_t<T>());
}

} // namespace dtm
