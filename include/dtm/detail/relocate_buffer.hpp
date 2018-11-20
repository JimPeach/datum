// realloc_buffer.hpp
//
// The buffers implement the resizable buffer semantics of vectors. However, they should not be used
// directly by calling code.
//
// Buffers are structs with all public members. They do not enforce internal invariants. A buffer is
// a tool to implement a safe and fast vector, but it is not a safe and fast vector. Its interface
// must be wrapped by another type which provides safety.

#ifndef INCLUDED_DATUM_DETAIL_RELOCATE_BUFFER_HPP
#define INCLUDED_DATUM_DETAIL_RELOCATE_BUFFER_HPP

#include <utility>
#include <type_traits>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cassert>

namespace dtm { 
namespace detail {

template <typename T>
struct relocate_buffer
{
    static_assert(std::is_nothrow_move_constructible<T>::value);
    static_assert(std::is_nothrow_destructible<T>::value);

    T* begin = nullptr;
    T* end = nullptr;
    size_t capacity = 0;

    relocate_buffer() = default;
    relocate_buffer(size_t initial_size);
    relocate_buffer(const relocate_buffer&);
    relocate_buffer(relocate_buffer&&) noexcept;

    relocate_buffer& operator = (const relocate_buffer&);
    relocate_buffer&& operator = (relocate_buffer&&) noexcept;

    ~relocate_buffer();

    // Reallocates the buffer to meet a new capacity constraint. Will move values.
    void reallocate(size_t new_capacity);

    // Constructs a value at the provided pointer, forwarding arguments to the constructor.
    // Not noexcept - generally constructors are allowed to throw.
    template <typename... Args>
    void construct(T*, Args&&...);

    // Constructs a value at hte provided pointer, calling the move constructor. This is an
    // overload separate from the forwarding construct, because we want to specify this one
    // is noexcept.
    void construct(T*, T&&) noexcept;

    // Destroys the value at the location (calling the destructor).
    void destruct(T*) noexcept;

    // Shifts values right to leave a hole into which new values can be placed.
    //
    // Returns true if the hole is in a move-assignable state, returns false if the hole needs
    // to be constructed.
    bool shift_right(size_t offset, size_t num);

private:
    // Allocates a memory buffer for T of uninitialized memory.
    T* allocate(size_t);

    // Releases a previously allocated memory buffer.
    void deallocate(T*);

    // Copy implementation - used by copy constructor and copy assignment.
    void copy_from(const relocate_buffer&);

    // Move implementation - used by move constructor and move assignment.
    void move_from(relocate_buffer&&) noexcept;
};

template <typename T>
relocate_buffer<T>::relocate_buffer(size_t initial_size) {
    begin = end = allocate(initial_size);
    capacity = initial_size;
}

template <typename T>
relocate_buffer<T>::relocate_buffer(const relocate_buffer& rhs) {
    copy_from(rhs);
}

template <typename T>
relocate_buffer<T>::relocate_buffer(relocate_buffer&& rhs) noexcept {
    move_from(std::move(rhs));
}

template <typename T>
relocate_buffer<T>& relocate_buffer<T>::operator = (const relocate_buffer& rhs) {
    this->~relocate_buffer<T>();
    copy_from(rhs);
    return *this;
}

template <typename T>
relocate_buffer<T>&& relocate_buffer<T>::operator = (relocate_buffer&& rhs) noexcept {
    this->~relocate_buffer<T>();
    move_from(rhs);
    return *this;
}

template <typename T>
relocate_buffer<T>::~relocate_buffer() {
    for (T* ptr = begin; ptr != end; ++ptr)
        destruct(ptr);
    deallocate(begin);
}

template <typename T>
T* relocate_buffer<T>::allocate(size_t size) {
    if (size > 0)
        return static_cast<T*>(malloc(sizeof(T) * size));
    else
        return nullptr;
}

template <typename T>
void relocate_buffer<T>::deallocate(T* ptr) {
    free(ptr);
}

template <typename T>
template <typename... Args>
void relocate_buffer<T>::construct(T* ptr, Args&&... args) {
    new (ptr) T(std::forward<Args>(args)...);
}

template <typename T>
void relocate_buffer<T>::construct(T* ptr, T&& rhs) noexcept {
    new (ptr) T(std::move(rhs));
}

template <typename T>
void relocate_buffer<T>::destruct(T* ptr) noexcept {
    ptr->~T();
}

template <typename T>
void relocate_buffer<T>::copy_from(const relocate_buffer& rhs) {
    capacity = rhs.end - rhs.begin;
    begin = end = allocate(capacity);
    for (T const* rhs_ptr = rhs.begin; rhs_ptr != rhs.end; ++rhs_ptr, ++end)
        construct(end, *rhs_ptr);
}

template <typename T>
void relocate_buffer<T>::move_from(relocate_buffer&& rhs) noexcept {
    capacity = rhs.capacity;
    begin = rhs.begin;
    end = rhs.end;

    rhs.begin = nullptr;
    rhs.end = nullptr;
    rhs.capacity = 0;
}

template <typename T>
void relocate_buffer<T>::reallocate(size_t new_size) {
    size_t size = end - begin;
    if (new_size < size) {
        // We're shrinking the array - we'll have to do some deconstructing.
        for (T* it = begin + new_size; it != end; ++it)
            destruct(it);

        size = new_size;
    }

    begin = static_cast<T*>(realloc(begin, sizeof(T) * new_size));
    end = begin + size;
    capacity = new_size;
}

template <typename T>
bool relocate_buffer<T>::shift_right(size_t offset, size_t num)
{
    size_t size = end - begin;
    size_t new_size = size + num;
    if (new_size > capacity)
        reallocate(new_size);

    if (offset != size) {
        size_t num_to_move = size - offset;
        memmove(begin + offset + num, begin + offset, sizeof(T) * num_to_move);
    }

    end += num;

    // We want to construct rather than assign into the gap. Even if the memory held previously constructed
    // values, we've relocated them away. The original memory should now be treated as uninitialized (but non-zero).
    return false;
}


} } // namespace dtm::detail

#endif //INCLUDED_DATUM_DETAIL_RELOCATE_BUFFER_HPP
