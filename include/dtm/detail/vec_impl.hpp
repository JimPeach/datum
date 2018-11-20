// details/vec_impl.hpp
//

#ifndef INCLUDING_DATUM_DETAIL_VEC_IMPL_HPP
#error "Don't include or compile datum/detail/vec_impl.hpp directly." 
#endif

namespace dtm {

// Constructors

template <typename T, typename S>
template <typename... Args>
vec<T,S>::vec(size_t count, Args&&... args) {
    buffer.reallocate(count);
    for (int i = 0; i < count; ++i)
        unsafe_emplace_back(std::forward<Args>(args)...);
}

template <typename T, typename S>
vec<T,S>::vec(std::initializer_list<T> init)
    : buffer(init.size()) 
{
    for (auto it = std::begin(init), end = std::end(init); it != end; ++it)
        unsafe_emplace_back(*it);
}

template <typename T, typename S>
template <typename It, typename> 
vec<T,S>::vec(It begin, It end)
    : buffer(detail::min_range_size(begin, end))
{
    for (auto it = begin; it != end; ++it)
        push_back(*it);
}

template <typename T, typename S>
void vec<T,S>::swap(vec& rhs) noexcept
{
    buffer_t temp(std::move(rhs.buffer));
    rhs.buffer = std::move(buffer);
    buffer = std::move(temp);
}

// Size queries

template <typename T, typename S>
size_t vec<T,S>::size() const noexcept {
    return buffer.end - buffer.begin;   
}

template <typename T, typename S>
bool vec<T,S>::empty() const noexcept {
    return buffer.end == buffer.begin;
}

template <typename T, typename S>
size_t vec<T,S>::capacity() const noexcept {
    return buffer.capacity;
}

// Iterators

template <typename T, typename S>
typename vec<T,S>::iterator vec<T,S>::begin() noexcept {
    return iterator(buffer.begin);
}

template <typename T, typename S>
typename vec<T,S>::iterator vec<T,S>::end() noexcept {
    return iterator(buffer.end);
}

template <typename T, typename S>
typename vec<T,S>::const_iterator vec<T,S>::begin() const noexcept {
    return const_iterator(buffer.begin);
}

template <typename T, typename S>
typename vec<T,S>::const_iterator vec<T,S>::end() const noexcept {
    return const_iterator(buffer.end);
}

template <typename T, typename S>
typename vec<T,S>::reverse_iterator vec<T,S>::rbegin() noexcept {
    return std::make_reverse_iterator(end());
}

template <typename T, typename S>
typename vec<T,S>::reverse_iterator vec<T,S>::rend() noexcept {
    return std::make_reverse_iterator(begin());
}

template <typename T, typename S>
typename vec<T,S>::const_reverse_iterator vec<T,S>::rbegin() const noexcept {
    return std::make_reverse_iterator(end());
}

template <typename T, typename S>
typename vec<T,S>::const_reverse_iterator vec<T,S>::rend() const noexcept {
    return std::make_reverse_iterator(begin());
}

// Accessing

template <typename T, typename S>
T& vec<T,S>::operator[] (size_t index) noexcept {
    return buffer.begin[index];
}

template <typename T, typename S>
const T& vec<T,S>::operator[] (size_t index) const noexcept {
    return buffer.begin[index];
}

template <typename T, typename S>
T& vec<T,S>::front() noexcept {
    return *buffer.begin;
}

template <typename T, typename S>
T& vec<T,S>::back() noexcept {
    return *(buffer.end - 1);
}

template <typename T, typename S>
const T& vec<T,S>::front() const noexcept {
    return *buffer.begin;
}

template <typename T, typename S>
const T& vec<T,S>::back() const noexcept {
    return *(buffer.end - 1);
}


// Resizing

template <typename T, typename S>
void vec<T,S>::grow_if_necessary() {
    if ((buffer.begin + buffer.capacity) != buffer.end)
        return;

    size_t new_capacity = buffer.capacity * growth_factor + minimum_growth_size;
    buffer.reallocate(new_capacity);
}

template <typename T, typename S>
void vec<T,S>::reserve(size_t new_capacity) {
    if (new_capacity > buffer.capacity)
        buffer.reallocate(new_capacity);
}

template <typename T, typename S>
void vec<T,S>::shrink_to_fit() {
    size_t current_size = size();
    if (current_size < buffer.capacity) {
        buffer.reallocate(current_size);
    }
}

template <typename T, typename S>
template <typename... Args>
void vec<T,S>::resize(size_t new_size, Args&&... args) {
    if (new_size < size()) {
        // Shrink by destructing in place. We won't reallocate the buffer to a smaller
        // size unless explictly requested with shrink_to_fit.
        T* new_end = buffer.begin + new_size;
        for (T* ptr = new_end; ptr != buffer.end; ptr++) {
            buffer.destruct(ptr);
        }
        buffer.end = new_end;
    }
    else {
        if (new_size > buffer.capacity) {
            // We'll need to grow the vector.
            buffer.reallocate(new_size);
        }

        while (size() < new_size) {
            unsafe_emplace_back(std::forward<Args>(args)...);
        }
    }
}

template <typename T, typename S>
void vec<T,S>::clear() noexcept {
    for (T* ptr = buffer.begin; ptr != buffer.end; ++ptr)
        buffer.destruct(ptr);
    buffer.end = buffer.begin;
}

// Appending

template <typename T, typename S>
template <typename... Args>
T& vec<T,S>::unsafe_emplace_back(Args&&... args) {
    grow_if_necessary();
    T* ptr = buffer.end++;
    buffer.construct(ptr, std::forward<Args>(args)...);
    return *ptr;
}

template <typename T, typename S>
template <typename... Args>
T& vec<T,S>::emplace_back(Args&&... args) {
    grow_if_necessary();
    return unsafe_emplace_back(std::forward<Args>(args)...);
}

template <typename T, typename S>
T& vec<T,S>::push_back(const T& value) {
    return emplace_back(value);
}

template <typename T, typename S>
T& vec<T,S>::push_back(T&& value) {
    return emplace_back(std::move(value));
}

template <typename T, typename S>
void vec<T,S>::pop_back() noexcept {
    buffer.end--;
    buffer.destruct(buffer.end);
}

// Inserting

template <typename T, typename S>
typename vec<T,S>::iterator vec<T,S>::insert(const_iterator pos, const T& value) {
    std::ptrdiff_t offset = pos.p - buffer.begin;
    bool constructed = buffer.shift_right(offset, 1);

    T* ptr = &buffer.begin[offset];
    if (constructed)
        *ptr = value;
    else
        buffer.construct(ptr, value);

    return iterator(ptr);
}

template <typename T, typename S>
typename vec<T,S>::iterator vec<T,S>::insert(const_iterator pos, T&& value) {
    std::ptrdiff_t offset = pos.p - buffer.begin;
    bool constructed = buffer.shift_right(offset, 1);

    T* ptr = &buffer.begin[offset];
    if (constructed)
        *ptr = std::move(value);
    else
        buffer.construct(ptr, std::move(value));

    return iterator(ptr);
}

template <typename T, typename S>
typename vec<T,S>::iterator vec<T,S>::insert(const_iterator pos, size_t count, const T& value) {
    std::ptrdiff_t offset = pos.p - buffer.begin;
    bool constructed = buffer.shift_right(offset, 1);

    for (T* ptr = buffer.begin + offset, * ptr_end = buffer.begin + offset + count; ptr != ptr_end; ++ptr)
        if (constructed) 
            *ptr = value;
        else
            buffer.construct(ptr, value);

    return iterator(buffer.begin + offset);
}

template <typename T, typename S>
template <typename It>
typename vec<T,S>::iterator vec<T,S>::insert(const_iterator pos, It first, It last) {
    if (first == last)
        return pos;

    std::ptrdiff_t offset = pos.p - buffer.begin;
    bool constructed = buffer.shift_right(offset, 1);

    T* ptr = &buffer.begin[offset];
    for (It it = first; it != last; ++it)
        if (constructed) 
            *(ptr++) = *it;
        else
            buffer.construct(ptr++, *it);

    return iterator(buffer.begin + offset);    
}

template <typename T, typename S>
typename vec<T,S>::iterator vec<T,S>::insert(const_iterator pos, std::initializer_list<T> init) {
    return insert(pos, std::begin(init), std::end(init));
}

} // namespace dtm
