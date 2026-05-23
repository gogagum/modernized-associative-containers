#include <gtest/gtest.h>
#include <map.hpp>

namespace mstd {
    TEST(MapOperationsTest, Test1)
{
    map<int, int> mm0;
    typedef map<int, int>::value_type value_type;

    auto pp0 = mm0.equal_range(1);
    EXPECT_EQ(mm0.count(1), 0);
    EXPECT_EQ(pp0.begin(), mm0.end());
    EXPECT_EQ(pp0.end(), mm0.end());

    auto [iter0, inserted0] = mm0.insert(value_type(1, 1));
    auto [iter1, inserted1] = mm0.insert(value_type(2, 2));
    auto [iter2, inserted2] = mm0.insert(value_type(3, 3));

    pp0 = mm0.equal_range(2);
    EXPECT_EQ(mm0.count(2), 1);
    EXPECT_EQ(*pp0.begin(), value_type(2, 2));
    EXPECT_EQ(*pp0.end(), value_type(3, 3));
    EXPECT_EQ(pp0.begin(), iter1);
    EXPECT_EQ(--pp0.begin(), iter0);
    EXPECT_EQ(pp0.end(), iter2);

    auto [iter3, inserted3] = mm0.insert(value_type(3, 4));
    auto [iter4, inserted4] = mm0.insert(value_type(4, 6));

    pp0 = mm0.equal_range(3);
    EXPECT_EQ(mm0.count(3), 1);
    EXPECT_EQ(*pp0.begin(), value_type(3, 3));
    EXPECT_EQ(*pp0.end(), value_type(4, 6));
    EXPECT_EQ(pp0.begin(), iter2);
    EXPECT_EQ(--pp0.begin(), iter1);
    EXPECT_EQ(pp0.end(), iter4);

    auto [iter5, inserted5] = mm0.insert(value_type(0, 7));
    mm0.insert(value_type(1, 8));
    mm0.insert(value_type(1, 9));
    mm0.insert(value_type(1, 10));

    pp0 = mm0.equal_range(1);
    EXPECT_EQ(mm0.count(1), 1);
    EXPECT_EQ(*pp0.begin(), value_type(1, 1));
    EXPECT_EQ(*pp0.end(), value_type(2, 2));
    EXPECT_EQ(pp0.begin(), iter0);
    EXPECT_EQ(--pp0.begin(), iter5);
    EXPECT_EQ(pp0.end(), iter1);

    auto [iter6, inserted6] = mm0.insert(value_type(5, 11));
    mm0.insert(value_type(5, 12));
    mm0.insert(value_type(5, 13));

    pp0 = mm0.equal_range(5);
    EXPECT_EQ(mm0.count(5), 1);
    EXPECT_EQ(*pp0.begin(), value_type(5, 11));
    EXPECT_EQ(pp0.begin(), iter6);
    EXPECT_EQ(--pp0.begin(), iter4);
    EXPECT_EQ(pp0.end(), mm0.end());

    mm0.insert(value_type(4, 14));
    mm0.insert(value_type(4, 15));
    mm0.insert(value_type(4, 16));

    pp0 = mm0.equal_range(4);
    EXPECT_EQ(mm0.count(4), 1);
    EXPECT_EQ(*pp0.begin(), value_type(4, 6));
    EXPECT_EQ(*pp0.end(), value_type(5, 11));
    EXPECT_EQ(pp0.begin(), iter4);
    EXPECT_EQ(--pp0.begin(), iter3);
    EXPECT_EQ(pp0.end(), iter6);

    mm0.insert(value_type(0, 17));
    auto [iter7, inserted7] = mm0.insert(value_type(0, 18));
    mm0.insert(value_type(1, 19));

    pp0 = mm0.equal_range(0);
    EXPECT_EQ(mm0.count(0), 1);
    EXPECT_EQ(*pp0.begin(), value_type(0, 7));
    EXPECT_EQ(*pp0.end(), value_type(1, 1));
    EXPECT_EQ(pp0.begin(), iter5);
    EXPECT_EQ(pp0.begin(), mm0.begin());
    EXPECT_EQ(pp0.end(), iter0);

    const map<int, int> &mm1 = mm0;
    auto pp1 = mm1.equal_range(1);
    EXPECT_EQ(mm1.count(1), 1);
    EXPECT_EQ(*pp1.begin(), value_type(1, 1));
    EXPECT_EQ(*pp1.end(), value_type(2, 2));
    EXPECT_EQ(pp1.begin(), iter0);
    EXPECT_EQ(--pp1.begin(), iter7);
    EXPECT_EQ(pp1.end(), iter1);
}

}