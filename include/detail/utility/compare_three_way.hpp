#ifndef MSTD_UTILITY_COMPARE_THREE_WAY_HPP
#define MSTD_UTILITY_COMPARE_THREE_WAY_HPP

#include <utility>

namespace mstd {

    struct CompareThreeWay {
        template <class T, class U>
        auto operator()(T&& lhs, U&& rhs) const {
            return std::forward<T>(lhs) <=> std::forward<U>(rhs);
        }
    };

}

#endif
