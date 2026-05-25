#ifndef MSTD_UTILITY_COMPARE_THREE_WAY_HPP
#define MSTD_UTILITY_COMPARE_THREE_WAY_HPP

#include <cassert>
#include <cmath>
#include <concepts>
#include <utility>

namespace mstd {

struct compare_three_way {
    template <class T, class U>
    auto operator()(T&& lhs, U&& rhs) const {
        return std::forward<T>(lhs) <=> std::forward<U>(rhs);
    }
};

template <std::floating_point T>
struct fp_compare_three_way {
    std::weak_ordering operator()(T lhs, T rhs) const {
        assert(!std::isnan(lhs) && !std::isnan(rhs));
        if (lhs < rhs) {
            return std::weak_ordering::less;
        } else if (rhs < lhs) {
            return std::weak_ordering::greater;
        } else {
            return std::weak_ordering::equivalent;
        }
    }

    template <std::floating_point U>
    std::weak_ordering operator()(T lhs, U rhs) const {
        assert(!std::isnan(lhs) && !std::isnan(rhs));
        if (lhs < rhs) {
            return std::weak_ordering::less;
        } else if (rhs < lhs) {
            return std::weak_ordering::greater;
        } else {
            return std::weak_ordering::equivalent;
        }
    }

    template <std::floating_point U>
    std::weak_ordering operator()(U lhs, T rhs) const {
        assert(!std::isnan(lhs) && !std::isnan(rhs));
        if (lhs < rhs) {
            return std::weak_ordering::less;
        } else if (rhs < lhs) {
            return std::weak_ordering::greater;
        } else {
            return std::weak_ordering::equivalent;
        }
    }

    template <class U>
        requires(!std::floating_point<U>)
    auto operator()(T lhs, U&& rhs) const {
        return lhs <=> std::forward<U>(rhs);
    }

    template <class U>
        requires(!std::floating_point<U>)
    auto operator()(U&& lhs, T rhs) const {
        return std::forward<U>(lhs) <=> rhs;
    }
};

} // namespace mstd

#endif
