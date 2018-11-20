// ptr.hpp

namespace dtm {
namespace detail {

template <typename T>
class ptr
{
    template <typename, typename> friend class vec;
public:
    explicit ptr(T* p_) noexcept { p = p_; }

    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using pointer = T*;
    using iterator_category = std::random_access_iterator_tag;

    ptr& operator ++ () noexcept { ++p; return *this; }
    ptr& operator -- () noexcept { --p; return *this; }

    ptr operator ++ (int) noexcept { return ptr(p++); }
    ptr operator -- (int) noexcept { return ptr(p--); }

    ptr& operator += (std::ptrdiff_t n) noexcept { p += n; return *this; }
    ptr& operator -= (std::ptrdiff_t n) noexcept { p -= n; return *this; }

    std::ptrdiff_t operator - (ptr rhs) const noexcept { return p - rhs.p; }

    ptr operator - (std::ptrdiff_t diff) const noexcept { return ptr(p - diff); }
    ptr operator + (std::ptrdiff_t diff) const noexcept { return ptr(p + diff); }

    bool operator == (ptr rhs) const noexcept { return p == rhs.p; }
    bool operator != (ptr rhs) const noexcept { return p != rhs.p; }
 
    bool operator > (ptr rhs) const noexcept { return (rhs.p - p) > 0; }
    bool operator < (ptr rhs) const noexcept { return (p - rhs.p) > 0; }
    bool operator >= (ptr rhs) const noexcept { return !(*this < rhs); }
    bool operator <= (ptr rhs) const noexcept { return !(*this > rhs); }

    T& operator[] (size_t n) const noexcept { return p[n]; }
    T& operator* () const noexcept { return *p; }
    T* operator-> () const noexcept { return p; }

private:
    T* p;
};


}
}