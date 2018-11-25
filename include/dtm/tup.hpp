// tup.hpp
//

#ifndef INCLUDED_DATUM_TUP_HPP
#define INCLUDED_DATUM_TUP_HPP

#include <tuple>

namespace dtm {
template <typename... Args>
using tup = std::tuple<Args...>;
}

#endif //INCLUDED_DATUM_TUP_HPP
