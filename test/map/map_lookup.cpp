#include <gtest/gtest.h>
#include <map.hpp>

#include <test_values.hpp>
#include <rval_struct.hpp>
#include <counter_type.hpp>

namespace mstd {

TEST(MapTest, SubstractTest)
{
    mstd::map<int, test::Mapped> m1;
    m1[0] = test::Mapped();

    mstd::map<int, test::rvalstruct> m2;
    m2[0] = test::rvalstruct(13);

    mstd::map<int, test::DefaultConstructibleType> m3;
    EXPECT_EQ(m3[0].val, 123);
    EXPECT_EQ(m3.size(), 1);
    m3[0] = 2;
    EXPECT_EQ(m3[0].val, 2);

    mstd::map<test::counter_type, int> m4;
    EXPECT_EQ(m4[test::counter_type(1)], 0);
    EXPECT_EQ(test::counter_type::specialize_count, 1);
    EXPECT_EQ(test::counter_type::copy_count, 0);
    EXPECT_EQ(test::counter_type::move_count, 1);

    test::counter_type k(2);
    test::counter_type::reset();

    EXPECT_EQ(m4[k], 0);
    EXPECT_EQ(test::counter_type::copy_count, 1);
    EXPECT_EQ(test::counter_type::move_count, 0);
}

TEST(MapAtTest, Test1)
{
    typedef mstd::map<char, int> Map;
    Map m;

    m['a'] = 1;
    m['b'] = 2;

    EXPECT_EQ(m.at('a'), 1);
    EXPECT_EQ(m.at('b'), 2);
}

}