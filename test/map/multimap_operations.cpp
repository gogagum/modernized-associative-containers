#include "detail/utility/compare_three_way.hpp"
#include <gtest/gtest.h>
#include <rval_struct.hpp>
#include <counter_type.hpp>
#include <uneq_allocator.hpp>
#include <propogating_allocator.hpp>
#include <tracker_allocator.hpp>

#include <map.hpp>
#include <string>
#include <test_values.hpp>

TEST(MultimapOperationsTest, Test1)
{
    mstd::multimap<int, int> mm0;
    typedef mstd::multimap<int, int>::iterator iterator;
    typedef mstd::multimap<int, int>::const_iterator const_iterator;
    typedef mstd::multimap<int, int>::value_type value_type;

    auto pp0 = mm0.equal_range(1);
    EXPECT_EQ(mm0.count(1), 0);
    EXPECT_EQ(pp0.begin(), mm0.end());
    EXPECT_EQ(pp0.end(), mm0.end());

    iterator iter0 = mm0.insert(value_type(1, 1));
    iterator iter1 = mm0.insert(value_type(2, 2));
    iterator iter2 = mm0.insert(value_type(3, 3));

    pp0 = mm0.equal_range(2);
    EXPECT_EQ(mm0.count(2), 1);
    EXPECT_EQ(*pp0.begin(), value_type(2, 2));
    EXPECT_EQ(*pp0.end(), value_type(3, 3));
    EXPECT_EQ(pp0.begin(), iter1);
    EXPECT_EQ(--pp0.begin(), iter0);
    EXPECT_EQ(pp0.end(), iter2);

    mm0.insert(value_type(3, 4));
    iterator iter3 = mm0.insert(value_type(3, 5));
    iterator iter4 = mm0.insert(value_type(4, 6));

    pp0 = mm0.equal_range(3);
    EXPECT_EQ(mm0.count(3), 3);
    EXPECT_EQ(*pp0.begin(), value_type(3, 3));
    EXPECT_EQ(*pp0.end(), value_type(4, 6));
    EXPECT_EQ(pp0.begin(), iter2);
    EXPECT_EQ(--pp0.begin(), iter1);
    EXPECT_EQ(pp0.end(), iter4);

    iterator iter5 = mm0.insert(value_type(0, 7));
    mm0.insert(value_type(1, 8));
    mm0.insert(value_type(1, 9));
    mm0.insert(value_type(1, 10));

    pp0 = mm0.equal_range(1);
    EXPECT_EQ(mm0.count(1), 4);
    EXPECT_EQ(*pp0.begin(), value_type(1, 1));
    EXPECT_EQ(*pp0.end(), value_type(2, 2));
    EXPECT_EQ(pp0.begin(), iter0);
    EXPECT_EQ(--pp0.begin(), iter5);
    EXPECT_EQ(pp0.end(), iter1);

    iterator iter6 = mm0.insert(value_type(5, 11));
    mm0.insert(value_type(5, 12));
    mm0.insert(value_type(5, 13));

    pp0 = mm0.equal_range(5);
    EXPECT_EQ(mm0.count(5), 3);
    EXPECT_EQ(*pp0.begin(), value_type(5, 11));
    EXPECT_EQ(pp0.begin(), iter6);
    EXPECT_EQ(--pp0.begin(), iter4);
    EXPECT_EQ(pp0.end(), mm0.end());

    mm0.insert(value_type(4, 14));
    mm0.insert(value_type(4, 15));
    mm0.insert(value_type(4, 16));

    pp0 = mm0.equal_range(4);
    EXPECT_EQ(mm0.count(4), 4);
    EXPECT_EQ(*pp0.begin(), value_type(4, 6));
    EXPECT_EQ(*pp0.end(), value_type(5, 11));
    EXPECT_EQ(pp0.begin(), iter4);
    EXPECT_EQ(--pp0.begin(), iter3);
    EXPECT_EQ(pp0.end(), iter6);

    mm0.insert(value_type(0, 17));
    iterator iter7 = mm0.insert(value_type(0, 18));
    mm0.insert(value_type(1, 19));

    pp0 = mm0.equal_range(0);
    EXPECT_EQ(mm0.count(0), 3);
    EXPECT_EQ(*pp0.begin(), value_type(0, 7));
    EXPECT_EQ(*pp0.end(), value_type(1, 1));
    EXPECT_EQ(pp0.begin(), iter5);
    EXPECT_EQ(pp0.begin(), mm0.begin());
    EXPECT_EQ(pp0.end(), iter0);

    const mstd::multimap<int, int> &mm1 = mm0;
    auto pp1 = mm1.equal_range(1);
    EXPECT_EQ(mm1.count(1), 5);
    EXPECT_EQ(*pp1.begin(), value_type(1, 1));
    EXPECT_EQ(*pp1.end(), value_type(2, 2));
    EXPECT_EQ(pp1.begin(), iter0);
    EXPECT_EQ(--pp1.begin(), iter7);
    EXPECT_EQ(pp1.end(), iter1);
}

using Cmp = mstd::CompareThreeWay;

