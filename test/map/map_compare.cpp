#include <gtest/gtest.h>

#include <map.hpp>

#include <test_values.hpp>

namespace mstd {

TEST(MapCompareTests, CompareNumeric)
{
    map<int, int> c1{{1, 1}, {2, 1}, {3, 1}};
    map<int, int> c2{{1, 1}, {2, 1}, {3, 1}, {4, 1}};
    map<int, int> c3{{1, 1}, {2, 1}, {3, 2}};
    EXPECT_EQ(c1, c1);
    EXPECT_TRUE(std::is_eq(c1 <=> c1));
    EXPECT_LT(c1, c2);
    EXPECT_TRUE(std::is_lt(c1 <=> c2));
    EXPECT_LT(c1, c3);
    EXPECT_TRUE(std::is_lt(c1 <=> c3));
    EXPECT_LT(c2, c3);
    EXPECT_TRUE(std::is_lt(c2 <=> c3));

    static_assert(std::totally_ordered<map<int, int>>);

    static_assert(std::three_way_comparable<map<int, int>, std::strong_ordering>);
    static_assert(!std::three_way_comparable<map<float, float, FpCompareThreeWay<float>>, std::strong_ordering>);
    static_assert(std::three_way_comparable<map<float, float, FpCompareThreeWay<float>>, std::weak_ordering>);
    static_assert(std::three_way_comparable<map<int, float>, std::weak_ordering>);

    static_assert(std::totally_ordered<map<test::AllEqual, int>>);
    static_assert(!std::three_way_comparable<test::AllEqual>);
    static_assert(std::three_way_comparable<map<test::AllEqual, int>>);
}

TEST(MapCompareTests, BitOr1Key)
{
    using Map = map<test::BitOr1, test::BitOr1>;
    static_assert(std::totally_ordered<Map>);
    auto c1 = Map{ {1, 1}, {2, 2}, {3, 3} }; 
    auto c2 = Map{ {1, 0}, {3, 2}, {3, 3} };

    static_assert(std::same_as<decltype(c1 <=> c1), std::weak_ordering>);
    EXPECT_EQ(c1, c2);
    EXPECT_TRUE(std::is_eq(c1 <=> c2));
}

TEST(MapCompareTests, WeaklyOrderedKey)
{
    {
        using Map = mstd::map<mstd::test::WeaklyOrdered, mstd::test::WeaklyOrdered>;
        static_assert(std::totally_ordered<Map>);

        auto c1 = Map{{1, 1}, {2, 2}, {3, 3}};
        auto c2 = Map{{1, 1}, {2, 2}, {3, 4}};
        static_assert(std::same_as<decltype(c1 <=> c2), std::weak_ordering>);
        EXPECT_TRUE(std::is_lt(c1 <=> c2));
    }

    {
        using Map = mstd::map<int, mstd::test::WeaklyOrdered>;
        static_assert(std::totally_ordered<Map>);

        auto c1 = Map{{1, 1}, {2, 2}, {3, 3}};
        auto c2 = Map{{1, 1}, {2, 2}, {3, 4}};
        // static_assert(std::same_as<decltype(c1 <=> c2), std::weak_ordering>); // TODO(gogagum)
        EXPECT_TRUE(std::is_lt(c1 <=> c2));
    }
}

// Associative container iterators are not random access
static_assert(!std::totally_ordered<mstd::map<int, int>::iterator>);
static_assert(!std::three_way_comparable<mstd::map<int, int>::iterator>);

} // namespace mstd
