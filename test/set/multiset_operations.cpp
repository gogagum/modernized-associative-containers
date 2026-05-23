#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <rval_struct.hpp>
#include <test_values.hpp>

#include <set.hpp>

namespace mstd {

struct aggressive_aggregate
{
    int a;
    int b;
};

auto operator<=>(const aggressive_aggregate &a, const aggressive_aggregate &b)
{
    return a.a <=> b.a;
};

TEST(MultisetEmplaceTest, Test2)
{
    multiset<aggressive_aggregate> x;
    auto it = x.emplace(1, 2);
    EXPECT_EQ(it->a, 1);
    EXPECT_EQ(it->b, 2);
    it = x.emplace(2);
    EXPECT_EQ(it->a, 2);
    EXPECT_EQ(it->b, 0);
    it = x.emplace();
    EXPECT_EQ(it->a, 0);
    EXPECT_EQ(it->b, 0);
}

TEST(MultisetEmplaceTest, TestHint)
{
    multiset<aggressive_aggregate> x;
    auto it = x.emplace_hint(x.begin(), 3, 2);
    EXPECT_EQ(it->a, 3);
    EXPECT_EQ(it->b, 2);
    it = x.emplace_hint(x.begin(), 4);
    EXPECT_EQ(it->a, 4);
    EXPECT_EQ(it->b, 0);
    it = x.emplace_hint(x.begin());
    EXPECT_EQ(it->a, 0);
    EXPECT_EQ(it->b, 0);
}

TEST(MultisetInsertTest, Test1)
{
    multiset<int> ms0, ms1;
    multiset<int>::iterator iter1;

    ms0.insert(1);
    ms1.insert(ms1.end(), 1);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(3);
    ms1.insert(ms1.begin(), 3);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(4);
    iter1 = ms1.insert(ms1.end(), 4);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(6);
    ms1.insert(iter1, 6);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(2);
    ms1.insert(ms1.begin(), 2);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(7);
    ms1.insert(ms1.end(), 7);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(5);
    ms1.insert(ms1.find(4), 5);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(0);
    ms1.insert(ms1.end(), 0);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(8);
    ms1.insert(ms1.find(3), 8);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(9);
    ms1.insert(ms1.end(), 9);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(10);
    ms1.insert(ms1.begin(), 10);
    EXPECT_EQ(ms0, ms1);
}

TEST(MultisetInsertTest, Test2)
{
    multiset<int> ms0, ms1;
    multiset<int>::iterator iter1;

    ms0.insert(1);
    ms1.insert(ms1.end(), 1);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(3);
    ms1.insert(ms1.begin(), 3);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(4);
    iter1 = ms1.insert(ms1.end(), 4);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(6);
    ms1.insert(iter1, 6);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(2);
    ms1.insert(ms1.begin(), 2);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(7);
    ms1.insert(ms1.end(), 7);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(5);
    ms1.insert(ms1.find(4), 5);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(0);
    ms1.insert(ms1.end(), 0);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(8);
    ms1.insert(ms1.find(3), 8);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(9);
    ms1.insert(ms1.end(), 9);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(10);
    ms1.insert(ms1.begin(), 10);
    EXPECT_EQ(ms0, ms1);
}

TEST(MultisetInsertTest, TestRvalStruct1)
{
    using Set = multiset<test::rvalstruct>;
    Set s;
    EXPECT_TRUE(s.empty());

    Set::iterator i = s.insert(test::rvalstruct(1));
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(std::distance(s.begin(), s.end()), 1);
    EXPECT_EQ(i, s.begin());
    EXPECT_EQ((*i).val, 1);
}

TEST(MultisetInsertTest, TestRvalStruct2)
{
    using Set = multiset<test::rvalstruct>;
    Set s;
    EXPECT_TRUE(s.empty());

    s.insert(test::rvalstruct(2));
    Set::iterator i = s.insert(test::rvalstruct(2));
    EXPECT_EQ(s.size(), 2);
    EXPECT_EQ(std::distance(s.begin(), s.end()), 2);
    EXPECT_EQ((*i).val, 2);

    Set::iterator i2 = s.begin();
    ++i2;
    EXPECT_EQ(i, i2);
    EXPECT_EQ(s.begin()->val, 2);
    EXPECT_EQ(i->val, 2);
}

TEST(MultisetEmplaceTest, Test1)
{
    using Mset = multiset<test::PathPoint, test::PathPointCmp>;
    Mset ms;

    std::vector<double> coord1 = {0.0, 1.0, 2.0};

    auto it = ms.emplace('a', coord1);
    EXPECT_EQ(ms.size(), 1);
    EXPECT_EQ(it->getType(), 'a');

    coord1[0] = 3.0;
    it = ms.emplace('a', coord1);
    EXPECT_EQ(ms.size(), 2);
    EXPECT_EQ(it->getType(), 'a');
    EXPECT_EQ(it->getCoords()[0], 3.0);

    it = ms.emplace_hint(ms.begin(), 'b', coord1);
    EXPECT_NE(it, ms.end());
    EXPECT_EQ(it->getType(), 'b');
    EXPECT_EQ(it->getCoords()[0], 3.0);

    double *px = &coord1[0];
    it = ms.emplace('c', std::move(coord1));
    EXPECT_EQ(ms.size(), 4);
    EXPECT_EQ(it->getType(), 'c');
    EXPECT_EQ(&(it->getCoords()[0]), px);
}

// Associative container iterators are not random access
static_assert(!std::totally_ordered<multiset<int>::iterator>);
static_assert(!std::three_way_comparable<multiset<int>::iterator>);

TEST(MultisetOperationsTest, Test1)
{
    multiset<int> ms0;
    using iterator = multiset<int>::iterator;
    using const_iterator = multiset<int>::const_iterator;
    std::pair<iterator, iterator> pp0;

    pp0 = ms0.equal_range(1);
    EXPECT_EQ(ms0.count(1), 0);
    EXPECT_EQ(pp0.first, ms0.end());
    EXPECT_EQ(pp0.second, ms0.end());

    iterator iter0 = ms0.insert(1);
    iterator iter1 = ms0.insert(2);
    iterator iter2 = ms0.insert(3);

    pp0 = ms0.equal_range(2);
    EXPECT_EQ(ms0.count(2), 1);
    EXPECT_EQ(*pp0.first, 2);
    EXPECT_EQ(*pp0.second, 3);
    EXPECT_EQ(pp0.first, iter1);
    EXPECT_EQ(--pp0.first, iter0);
    EXPECT_EQ(pp0.second, iter2);

    ms0.insert(3);
    iterator iter3 = ms0.insert(3);
    iterator iter4 = ms0.insert(4);

    pp0 = ms0.equal_range(3);
    EXPECT_EQ(ms0.count(3), 3);
    EXPECT_EQ(*pp0.first, 3);
    EXPECT_EQ(*pp0.second, 4);
    EXPECT_EQ(pp0.first, iter2);
    EXPECT_EQ(--pp0.first, iter1);
    EXPECT_EQ(pp0.second, iter4);

    iterator iter5 = ms0.insert(0);
    ms0.insert(1);
    ms0.insert(1);
    ms0.insert(1);

    pp0 = ms0.equal_range(1);
    EXPECT_EQ(ms0.count(1), 4);
    EXPECT_EQ(*pp0.first, 1);
    EXPECT_EQ(*pp0.second, 2);
    EXPECT_EQ(pp0.first, iter0);
    EXPECT_EQ(--pp0.first, iter5);
    EXPECT_EQ(pp0.second, iter1);

    iterator iter6 = ms0.insert(5);
    ms0.insert(5);
    ms0.insert(5);

    pp0 = ms0.equal_range(5);
    EXPECT_EQ(ms0.count(5), 3);
    EXPECT_EQ(*pp0.first, 5);
    EXPECT_EQ(pp0.first, iter6);
    EXPECT_EQ(--pp0.first, iter4);
    EXPECT_EQ(pp0.second, ms0.end());

    ms0.insert(4);
    ms0.insert(4);
    ms0.insert(4);

    pp0 = ms0.equal_range(4);
    EXPECT_EQ(ms0.count(4), 4);
    EXPECT_EQ(*pp0.first, 4);
    EXPECT_EQ(*pp0.second, 5);
    EXPECT_EQ(pp0.first, iter4);
    EXPECT_EQ(--pp0.first, iter3);
    EXPECT_EQ(pp0.second, iter6);

    ms0.insert(0);
    iterator iter7 = ms0.insert(0);
    ms0.insert(1);

    pp0 = ms0.equal_range(0);
    EXPECT_EQ(ms0.count(0), 3);
    EXPECT_EQ(*pp0.first, 0);
    EXPECT_EQ(*pp0.second, 1);
    EXPECT_EQ(pp0.first, iter5);
    EXPECT_EQ(pp0.first, ms0.begin());
    EXPECT_EQ(pp0.second, iter0);

    const multiset<int> &ms1 = ms0;
    std::pair<const_iterator, const_iterator> pp1 = ms1.equal_range(1);
    EXPECT_EQ(ms1.count(1), 5);
    EXPECT_EQ(*pp1.first, 1);
    EXPECT_EQ(*pp1.second, 2);
    EXPECT_EQ(pp1.first, iter0);
    EXPECT_EQ(--pp1.first, iter7);
    EXPECT_EQ(pp1.second, iter1);
}

TEST(MultisetCompareTest, Test1)
{
    multiset<int> m;
    EXPECT_FALSE(m.contains(0));
    EXPECT_FALSE(m.contains(1));
    m.emplace(0);
    EXPECT_TRUE(m.contains(0));
    EXPECT_FALSE(m.contains(1));
    m.emplace(0);
    EXPECT_TRUE(m.contains(0));
    EXPECT_FALSE(m.contains(1));
    m.emplace(1);
    EXPECT_TRUE(m.contains(0));
    EXPECT_TRUE(m.contains(1));
}

TEST(MultisetCompareTest, Test2)
{
    multiset<int> m;
    EXPECT_FALSE(m.contains(test::Zero{}));
    EXPECT_FALSE(m.contains(test::One{}));
    m.emplace(0);
    EXPECT_TRUE(m.contains(test::Zero{}));
    EXPECT_FALSE(m.contains(test::One{}));
    m.emplace(0);
    EXPECT_TRUE(m.contains(test::Zero{}));
    EXPECT_FALSE(m.contains(test::One{}));
    m.emplace(1);
    EXPECT_TRUE(m.contains(test::Zero{}));
    EXPECT_TRUE(m.contains(test::One{}));
}

TEST(MultisetCountTest, Test1)
{
    multiset<int> ms0;
    EXPECT_EQ(ms0.count(0), 0);
    EXPECT_EQ(ms0.count(1), 0);

    ms0.insert(1);
    EXPECT_EQ(ms0.count(0), 0);
    EXPECT_EQ(ms0.count(1), 1);

    ms0.insert(1);
    EXPECT_EQ(ms0.count(0), 0);
    EXPECT_EQ(ms0.count(1), 2);

    ms0.insert(2);
    EXPECT_EQ(ms0.count(2), 1);

    ms0.insert(3);
    ms0.insert(3);
    ms0.insert(3);
    EXPECT_EQ(ms0.count(3), 3);

    ms0.erase(2);
    EXPECT_EQ(ms0.count(2), 0);

    ms0.erase(0);
    EXPECT_EQ(ms0.count(0), 0);

    multiset<int> ms1(ms0);
    EXPECT_EQ(ms1.count(0), 0);
    EXPECT_EQ(ms1.count(1), 2);
    EXPECT_EQ(ms1.count(2), 0);
    EXPECT_EQ(ms1.count(3), 3);

    ms0.clear();
    EXPECT_EQ(ms0.count(0), 0);
    EXPECT_EQ(ms0.count(1), 0);
    EXPECT_EQ(ms0.count(2), 0);
    EXPECT_EQ(ms0.count(3), 0);

    ms1.insert(4);
    ms1.insert(5);
    ms1.insert(5);
    ms1.insert(5);
    ms1.insert(5);
    EXPECT_EQ(ms1.count(4), 1);
    EXPECT_EQ(ms1.count(5), 4);

    ms1.erase(1);
    EXPECT_EQ(ms1.count(1), 0);

    ms1.erase(ms1.find(5));
    EXPECT_EQ(ms1.count(5), 3);

    ms1.insert(1);
    ms1.insert(1);
    EXPECT_EQ(ms1.count(1), 2);

    ms1.erase(5);
    EXPECT_EQ(ms1.count(5), 0);

    ms1.erase(ms1.find(4));
    EXPECT_EQ(ms1.count(4), 0);

    ms1.clear();
    EXPECT_EQ(ms1.count(0), 0);
    EXPECT_EQ(ms1.count(1), 0);
    EXPECT_EQ(ms1.count(2), 0);
    EXPECT_EQ(ms1.count(3), 0);
    EXPECT_EQ(ms1.count(4), 0);
    EXPECT_EQ(ms1.count(5), 0);
}

} // namespace mstd
