#include <gtest/gtest.h>

#include <test_values.hpp>

#include <set.hpp>

namespace mstd {

TEST(SetCompare, Numeric)
{
    auto c1 = set{1, 2, 3};
    auto c2 = set{1, 2, 3, 4};
    auto c3 = set{1, 2, 4};
    EXPECT_EQ(c1, c1);
    EXPECT_TRUE(std::is_eq(c1 <=> c1));
    EXPECT_LT(c1, c2);
    EXPECT_TRUE(std::is_lt(c1 <=> c2));
    EXPECT_LT(c1, c3);
    EXPECT_TRUE(std::is_lt(c1 <=> c3));
    EXPECT_LT(c2, c3);
    EXPECT_TRUE(std::is_lt(c2 <=> c3));

    static_assert(std::totally_ordered<set<int>>);

    static_assert(std::three_way_comparable<set<int>, std::strong_ordering>);
    static_assert(!std::three_way_comparable<set<float, fp_compare_three_way<float>>, std::strong_ordering>);
    static_assert(std::three_way_comparable<set<float, fp_compare_three_way<float>>, std::weak_ordering>);
    static_assert(std::three_way_comparable<set<float, fp_compare_three_way<float>>, std::partial_ordering>);

    struct E
    {
        bool operator==(E) { return true; }
    };
    struct Cmp
    {
        auto operator()(E, E) const noexcept { return std::weak_ordering::equivalent; }
    };
    static_assert(std::totally_ordered<set<E, Cmp>>);
    static_assert(!std::three_way_comparable<E>);
    static_assert(std::three_way_comparable<set<E, Cmp>>);
}

TEST(SetCompare, BitOr1)
{
    static_assert(std::totally_ordered<set<test::BitOr1>>);

    auto c1 = set<test::BitOr1>{{1}, {2}, {3}};
    auto c2 = set<test::BitOr1>{{0}, {3}, {3}};
    static_assert(std::same_as<decltype(c1 <=> c1), std::weak_ordering>);
    EXPECT_EQ(c1, c2);
    EXPECT_TRUE(std::is_eq(c1 <=> c2));
}

TEST(SetCompare, WeaklyOrdered)
{
    static_assert(std::totally_ordered<set<test::WeaklyOrdered>>);

    auto c = set<test::WeaklyOrdered>{{1}, {2}, {3}};
    auto d = set<test::WeaklyOrdered>{{1}, {2}, {3}, {4}};
    static_assert(std::same_as<decltype(c <=> d), std::weak_ordering>);
    EXPECT_TRUE(std::is_lt(c <=> d));
}


}
