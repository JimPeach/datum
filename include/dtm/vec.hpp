// vec.hpp
//

#ifndef INCLUDED_DATUM_VEC_HPP
#define INCLUDED_DATUM_VEC_HPP

#include <new>
#include <array>
#include <utility>
#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <cstddef>
#include <cstring>

#include "dtm/tup.hpp"

#include "dtm/detail/config.hpp"
#include "dtm/detail/iterators.hpp"
#include "dtm/detail/ptr.hpp"

//
// XXX provide exception safety via two-stage move.
//

namespace dtm {

template <typename T>
struct is_relocatable {
    // By default, all trivially copyable types are relocatable.
    static constexpr bool value = std::is_trivially_copyable<T>::value;
};

template <typename T>
using is_relocatable_t = typename std::conditional<is_relocatable<T>::value, std::true_type, std::false_type>::type;

template <typename T>
class vec {
public:
    using value_type = T;

    using iterator = detail::ptr<T>;
    using const_iterator = const detail::ptr<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    vec();

    template <typename... Args>
    explicit vec(size_t count, Args&&... args);

    vec(std::initializer_list<T> init);

    template <typename It, typename = detail::require_input_iterator<It>>
    vec(It begin, It end);

    vec(const vec<T>& v);

    vec(vec<T>&& v);

    ~vec();

    vec& operator= (const vec& rhs);
    vec& operator= (vec&& rhs);
    vec& operator= (std::initializer_list<T> init);

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

    reverse_iterator rbegin() noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    void swap(vec& rhs) noexcept;

    T& front() noexcept;
    T& back() noexcept;

    const T& front() const noexcept;
    const T& back() const noexcept;

    T& operator[] (size_t) noexcept;
    const T& operator[] (size_t) const noexcept;

    T& at(size_t);
    const T& at(size_t) const;

    size_t size() const noexcept;
    bool empty() const noexcept;
    size_t capacity() const noexcept;

    void reserve(size_t size);
    void shrink_to_fit();

    void clear();
    
    template <typename... Args>
    void resize(size_t new_size, Args&&...);

    void assign(const vec<T>& rhs);
    void assign(vec<T>&& rhs);
    void assign(std::initializer_list<T> init);

    template <typename It, typename = detail::require_input_iterator<It>>
    void assign(It begin, It end);

    template <typename... Args>
    void fill(size_t count, Args&&... args);

    void pop_back();

    void push_back(const T&);
    void push_back(T&&);

    template <typename... Args>
    void emplace_back(Args&&...);

    template <typename... Args>
    void emplace(const_iterator it, Args&&...);

    template <typename It, typename = detail::require_input_iterator<It>>
    void insert(const_iterator it, It begin, It end);

    void insert(const_iterator it, const T&);
    void insert(const_iterator it, T&&);

protected:
    vec(T* local_store, size_t local_store_capacity);

    template <typename... Args>
    vec(T* local_store, size_t local_store_capacity, size_t count, Args&&... args);

    vec(T* local_store, size_t local_store_capacity, std::initializer_list<T> init);

    template <typename It, typename = detail::require_input_iterator<It>>
    vec(T* local_store, size_t local_store_capacity, It begin, It end);

    vec(T* local_store, size_t local_store_capacity, const vec<T>& v);

    vec(T* local_store, size_t local_store_capacity, vec<T>&& v);

private:
    T* m_begin;
    T* m_end;

#ifdef DATUM_IS_64BIT_SIZEt
    size_t m_capacity : 63;
    bool m_local_storage : 1;
#else
    size_t m_capacity;
    bool m_local_storage;
#endif

    template <typename It>
    void assign_internal(It begin, It end, std::input_iterator_tag);

    template <typename It>
    void assign_internal(It begin, It end, std::forward_iterator_tag);

    template <typename It>
    void insert_internal(const_iterator pos, It begin, It end, std::input_iterator_tag);

    template <typename It>
    void insert_internal(const_iterator pos, It begin, It end, std::forward_iterator_tag);


    void grow_if_necessary();

    void reserve_internal(size_t size, std::true_type is_relocatable);
    void reserve_internal(size_t size, std::false_type is_not_relocatable);

    tup<T*, bool> create_space(const_iterator pos, size_t length, std::true_type is_relocatable);
    tup<T*, bool> create_space(const_iterator pos, size_t length, std::false_type is_not_relocatable);
    tup<T*, bool> create_space(const_iterator pos, size_t length);

    T* allocate(size_t size);
    void release();
};

template <typename T, size_t LocalSize>
class small_vec : public vec<T> {
public:
    small_vec()
        : vec<T>(local_storage.begin(), local_storage.size())
    {}

    template <typename... Args>
    explicit small_vec(size_t count, Args&&... args)
        : vec<T>(local_storage.begin(), local_storage.size(), std::forward<Args>(args)...)
    {}

    small_vec(std::initializer_list<T> init)
        : vec<T>(local_storage.begin(), local_storage.size(), init)
    {}

    template <typename It, typename = detail::require_input_iterator<It>>
    small_vec(It begin, It end)
        : vec<T>(local_storage.begin(), local_storage.size(), begin, end)
    {}

    small_vec(const vec<T>& v)
        : vec<T>(local_storage.begin(), local_storage.size(), v)
    {}

    small_vec(vec<T>&& v)
        : vec<T>(local_storage.begin(), local_storage.size(), std::move(v))
    {}

private:
    std::array<T, LocalSize> local_storage;
};

}

// Implementation of vec is in detail/vec.hpp
#define INCLUDING_DATUM_DETAIL_VEC_IMPL_HPP
#include "detail/vec_impl.hpp"
#undef INCLUDING_DATUM_DETAIL_VEC_IMPL_HPP

#endif //INCLUDED_DATUM_VEC_HPP
