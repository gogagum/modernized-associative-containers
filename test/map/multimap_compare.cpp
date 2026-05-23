#include <gtest/gtest.h>

#include <map.hpp>

#include <test_values.hpp>

namespace mstd {

TEST(MultimapCompareTests, CompareNumeric)
{
    multimap<int, int> c1{{1, 1}, {2, 1}, {3, 1}};
    multimap<int, int> c2{{1, 1}, {2, 1}, {3, 1}, {4, 1}};
    multimap<int, int> c3{{1, 1}, {2, 1}, {3, 2}};
    EXPECT_EQ(c1, c1);
    EXPECT_TRUE(std::is_eq(c1 <=> c1));
    EXPECT_LT(c1, c2);
    EXPECT_TRUE(std::is_lt(c1 <=> c2));
    EXPECT_LT(c1, c3);
    EXPECT_TRUE(std::is_lt(c1 <=> c3));
    EXPECT_LT(c2, c3);
    EXPECT_TRUE(std::is_lt(c2 <=> c3));

    static_assert(std::totally_ordered<multimap<int, int>>);

    static_assert(std::three_way_comparable<multimap<int, int>, std::strong_ordering>);
    static_assert(!std::three_way_comparable<multimap<float, float, fp_compare_three_way<float>>, std::strong_ordering>);
    static_assert(std::three_way_comparable<multimap<float, float, fp_compare_three_way<float>>, std::weak_ordering>);
    static_assert(std::three_way_comparable<multimap<int, float>, std::weak_ordering>);

    static_assert(std::totally_ordered<multimap<test::AllEqual, int>>);
    static_assert(!std::three_way_comparable<test::AllEqual>);
    static_assert(std::three_way_comparable<multimap<test::AllEqual, int>>);
}

TEST(MultimapCompareTests, BitOr1Key)
{
    using Multimap = multimap<test::BitOr1, test::BitOr1>;
    static_assert(std::totally_ordered<Multimap>);

    auto c1 = Multimap{{1, 1}, {2, 2}, {3, 3}};
    auto c2 = Multimap{{1, 0}, {3, 2}, {3, 3}};
    static_assert(std::same_as<decltype(c1 <=> c1), std::weak_ordering>);
    EXPECT_EQ(c1, c2);
    EXPECT_TRUE(std::is_eq(c1 <=> c2));
}

TEST(MultimapCompareTests, WeaklyOrderedKey)
{
    {
        using Multimap = multimap<test::WeaklyOrdered, test::WeaklyOrdered>;
        static_assert(std::totally_ordered<Multimap>);

        auto c1 = Multimap{{1, 1}, {2, 2}, {3, 3}};
        auto c2 = Multimap{{1, 1}, {2, 2}, {3, 4}};
        static_assert(std::same_as<decltype(c1 <=> c2), std::weak_ordering>);
        EXPECT_TRUE(std::is_lt(c1 <=> c2));
    }

    {
        using Multimap = multimap<test::WeaklyOrdered, test::WeaklyOrdered>;
        static_assert(std::totally_ordered<Multimap>);

        auto c1 = Multimap{{1, 1}, {2, 2}, {3, 3}};
        auto c2 = Multimap{{1, 1}, {2, 2}, {3, 4}};
        // static_assert(std::same_as<decltype(c1 <=> c2), std::weak_ordering>); // TODO(gogagum)
        EXPECT_TRUE(std::is_lt(c1 <=> c2));
    }
}

// Associative container iterators are not random access
static_assert(!std::totally_ordered<multimap<int, int>::iterator>);
static_assert(!std::three_way_comparable<multimap<int, int>::iterator>);

} // namespace mstd
