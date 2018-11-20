// move_buffer.hpp
//
// The buffers implement the resizable buffer semantics of vectors. However, they should not be used
// directly by calling code.
//
// Buffers are structs with all public members. They do not enforce internal invariants. A buffer is
// a tool to implement a safe and fast vector, but it is not a safe and fast vector. Its interface
// must be wrapped by another type which provides safety.

#ifndef INCLUDED_DATUM_DETAIL_MOVE_BUFFER_HPP
#define INCLUDED_DATUM_DETAIL_MOVE_BUFFER_HPP

#include <utility>
#include <type_traits>
#include <cstddef>

namespace dtm { 
namespace detail {

// template <typename T>
// struct malloc_memcpy_buffer
// {

// };

// template <typename T>
// struct realloc_buffer
// {

// };


template <typename T>
struct move_buffer
{
    static_assert(std::is_nothrow_move_constructible<T>::value);
    static_assert(std::is_nothrow_destructible<T>::value);

    T* begin = nullptr;
    T* end = nullptr;
    size_t capacity = 0;

    move_buffer() = default;
    move_buffer(size_t initial_size);
    move_buffer(const move_buffer&);
    move_buffer(move_buffer&&) noexcept;

    move_buffer& operator = (const move_buffer&);
    move_buffer&& operator = (move_buffer&&) noexcept;

    ~move_buffer();

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
    void copy_from(const move_buffer&);

    // Move implementation - used by move constructor and move assignment.
    void move_from(move_buffer&&) noexcept;

    // Shifts values to the right, assuming there's enough capacity.
    void shift_right_into_existing(size_t offset, size_t num) noexcept;

    // Reallocates range, creating a hole in the middle.
    void reallocate_and_shift_right(size_t offset, size_t num);
};

//
// Impementation
//

template <typename T>
move_buffer<T>::move_buffer(size_t initial_size) {
    begin = end = allocate(initial_size);
    capacity = initial_size;
}

template <typename T>
move_buffer<T>::move_buffer(const move_buffer& rhs) {
    copy_from(rhs);
}

template <typename T>
move_buffer<T>::move_buffer(move_buffer&& rhs) noexcept {
    move_from(std::move(rhs));
}

template <typename T>
move_buffer<T>& move_buffer<T>::operator = (const move_buffer& rhs) {
    this->~move_buffer<T>();
    copy_from(rhs);
    return *this;
}

template <typename T>
move_buffer<T>&& move_buffer<T>::operator = (move_buffer&& rhs) noexcept {
    this->~move_buffer<T>();
    move_from(rhs);
    return *this;
}

template <typename T>
move_buffer<T>::~move_buffer() {
    for (T* ptr = begin; ptr != end; ++ptr)
        destruct(ptr);
    deallocate(begin);
}

template <typename T>
T* move_buffer<T>::allocate(size_t size) {
    if (size == 0)
        return nullptr;
    else
        return static_cast<T*>(operator new[] (size * sizeof(T)));
}

template <typename T>
void move_buffer<T>::deallocate(T* ptr) {
    operator delete[] (static_cast<void*>(ptr));
}

template <typename T>
template <typename... Args>
void move_buffer<T>::construct(T* ptr, Args&&... args) {
    new (ptr) T(std::forward<Args>(args)...);
}

template <typename T>
void move_buffer<T>::construct(T* ptr, T&& rhs) noexcept {
    new (ptr) T(std::move(rhs));
}

template <typename T>
void move_buffer<T>::destruct(T* ptr) noexcept {
    ptr->~T();
}

template <typename T>
void move_buffer<T>::copy_from(const move_buffer& rhs) {
    capacity = rhs.end - rhs.begin;
    begin = end = allocate(capacity);
    for (T const* rhs_ptr = rhs.begin; rhs_ptr != rhs.end; ++rhs_ptr, ++end)
        construct(end, *rhs_ptr);
}

template <typename T>
void move_buffer<T>::move_from(move_buffer&& rhs) noexcept {
    capacity = rhs.capacity;
    begin = rhs.begin;
    end = rhs.end;

    rhs.begin = nullptr;
    rhs.end = nullptr;
    rhs.capacity = 0;
}

template <typename T>
void move_buffer<T>::reallocate(size_t new_size) {
    // This function isn't noexcept because allocate can throw. However, everything past this
    // point is noexcept. And if allocate throws, we haven't changed anything. Therefore this
    // function provides the strong exception guarantee.
    T* new_buffer = allocate(new_size);
    T* new_it = new_buffer;

    if (new_size > (end - begin)) {
        // If the new buffer is bigger than the old, we simply move all existing values into it
        // destroying the old ones along the way.
        for (T* old_it = begin; old_it != end; ++old_it, ++new_it) {
            construct(new_it, std::move(*old_it));
            destruct(old_it);
        }
    }
    else {
        // The new buffer is smaller than the old, so we'll move as much as can fit, and
        // simply destroy whatever is left behind.
        T* old_it = begin;
        T* new_end = new_buffer + new_size;
        for (; new_it != new_end; ++old_it, ++new_it) {
            construct(new_it, std::move(*old_it));
            destruct(old_it);
        }
        for (; old_it != end; ++old_it) {
            destruct(old_it);
        }
    }

    // Finally we can get rid of the original buffer and swap the pointers around.
    deallocate(begin);
    begin = new_buffer;
    end = new_it;
    capacity = new_size;
}

template <typename T>
void move_buffer<T>::reallocate_and_shift_right(size_t offset, size_t num)
{
    size_t new_size = (end - begin) + num;

    // The allocate is the only thing allowed to throw. After this point all operations should be
    // noexcept. If allocate throws we haven't altered the structure, thus providing the strong
    // exception guarantee.
    T* new_buffer = allocate(new_size);

    // Copy the values from the old buffer into the new buffer
    for (T* old_it = begin, * new_it = new_buffer; old_it != end; ++old_it, ++new_it) {
        // Check if we've hit the insertion point in the old buffer. Once we hit the insertion point we
        // want to skip ahead to in the new buffer. This will leave a chunk of unconstructed room which
        // new values can be placed into.
        if (old_it == begin + offset)
            new_it += num;           

        construct(new_it, std::move(*old_it));
        destruct(old_it);
    }

    // Get rid of original buffer and swap pointers around to point at new buffer.
    deallocate(begin);
    begin = new_buffer;
    end = begin + new_size;
    capacity = new_size;
}

template <typename T>
void move_buffer<T>::shift_right_into_existing(size_t offset, size_t num) noexcept
{
    // This function is implemented in terms of indices instead of pointers, because it makes the logic
    // a lot clearer. And the compiler should be able to turn it into faster code.

    size_t size = end - begin;

    // Loop over the values between the insert offset and the end of the list, shifting them to the right.
    for (std::ptrdiff_t i = size - 1; i >= static_cast<std::ptrdiff_t>(offset); i--) {
        if (i + num >= size)
            // If the write offset is beyond the old size, we're writing to a non-constructed element. So
            // move construct.
            construct(&begin[i + num], std::move(begin[i]));
        else
            // Otherwise it is constructed and we move assign in.
            begin[i + num] = std::move(begin[i]);
    }

    end += num;
}

template <typename T>
bool move_buffer<T>::shift_right(size_t offset, size_t num)
{
    size_t size = end - begin;
    if (size + num <= capacity) {
        if (offset == size) {
            end += num;
            // New space is at the end of the list. So, return false because they'll need to be constructed.
            return false;
        }
        else {
            shift_right_into_existing(offset, num);
            // The space to be moved into is already constructed.  
            return true;
        }
    }
    else {
        reallocate_and_shift_right(offset, num);

        // New elements won't be constructed since we've done a reallocation.
        return false;
    }
}

} } // namespace detail, dtm

#endif //INCLUDED_DATUM_DETAIL_MOVE_BUFFER_HPP
