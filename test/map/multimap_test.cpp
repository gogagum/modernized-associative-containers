#include "detail/utility/compare_three_way.hpp"
#include <gtest/gtest.h>
#include <rval_struct.hpp>
#include <counter_type.hpp>
#include <uneq_allocator.hpp>
#include <propogating_allocator.hpp>
#include <tracker_allocator.hpp>

#include <map.hpp>
#include <string>
#include <compare>

TEST(MultimapTest, Compare1)
{
    mstd::multimap<int, int> c1{{1, 1}, {2, 1}, {3, 1}};
    mstd::multimap<int, int> c2{{1, 1}, {2, 1}, {3, 1}, {4, 1}};
    mstd::multimap<int, int> c3{{1, 1}, {2, 1}, {3, 2}};
    EXPECT_EQ(c1, c1);
    EXPECT_TRUE(std::is_eq(c1 <=> c1));
    EXPECT_LT(c1, c2);
    EXPECT_TRUE(std::is_lt(c1 <=> c2));
    EXPECT_LT(c1, c3);
    EXPECT_TRUE(std::is_lt(c1 <=> c3));
    EXPECT_LT(c2, c3);
    EXPECT_TRUE(std::is_lt(c2 <=> c3));

    static_assert(std::totally_ordered<mstd::multimap<int, int>>);

    static_assert(std::three_way_comparable<mstd::multimap<int, int>, std::strong_ordering>);
    static_assert(!std::three_way_comparable<mstd::multimap<float, float, mstd::FpCompareThreeWay<float>>, std::strong_ordering>);
    static_assert(std::three_way_comparable<mstd::multimap<float, float, mstd::FpCompareThreeWay<float>>, std::weak_ordering>);
    static_assert(std::three_way_comparable<mstd::multimap<int, float>, std::partial_ordering>);

    struct E
    {
        auto operator<=>(E) const noexcept { return std::weak_ordering::equivalent; }
    };
    static_assert(std::totally_ordered<mstd::multimap<E, int>>);
    static_assert(!std::three_way_comparable<E>);
    static_assert(std::three_way_comparable<mstd::multimap<E, int>>);
}

TEST(MultimapTest, Compare2)
{
    struct W
    {
        int value = 0;

        bool operator==(W rhs) const noexcept
        {
            return (value | 1) == (rhs.value | 1);
        }

        std::weak_ordering
        operator<=>(W rhs) const noexcept
        {
            return (value | 1) <=> (rhs.value | 1);
        }
    };

    static_assert(std::totally_ordered<mstd::multimap<int, W>>);

    mstd::multimap<W, W> c1{{1, 1}, {2, 2}, {3, 3}}, c2{{1, 0}, {3, 2}, {3, 3}};
    static_assert(std::same_as<decltype(c1 <=> c1), std::weak_ordering>);
    EXPECT_EQ(c1, c2);
    EXPECT_TRUE(std::is_eq(c1 <=> c2));
}


TEST(MultimapTest, Compare3)
{
    struct L
    {
        int value = 0;

        std::weak_ordering operator<=>(L rhs) const noexcept { return value <=> rhs.value; }
        bool operator==(const L&) const = default;
    };

    static_assert(std::totally_ordered<mstd::multimap<int, L>>);

    mstd::multimap<L, L> c{{1, 1}, {2, 2}, {3, 3}}, d{{1, 1}, {2, 2}, {3, 4}};

    static_assert(std::same_as<decltype(c <=> c), std::weak_ordering>);
    EXPECT_TRUE(std::is_lt(c <=> d));
}
    

// Associative container iterators are not random access
static_assert(!std::totally_ordered<mstd::multimap<int, int>::iterator>);
static_assert(!std::three_way_comparable<mstd::multimap<int, int>::iterator>);

TEST(MultimapOperationsTest, Test1)
{
    using namespace std;

    multimap<int, int> mm0;
    typedef multimap<int, int>::iterator iterator;
    typedef multimap<int, int>::const_iterator const_iterator;
    pair<iterator, iterator> pp0;
    typedef multimap<int, int>::value_type value_type;

    pp0 = mm0.equal_range(1);
    EXPECT_EQ(mm0.count(1), 0);
    EXPECT_EQ(pp0.first, mm0.end());
    EXPECT_EQ(pp0.second, mm0.end());

    iterator iter0 = mm0.insert(value_type(1, 1));
    iterator iter1 = mm0.insert(value_type(2, 2));
    iterator iter2 = mm0.insert(value_type(3, 3));

    pp0 = mm0.equal_range(2);
    EXPECT_EQ(mm0.count(2), 1);
    EXPECT_EQ(*pp0.first, value_type(2, 2));
    EXPECT_EQ(*pp0.second, value_type(3, 3));
    EXPECT_EQ(pp0.first, iter1);
    EXPECT_EQ(--pp0.first, iter0);
    EXPECT_EQ(pp0.second, iter2);

    mm0.insert(value_type(3, 4));
    iterator iter3 = mm0.insert(value_type(3, 5));
    iterator iter4 = mm0.insert(value_type(4, 6));

    pp0 = mm0.equal_range(3);
    EXPECT_EQ(mm0.count(3), 3);
    EXPECT_EQ(*pp0.first, value_type(3, 3));
    EXPECT_EQ(*pp0.second, value_type(4, 6));
    EXPECT_EQ(pp0.first, iter2);
    EXPECT_EQ(--pp0.first, iter1);
    EXPECT_EQ(pp0.second, iter4);

    iterator iter5 = mm0.insert(value_type(0, 7));
    mm0.insert(value_type(1, 8));
    mm0.insert(value_type(1, 9));
    mm0.insert(value_type(1, 10));

    pp0 = mm0.equal_range(1);
    EXPECT_EQ(mm0.count(1), 4);
    EXPECT_EQ(*pp0.first, value_type(1, 1));
    EXPECT_EQ(*pp0.second, value_type(2, 2));
    EXPECT_EQ(pp0.first, iter0);
    EXPECT_EQ(--pp0.first, iter5);
    EXPECT_EQ(pp0.second, iter1);

    iterator iter6 = mm0.insert(value_type(5, 11));
    mm0.insert(value_type(5, 12));
    mm0.insert(value_type(5, 13));

    pp0 = mm0.equal_range(5);
    EXPECT_EQ(mm0.count(5), 3);
    EXPECT_EQ(*pp0.first, value_type(5, 11));
    EXPECT_EQ(pp0.first, iter6);
    EXPECT_EQ(--pp0.first, iter4);
    EXPECT_EQ(pp0.second, mm0.end());

    mm0.insert(value_type(4, 14));
    mm0.insert(value_type(4, 15));
    mm0.insert(value_type(4, 16));

    pp0 = mm0.equal_range(4);
    EXPECT_EQ(mm0.count(4), 4);
    EXPECT_EQ(*pp0.first, value_type(4, 6));
    EXPECT_EQ(*pp0.second, value_type(5, 11));
    EXPECT_EQ(pp0.first, iter4);
    EXPECT_EQ(--pp0.first, iter3);
    EXPECT_EQ(pp0.second, iter6);

    mm0.insert(value_type(0, 17));
    iterator iter7 = mm0.insert(value_type(0, 18));
    mm0.insert(value_type(1, 19));

    pp0 = mm0.equal_range(0);
    EXPECT_EQ(mm0.count(0), 3);
    EXPECT_EQ(*pp0.first, value_type(0, 7));
    EXPECT_EQ(*pp0.second, value_type(1, 1));
    EXPECT_EQ(pp0.first, iter5);
    EXPECT_EQ(pp0.first, mm0.begin());
    EXPECT_EQ(pp0.second, iter0);

    const multimap<int, int> &mm1 = mm0;
    pair<const_iterator, const_iterator> pp1 = mm1.equal_range(1);
    EXPECT_EQ(mm1.count(1), 5);
    EXPECT_EQ(*pp1.first, value_type(1, 1));
    EXPECT_EQ(*pp1.second, value_type(2, 2));
    EXPECT_EQ(pp1.first, iter0);
    EXPECT_EQ(--pp1.first, iter7);
    EXPECT_EQ(pp1.second, iter1);
}

struct T
{
    int i;
};

std::strong_ordering operator<=>(T l, T r) { return l.i <=> r.i; }

using Cmp = mstd::CompareThreeWay;

struct U
{
};


TEST(MultimapUneqAllocMove, Test1)
{
    typedef uneq_allocator<mstd::MapValue<T, U>> alloc_type;
    typedef mstd::multimap<T, U, Cmp, alloc_type> test_type;
    test_type v1(alloc_type(1));
    v1 = {test_type::value_type{}};
    auto it = v1.begin();

    test_type v2(std::move(v1));

    EXPECT_EQ(1, v1.get_allocator().get_personality());
    EXPECT_EQ(1, v2.get_allocator().get_personality());
    EXPECT_EQ(it, v2.begin());
}

TEST(MultimapUneqAllocMove, Test2)
{
    typedef uneq_allocator<mstd::MapValue<T, U>> alloc_type;
    typedef mstd::multimap<T, U, Cmp, alloc_type> test_type;
    test_type v1(alloc_type(1));
    v1 = {test_type::value_type{}};
    test_type v2(std::move(v1), alloc_type(2));
    EXPECT_EQ(1, v1.get_allocator().get_personality());
    EXPECT_EQ(2, v2.get_allocator().get_personality());
}

TEST(MultimapUneqAllocMoveAssign, Test1)
{
    typedef propagating_allocator<mstd::MapValue<T, U>, false> alloc_type;
    typedef mstd::multimap<T, U, Cmp, alloc_type> test_type;
    test_type v1(alloc_type(1));
    v1 = {test_type::value_type{}};
    test_type v2(alloc_type(2));
    v2 = {test_type::value_type{}};
    v2 = std::move(v1);
    EXPECT_EQ(1, v1.get_allocator().get_personality());
    EXPECT_EQ(2, v2.get_allocator().get_personality());
}

TEST(MultimapUneqAllocMoveAssign, Test2)
{
    typedef propagating_allocator<mstd::MapValue<T, U>, true> alloc_type;
    typedef mstd::multimap<T, U, Cmp, alloc_type> test_type;
    test_type v1(alloc_type(1));
    v1 = {test_type::value_type{}};
    auto it = v1.begin();
    test_type v2(alloc_type(2));
    v2 = {test_type::value_type{}};
    v2 = std::move(v1);
    EXPECT_EQ(1, v1.get_allocator().get_personality());
    EXPECT_EQ(1, v2.get_allocator().get_personality());
    EXPECT_EQ(it, v2.begin());
}

TEST(MultimapUneqAllocMoveAssign, Test3)
{

    typedef propagating_allocator<mstd::MapValue<int, int>, false,
                                  tracker_allocator<mstd::MapValue<int, int>>>
        alloc_type;
    typedef mstd::multimap<int, int, mstd::CompareThreeWay, alloc_type> test_type;

    tracker_allocator_counter::reset();

    test_type v1(alloc_type(1));
    v1 = {{1, 1}, {1, 1}};

    test_type v2(alloc_type(2));
    v2 = {{2, 2}, {2, 2}};

    auto allocs = tracker_allocator_counter::get_allocation_count();
    auto constructs = tracker_allocator_counter::get_construct_count();

    // Check no allocation on move assignment with non propagating allocators.
    v1 = std::move(v2);

    EXPECT_EQ(1, v1.get_allocator().get_personality());
    EXPECT_EQ(2, v2.get_allocator().get_personality());

    EXPECT_EQ(tracker_allocator_counter::get_allocation_count(), allocs);
    EXPECT_EQ(tracker_allocator_counter::get_construct_count(), constructs);
}
