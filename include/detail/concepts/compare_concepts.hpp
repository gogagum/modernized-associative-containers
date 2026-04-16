#ifndef MSTD_COMPARE_TRAITS_HPP
#define MSTD_COMPARE_TRAITS_HPP

#include <compare>

namespace mstd {

template <class CompareT, class Key1T, class Key2T, class Ordering>
concept OrdersWithAtLeast = requires (const CompareT& comp, Key1T key1, Key2T key2) {
    { comp(key1, key2) } -> std::convertible_to<Ordering>;
    { comp(key2, key1) } -> std::convertible_to<Ordering>;
};

template <class CompareT, class Key1T, class Key2T>
concept OrdersWithAtLeastWeakly = OrdersWithAtLeast<CompareT, Key1T, Key2T, std::weak_ordering>;

template <class CompareT, class Key1T, class Key2T>
concept OrdersWithAtLeastStrongly = OrdersWithAtLeast<CompareT, Key1T, Key2T, std::strong_ordering>;

template <class CompareT, class Key, class Ordering>
concept OrdersAtLeast = OrdersWithAtLeast<CompareT, Key, Key, Ordering>;

template <class CompareT, class Key>
concept OrdersAtLeastWeakly = OrdersAtLeast<CompareT, Key, std::weak_ordering>;

template <class CompareT, class Key>
concept OrdersAtLeastStrongly = OrdersAtLeast<CompareT, Key, std::strong_ordering>;

}

#endif // MSTD_COMPARE_TRAITS_HPP