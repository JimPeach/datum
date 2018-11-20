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
    T* reallocate(size_t new_capacity);

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
};


} } // namespace dtm::detail

#endif //INCLUDED_DATUM_DETAIL_RELOCATE_BUFFER_HPP
