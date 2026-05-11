#include <gtest/gtest.h>
#include <rval_struct.hpp>

#include <set.hpp>

TEST(MultisetCompare, Test1)
{
    mstd::multiset<int> c1{1, 2, 3}, c2{1, 2, 3, 4}, c3{1, 2, 4};
    EXPECT_EQ(c1, c1);
    EXPECT_TRUE(std::is_eq(c1 <=> c1));
    EXPECT_LT(c1, c2);
    EXPECT_TRUE(std::is_lt(c1 <=> c2));
    EXPECT_LT(c1, c3);
    EXPECT_TRUE(std::is_lt(c1 <=> c3));
    EXPECT_LT(c2, c3);
    EXPECT_TRUE(std::is_lt(c2 <=> c3));

    static_assert(std::totally_ordered<mstd::multiset<int>>);

    //static_assert(std::three_way_comparable<mstd::multiset<int>, std::strong_ordering>);
    static_assert(!std::three_way_comparable<mstd::multiset<float, mstd::FpCompareThreeWay<float>>, std::strong_ordering>);
    static_assert(std::three_way_comparable<mstd::multiset<float, mstd::FpCompareThreeWay<float>>, std::weak_ordering>);
    static_assert(std::three_way_comparable<mstd::multiset<float, mstd::FpCompareThreeWay<float>>, std::partial_ordering>);

    struct E
    {
        bool operator==(E) { return true; }
    };
    struct Cmp
    {
        std::weak_ordering operator()(E, E) const { return std::weak_ordering::equivalent; }
    };
    static_assert(std::totally_ordered<mstd::multiset<E, Cmp>>);
    static_assert(!std::three_way_comparable<E>);
    static_assert(std::three_way_comparable<mstd::multiset<E, Cmp>>);
}

TEST(MultisetCompare, Test2)
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

    static_assert(std::totally_ordered<mstd::multiset<W>>);

    mstd::multiset<W> c1{{1}, {2}, {3}}, c2{{0}, {3}, {3}};
    static_assert(std::same_as<decltype(c1 <=> c1), std::weak_ordering>);
    EXPECT_EQ(c1, c2);
    EXPECT_TRUE(std::is_eq(c1 <=> c2));
}

TEST(MultisetCompare, Test3)
{
    struct L
    {
        int value = 0;

        auto operator<=>(L rhs) const noexcept { return value <=> rhs.value; }
    };

    static_assert(std::totally_ordered<mstd::multiset<L>>);

    mstd::multiset<L> c{{1}, {2}, {3}}, d{{1}, {2}, {3}, {4}};
    //static_assert(std::same_as<decltype(c <=> c), std::strong_ordering>);
    EXPECT_TRUE(std::is_lt(c <=> d));
}

// Associative container iterators are not random access
static_assert(!std::totally_ordered<mstd::multiset<int>::iterator>);
static_assert(!std::three_way_comparable<mstd::multiset<int>::iterator>);

TEST(MultisetOperationsTest, Test1)
{
    mstd::multiset<int> ms0;
    typedef mstd::multiset<int>::iterator iterator;
    typedef mstd::multiset<int>::const_iterator const_iterator;
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

    const mstd::multiset<int> &ms1 = ms0;
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
    mstd::multiset<int> m;
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

struct Zero
{
};
auto operator<=>(Zero, int i) { return 0 <=> i; }
auto operator<=>(int i, Zero) { return i <=> 0; }

struct One
{
};
auto operator<=>(One, int i) { return 1 <=> i; }
auto operator<=>(int i, One) { return i <=> 1; }

TEST(MultisetCompareTest, Test2)
{
    mstd::multiset<int> m;
    EXPECT_FALSE(m.contains(Zero{}));
    EXPECT_FALSE(m.contains(One{}));
    m.emplace(0);
    EXPECT_TRUE(m.contains(Zero{}));
    EXPECT_FALSE(m.contains(One{}));
    m.emplace(0);
    EXPECT_TRUE(m.contains(Zero{}));
    EXPECT_FALSE(m.contains(One{}));
    m.emplace(1);
    EXPECT_TRUE(m.contains(Zero{}));
    EXPECT_TRUE(m.contains(One{}));
}

TEST(MultisetCountTest, Test1)
{
    using namespace std;

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

class PathPoint
{
public:
    PathPoint(char t, const std::vector<double> &c)
        : type(t), coords(c) {}
    PathPoint(char t, std::vector<double> &&c)
        : type(t), coords(std::move(c)) {}
    char getType() const { return type; }
    const std::vector<double> &getCoords() const { return coords; }

private:
    char type;
    std::vector<double> coords;
};

struct PathPointCmp
{
    auto operator()(const PathPoint &__lhs, const PathPoint &__rhs) const
    {
        return __lhs.getType() <=> __rhs.getType();
    }
};

TEST(MultisetEmplaceTest, Test1)
{
    typedef mstd::multiset<PathPoint, PathPointCmp> Mset;
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
    mstd::multiset<aggressive_aggregate> x;
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
    mstd::multiset<aggressive_aggregate> x;
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
    mstd::multiset<int> ms0, ms1;
    mstd::multiset<int>::iterator iter1;

    ms0.insert(1);
    //ms1.insert(ms1.end(), 1);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(3);
    ms1.insert(ms1.begin(), 3);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(4);
    //iter1 = ms1.insert(ms1.end(), 4);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(6);
    ms1.insert(iter1, 6);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(2);
    ms1.insert(ms1.begin(), 2);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(7);
    //ms1.insert(ms1.end(), 7);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(5);
    ms1.insert(ms1.find(4), 5);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(0);
    //ms1.insert(ms1.end(), 0);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(8);
    ms1.insert(ms1.find(3), 8);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(9);
    //ms1.insert(ms1.end(), 9);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(10);
    ms1.insert(ms1.begin(), 10);
    EXPECT_EQ(ms0, ms1);
}

TEST(MultisetInsertTest, Test2)
{
    mstd::multiset<int> ms0, ms1;
    mstd::multiset<int>::iterator iter1;

    ms0.insert(1);
    //ms1.insert(ms1.end(), 1);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(3);
    ms1.insert(ms1.begin(), 3);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(4);
    //iter1 = ms1.insert(ms1.end(), 4);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(6);
    //ms1.insert(iter1, 6);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(2);
    ms1.insert(ms1.begin(), 2);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(7);
    //ms1.insert(ms1.end(), 7);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(5);
    ms1.insert(ms1.find(4), 5);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(0);
    //ms1.insert(ms1.end(), 0);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(8);
    ms1.insert(ms1.find(3), 8);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(9);
    //ms1.insert(ms1.end(), 9);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(10);
    ms1.insert(ms1.begin(), 10);
    EXPECT_EQ(ms0, ms1);
}

TEST(MultisetInsertTest, TestRvalStruct1)
{
    typedef mstd::multiset<rvalstruct> Set;
    Set s;
    EXPECT_TRUE(s.empty());

    Set::iterator i = s.insert(rvalstruct(1));
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(std::ranges::distance(s.begin(), s.end()), 1);
    EXPECT_EQ(i, s.begin());
    EXPECT_EQ((*i).val, 1);
}

TEST(MultisetInsertTest, TestRvalStruct2)
{
    typedef mstd::multiset<rvalstruct> Set;
    Set s;
    EXPECT_TRUE(s.empty());

    s.insert(rvalstruct(2));
    Set::iterator i = s.insert(rvalstruct(2));
    EXPECT_EQ(s.size(), 2);
    EXPECT_EQ(std::ranges::distance(s.begin(), s.end()), 2);
    EXPECT_EQ((*i).val, 2);

    Set::iterator i2 = s.begin();
    ++i2;
    EXPECT_EQ(i, i2);
    EXPECT_EQ((*(s.begin())).val , 2);
    EXPECT_EQ((*i2).val, 2);
}
