#include <gtest/gtest.h>

#include <map.hpp>

#include <test_values.hpp>

namespace mstd {

TEST(MultimapEmplaceTest, Test1)
{
    using Multimap = multimap<char, std::vector<double>>;
    Multimap m;

    std::vector<double> coord1 = {0.0, 1.0, 2.0};

    auto it = m.emplace('a', coord1);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(it->key(), 'a');

    coord1[0] = 3.0;
    auto it2 = m.emplace('a', coord1);
    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(it2->key(), 'a');

    auto it3 = m.emplace_hint(m.begin(), 'b', coord1);
    EXPECT_NE(it3, m.end());
    EXPECT_EQ(it3->key(), 'b');
    EXPECT_EQ(it3->value()[0], 3.0);

    double *px = &coord1[0];
    auto it4 = m.emplace('c', std::move(coord1));
    EXPECT_EQ(it4->key(), 'c');
    EXPECT_EQ(&(it4->value()[0]), px);
}

TEST(MultimapEmplaceTest, Test2)
{
    using Multimap = multimap<char, test::PathPoint>;
    Multimap m;

    std::vector<double> coord1 = {0.0, 1.0, 2.0};

    auto it = m.emplace(std::piecewise_construct,
                        std::tuple('a'), std::tuple('a', coord1));
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(it->key(), 'a');

    coord1[0] = 3.0;
    auto it2 = m.emplace(std::piecewise_construct,
                         std::tuple('a'), std::tuple('b', coord1));
    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(it2->key(), 'a');
    EXPECT_EQ(it2->value().getCoords()[0], 3.0);

    auto it3 = m.emplace_hint(m.begin(), std::piecewise_construct,
                              std::tuple('b'), std::tuple('c', coord1));
    EXPECT_NE(it3, m.end());
    EXPECT_EQ(it3->key(), 'b');
    EXPECT_EQ(it3->value().getCoords()[0], 3.0);

    double *px = &coord1[0];
    auto it4 = m.emplace(std::piecewise_construct,
                         std::tuple('c'), std::tuple('d', std::move(coord1)));
    EXPECT_EQ(it4->key(), 'c');
    EXPECT_EQ(&(it4->value().getCoords()[0]), px);
}

TEST(MultimapInsertTest, Test1)
{
    using Multimap = multimap<int, int>;
    Multimap m;
    Multimap::iterator hint;

    hint = m.insert(Multimap::value_type(7, 0));

    m.insert(hint, Multimap::value_type(8, 1));
    m.insert(m.begin(), Multimap::value_type(9, 2));

    EXPECT_NE(m.find(7), m.end());
    EXPECT_NE(m.find(8), m.end());
    EXPECT_NE(m.find(9), m.end());
}

TEST(MultimapInsertTest, Test2)
{
    using Multimap = multimap<int, const int>;

    Multimap m;

    for (unsigned i = 0; i < 10; ++i)
        m.insert(Multimap::value_type(i, i));

    for (Multimap::const_iterator i = m.begin(); i != m.end(); ++i)
    {
    }

    for (Multimap::const_iterator i = m.begin(); m.end() != i; ++i)
    {
    }
}

TEST(MultimapInsertTest, Test3)
{
    using Multimap = multimap<int, test::Val>;
    Multimap m;
    
    auto it1 = m.insert(Multimap::value_type(0, test::Val(5)));
    EXPECT_NE(it1, m.end());
    EXPECT_EQ(it1->value().val, 5);
    
    test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    
    auto it2 = m.insert(Multimap::value_type(0, std::move(v1)));
    EXPECT_NE(it2, m.end());
    EXPECT_EQ(it2->value().val, 6);
    EXPECT_EQ(m.size(), 2);
    
    v1.moved_from_assign = false;
    auto it3 = m.insert(Multimap::value_type(1, std::move(v1)));
    EXPECT_NE(it3, m.end());
    EXPECT_EQ(it3->value().val, 6);
    EXPECT_EQ(m.size(), 3);
}

TEST(MultimapInsertTest, Test4)
{
    using Multimap = multimap<int, test::Val>;
    Multimap m;
    
    auto it1 = m.insert(m.begin(), Multimap::value_type(0, test::Val(5)));
    EXPECT_NE(it1, m.end());
    EXPECT_EQ(it1->value().val, 5);
    
    test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    
    auto it2 = m.insert(m.begin(), Multimap::value_type(0, std::move(v1)));
    EXPECT_EQ(it2->value().val, 6);
    EXPECT_EQ(m.size(), 2);
    
    v1.moved_from_assign = false;
    auto it3 = m.insert(m.begin(), Multimap::value_type(1, std::move(v1)));
    EXPECT_EQ(it3->value().val, 6);
    EXPECT_EQ(m.size(), 3);
}

TEST(MultimapInsertTest, Test5)
{
    using Multimap = multimap<test::Val, test::Val>;
    Multimap m;
    
    auto it1 = m.insert(Multimap::value_type(test::Val(0), test::Val(5)));
    EXPECT_NE(it1, m.end());
    EXPECT_EQ(it1->value().val, 5);
    
    test::Val k1{0};
    test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    
    auto it2 = m.insert(Multimap::value_type(std::move(k1), std::move(v1)));
    EXPECT_EQ(it2->value().val, 6);
    EXPECT_EQ(m.size(), 2);
    
    test::Val k2{1};
    v1.moved_from_assign = false;
    auto it3 = m.insert(Multimap::value_type(std::move(k2), std::move(v1)));
    EXPECT_EQ(it3->value().val, 6);
    EXPECT_EQ(m.size(), 3);
}

}
