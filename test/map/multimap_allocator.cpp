#include <gtest/gtest.h>
#include <test_values.hpp>

#include <map.hpp>
#include <rval_struct.hpp>
#include <test_allocator.hpp>
#include <propogating_allocator.hpp>
#include <tracker_allocator.hpp>
#include <uneq_allocator.hpp>
#include <detail/utility/compare_three_way.hpp>

namespace mstd {

TEST(MultimapUneqAllocMove, Test1)
{
    using alloc_type = uneq_allocator<MapValue<test::T, test::U>>;
    using test_type = multimap<test::T, test::U, CompareThreeWay, alloc_type>;
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
    using alloc_type = uneq_allocator<MapValue<test::T, test::U>>;
    using test_type = multimap<test::T, test::U, CompareThreeWay, alloc_type>;
    test_type v1(alloc_type(1));
    v1 = {test_type::value_type{}};
    test_type v2(std::move(v1), alloc_type(2));
    EXPECT_EQ(1, v1.get_allocator().get_personality());
    EXPECT_EQ(2, v2.get_allocator().get_personality());
}

TEST(MultimapUneqAllocMoveAssign, Test1)
{
    using alloc_type = propagating_allocator<MapValue<test::T, test::U>, false>;
    using test_type = mstd::multimap<test::T, test::U, CompareThreeWay, alloc_type>;
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
    using alloc_type = propagating_allocator<MapValue<test::T, test::U>, true>;
    using test_type = mstd::multimap<test::T, test::U, CompareThreeWay, alloc_type>;
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

    using alloc_type = propagating_allocator<MapValue<int, int>, false,
                                  tracker_allocator<MapValue<int, int>>>;
    using test_type = multimap<int, int, CompareThreeWay, alloc_type>;

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

}
