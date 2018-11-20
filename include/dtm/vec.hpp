// vec.hpp
//

#ifndef INCLUDED_DATUM_VEC_HPP
#define INCLUDED_DATUM_VEC_HPP

#include <utility>
#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <cstddef>

#include "dtm/detail/move_buffer.hpp"
#include "dtm/detail/relocate_buffer.hpp"
#include "dtm/detail/iterators.hpp"
#include "dtm/detail/ptr.hpp"

namespace dtm {
template <
    typename T, 
    typename Buffer = 
        typename std::conditional<std::is_trivially_copyable<T>::value,
                detail::relocate_buffer<T>, // If the type is trivially copyable we can use the relocation buffer
                detail::move_buffer<T>      // Otherwise we'll fall back on the move buffer.
        >::type
>
class vec {
    static_assert(std::is_nothrow_move_constructible<T>::value &&
                  std::is_nothrow_move_assignable<T>::value);
public:
    using value_type = T;

    using iterator = detail::ptr<T>;
    using const_iterator = const detail::ptr<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using buffer_t = Buffer;

    static constexpr int minimum_growth_size = 4;
    static constexpr float growth_factor = 1.5; 

    vec() = default;

    template <typename... Args>
    explicit vec(size_t count, Args&&...);
    vec(std::initializer_list<T> init);

    template <typename It, typename = detail::require_input_iterator<It>>
    vec(It begin, It end);

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;

    reverse_iterator rbegin() noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator rend() const noexcept;

    void swap(vec& rhs) noexcept;

    size_t size() const noexcept;
    bool empty() const noexcept;
    size_t capacity() const noexcept;

    T& front() noexcept;
    T& back() noexcept;

    const T& front() const noexcept;
    const T& back() const noexcept;

    T& operator[] (size_t index) noexcept;
    const T& operator[] (size_t index) const noexcept;

    void reserve(size_t new_capacity);
    void shrink_to_fit();
    
    template <typename... Args>
    void resize(size_t new_size, Args&&...);

    T& push_back(const T& value);
    T& push_back(T&& value);

    template <typename... Args>
    T& emplace_back(Args&&...);

    void clear() noexcept;
    void pop_back() noexcept;

    iterator insert(const_iterator pos, const T& value);
    iterator insert(const_iterator pos, T&& value);
    iterator insert(const_iterator pos, size_t count, const T& value);
    template <typename It>
    iterator insert(const_iterator pos, It first, It last);
    iterator insert(const_iterator pos, std::initializer_list<T> init);

protected:
    vec(T* begin, T* end, size_t capacity);

private:
    buffer_t buffer;

    // The operation "emplace_back" is split into two functions:
    // * grow_if_necessary
    // * unsafe_emplace_back 
    //
    // grow_if_necessary triggers a buffer growth if the size is currently at capacity.
    // unsafe_emplace_back assumes the size is less than capacity, and constructs the
    // object at the current end.
    void grow_if_necessary();

    template <typename... Args>
    T& unsafe_emplace_back(Args&&...);
};

template <typename T, size_t Count, typename Buffer = detail::move_buffer<T>>
class small_vec : public vec<T,Buffer> {
    static_assert(std::is_nothrow_move_constructible<T>::value &&
                  std::is_nothrow_move_assignable<T>::value);
public:
    small_vec();

    template <typename... Args>
    explicit small_vec(size_t count, Args&&...);

    small_vec(std::initializer_list<T> init);

    template <typename It, typename = detail::require_input_iterator<It>>
    small_vec(It begin, It end);

    small_vec(const vec<T,Buffer>& v);

    small_vec(vec<T,Buffer>&& v);
};

}

// Implementation of vec is in detail/vec.hpp
#define INCLUDING_DATUM_DETAIL_VEC_IMPL_HPP
#include "detail/vec_impl.hpp"
#undef INCLUDING_DATUM_DETAIL_VEC_IMPL_HPP

#endif //INCLUDED_DATUM_VEC_HPP
