// detail/iterators.hpp
//

#include <iterator>
#include <type_traits>
#include <cstddef>

namespace dtm {
namespace detail {

template<typename It> using require_input_iterator = typename
    std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<It>::iterator_category, 
            std::input_iterator_tag
        >::value
    >::type;

template <typename It>
size_t min_range_size(It begin, It end, std::forward_iterator_tag) {
    return std::distance(begin, end);
}

template <typename It>
size_t min_range_size(It begin, It end, std::input_iterator_tag) {
    return 0;
}

template <typename It>
size_t min_range_size(It begin, It end) {
    return min_range_size(begin, end, typename std::iterator_traits<It>::iterator_category());
}

} } // namespace