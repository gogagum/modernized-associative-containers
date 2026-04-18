#include <gtest/gtest.h>
#include <rval_struct.hpp>

#include <set.hpp>
#include <string>

TEST(SetFromVector, Test1)
{
    std::vector<int> nums{1, 2, 2, 3};

    auto c1 = mstd::set(nums.begin(), nums.end());
}

TEST(SetCompare, Test1)
{
    mstd::set<int> c1{1, 2, 3}, c2{1, 2, 3, 4}, c3{1, 2, 4};
    EXPECT_EQ(c1, c1);
    EXPECT_TRUE(std::is_eq(c1 <=> c1));
    EXPECT_LT(c1, c2);
    EXPECT_TRUE(std::is_lt(c1 <=> c2));
    EXPECT_LT(c1, c3);
    EXPECT_TRUE(std::is_lt(c1 <=> c3));
    EXPECT_LT(c2, c3);
    EXPECT_TRUE(std::is_lt(c2 <=> c3));

    static_assert(std::totally_ordered<mstd::set<int>>);

    static_assert(std::three_way_comparable<mstd::set<int>, std::strong_ordering>);
    static_assert(!std::three_way_comparable<mstd::set<float, mstd::FpCompareThreeWay<float>>, std::strong_ordering>);
    static_assert(std::three_way_comparable<mstd::set<float, mstd::FpCompareThreeWay<float>>, std::weak_ordering>);
    static_assert(std::three_way_comparable<mstd::set<float, mstd::FpCompareThreeWay<float>>, std::partial_ordering>);

    struct E
    {
        bool operator==(E) { return true; }
    };
    struct Cmp
    {
        auto operator()(E, E) const noexcept { return std::weak_ordering::equivalent; }
    };
    static_assert(std::totally_ordered<mstd::set<E, Cmp>>);
    static_assert(!std::three_way_comparable<E>);
    static_assert(std::three_way_comparable<mstd::set<E, Cmp>>);
}

TEST(SetCompare, Test2)
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

    static_assert(std::totally_ordered<mstd::set<W>>);

    mstd::set<W> c1{{1}, {2}, {3}}, c2{{0}, {3}, {3}};
    static_assert(std::same_as<decltype(c1 <=> c1), std::weak_ordering>);
    EXPECT_EQ(c1, c2);
    EXPECT_TRUE(std::is_eq(c1 <=> c2));
}

TEST(SetCompare, Test3)
{
    struct L
    {
        int value = 0;

        std::weak_ordering operator<=>(L rhs) const noexcept { return value <=> rhs.value; }
        bool operator==(const L &other) const = default;
        // bool operator!=(const L& other) const = default;
    };

    static_assert(std::totally_ordered<mstd::set<L>>);

    mstd::set<L> c{{1}, {2}, {3}}, d{{1}, {2}, {3}, {4}};
    static_assert(std::same_as<decltype(c <=> c), std::weak_ordering>);
    EXPECT_TRUE(std::is_lt(c <=> d));
}

// Associative container iterators are not random access
static_assert(!std::totally_ordered<mstd::set<int>::iterator>);
static_assert(!std::three_way_comparable<mstd::set<int>::iterator>);

TEST(SetContains, Test1)
{
    mstd::set<int> m;
    EXPECT_FALSE(m.contains(0));
    EXPECT_FALSE(m.contains(1));
    m.insert(0);
    EXPECT_TRUE(m.contains(0));
    EXPECT_FALSE(m.contains(1));
    m.insert(1);
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

TEST(SetContains, Test2)
{
    mstd::set<int> m;
    EXPECT_FALSE(m.contains(Zero{}));
    EXPECT_FALSE(m.contains(One{}));
    m.insert(0);
    EXPECT_TRUE(m.contains(Zero{}));
    EXPECT_FALSE(m.contains(One{}));
    m.insert(1);
    EXPECT_TRUE(m.contains(Zero{}));
    EXPECT_TRUE(m.contains(One{}));
}

TEST(SetCount, Test1)
{
    mstd::set<int> s0;
    EXPECT_EQ(s0.count(0), 0);
    EXPECT_EQ(s0.count(1), 0);

    s0.insert(1);
    EXPECT_EQ(s0.count(0), 0);
    EXPECT_EQ(s0.count(1), 1);

    s0.insert(1);
    EXPECT_EQ(s0.count(0), 0);
    EXPECT_EQ(s0.count(1), 1);

    s0.insert(2);
    EXPECT_EQ(s0.count(2), 1);

    s0.insert(3);
    s0.insert(3);
    s0.insert(3);
    EXPECT_EQ(s0.count(3), 1);

    s0.erase(2);
    EXPECT_EQ(s0.count(2), 0);

    s0.erase(0);
    EXPECT_EQ(s0.count(0), 0);

    mstd::set<int> s1(s0);
    EXPECT_EQ(s1.count(0), 0);
    EXPECT_EQ(s1.count(1), 1);
    EXPECT_EQ(s1.count(2), 0);
    EXPECT_EQ(s1.count(3), 1);

    s0.clear();
    EXPECT_EQ(s0.count(0), 0);
    EXPECT_EQ(s0.count(1), 0);
    EXPECT_EQ(s0.count(2), 0);
    EXPECT_EQ(s0.count(3), 0);

    s1.insert(4);
    s1.insert(5);
    s1.insert(5);
    s1.insert(5);
    s1.insert(5);
    EXPECT_EQ(s1.count(4), 1);
    EXPECT_EQ(s1.count(5), 1);

    s1.erase(1);
    EXPECT_EQ(s1.count(1), 0);

    s1.erase(s1.find(5));
    EXPECT_EQ(s1.count(5), 0);

    s1.insert(1);
    s1.insert(1);
    EXPECT_EQ(s1.count(1), 1);

    s1.erase(5);
    EXPECT_EQ(s1.count(5), 0);

    s1.erase(s1.find(4));
    EXPECT_EQ(s1.count(4), 0);

    s1.clear();
    EXPECT_EQ(s1.count(0), 0);
    EXPECT_EQ(s1.count(1), 0);
    EXPECT_EQ(s1.count(2), 0);
    EXPECT_EQ(s1.count(3), 0);
    EXPECT_EQ(s1.count(4), 0);
    EXPECT_EQ(s1.count(5), 0);
}

struct X
{
    auto operator<=>(const X &) const { return std::weak_ordering::equivalent; }
};

TEST(SetEqualRange, Test1)
{
    mstd::set<X> s;
    X x;
    (void)s.equal_range(x);
    const mstd::set<X> &cs = s;
    (void)cs.equal_range(x);
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

struct PathPointLess
{
    auto operator()(const PathPoint &lhs, const PathPoint &rhs) const
    {
        return lhs.getType() <=> rhs.getType();
    }
};

TEST(SetEmplace, Test1)
{
    typedef mstd::set<PathPoint, PathPointLess> Set;
    Set s;

    std::vector<double> coord1 = {0.0, 1.0, 2.0};

    auto ret = s.emplace('a', coord1);
    EXPECT_TRUE(ret.second);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(ret.first->getType(), 'a');

    coord1[0] = 3.0;
    ret = s.emplace('a', coord1);
    EXPECT_FALSE(ret.second);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(ret.first->getType(), 'a');
    EXPECT_EQ(ret.first->getCoords()[0], 0.0);

    auto it = s.emplace_hint(s.begin(), 'b', coord1);
    EXPECT_NE(it, s.end());
    EXPECT_EQ(it->getType(), 'b');
    EXPECT_EQ(it->getCoords()[0], 3.0);

    double *px = &coord1[0];
    ret = s.emplace('c', std::move(coord1));
    EXPECT_TRUE(ret.second);
    EXPECT_EQ(ret.first->getType(), 'c');
    EXPECT_EQ(&(ret.first->getCoords()[0]), px);
}

TEST(SetInsert, Test1)
{
    using namespace std;

    set<int> s0, s1;
    set<int>::iterator iter1;

    s0.insert(1);
    s1.insert(s1.end(), 1);
    EXPECT_EQ(s0, s1);

    s0.insert(3);
    s1.insert(s1.begin(), 3);
    EXPECT_EQ(s0, s1);

    s0.insert(4);
    iter1 = s1.insert(s1.end(), 4);
    EXPECT_EQ(s0, s1);

    s0.insert(6);
    s1.insert(iter1, 6);
    EXPECT_EQ(s0, s1);

    s0.insert(2);
    s1.insert(s1.begin(), 2);
    EXPECT_EQ(s0, s1);

    s0.insert(7);
    s1.insert(s1.end(), 7);
    EXPECT_EQ(s0, s1);

    s0.insert(5);
    s1.insert(s1.find(4), 5);
    EXPECT_EQ(s0, s1);

    s0.insert(0);
    s1.insert(s1.end(), 0);
    EXPECT_EQ(s0, s1);

    s0.insert(8);
    s1.insert(s1.find(3), 8);
    EXPECT_EQ(s0, s1);

    s0.insert(9);
    s1.insert(s1.end(), 9);
    EXPECT_EQ(s0, s1);

    s0.insert(10);
    s1.insert(s1.begin(), 10);
    EXPECT_EQ(s0, s1);
}

TEST(SetInsert, Test2)
{
    typedef mstd::set<rvalstruct> Set;
    Set s;
    EXPECT_TRUE(s.empty());

    std::pair<Set::iterator, bool> p = s.insert(rvalstruct(1));
    EXPECT_TRUE(p.second);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(std::distance(s.begin(), s.end()), 1);
    EXPECT_EQ(p.first, s.begin());
    EXPECT_EQ((*p.first).val, 1);
}

TEST(SetInsert, Test3)
{
    typedef mstd::set<rvalstruct> Set;
    Set s;
    EXPECT_TRUE(s.empty());

    std::pair<Set::iterator, bool> p1 = s.insert(rvalstruct(2));
    std::pair<Set::iterator, bool> p2 = s.insert(rvalstruct(2));
    EXPECT_TRUE(p1.second);
    EXPECT_FALSE(p2.second);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(p1.first, p2.first);
    EXPECT_EQ((*p1.first).val, 2);
}

TEST(SetInsert, Test4)
{
    typedef mstd::set<rvalstruct> Set;
    Set s;
    EXPECT_TRUE(s.empty());

    Set::iterator p = s.insert(s.begin(), rvalstruct(1));
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(std::distance(s.begin(), s.end()), 1);
    EXPECT_EQ(p, s.begin());
    EXPECT_EQ(p->val, 1);
}

TEST(SetInsert, Test5)
{
    typedef mstd::set<rvalstruct> Set;
    Set s;
    EXPECT_TRUE(s.empty());

    Set::iterator p1 = s.insert(s.begin(), rvalstruct(2));
    Set::iterator p2 = s.insert(p1, rvalstruct(2));
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(p1, p2);
    EXPECT_EQ(p1->val, 2);
}

TEST(SetOperations, Test1)
{
    mstd::set<int> s0;
    typedef mstd::set<int>::iterator iterator;
    typedef mstd::set<int>::const_iterator const_iterator;
    typedef std::pair<iterator, bool> insert_return_type;

    std::ranges::input_range auto pp0 = s0.equal_range(1);
    EXPECT_EQ(s0.count(1), 0);
    EXPECT_EQ(pp0.begin(), s0.end());
    EXPECT_EQ(pp0.end(), s0.end());

    insert_return_type irt0 = s0.insert(1);
    insert_return_type irt1 = s0.insert(2);
    insert_return_type irt2 = s0.insert(3);

    pp0 = s0.equal_range(2);
    EXPECT_EQ(s0.count(2), 1);
    EXPECT_EQ(*pp0.begin(), 2);
    EXPECT_EQ(*pp0.end(), 3);
    EXPECT_EQ(pp0.begin(), irt1.first);
    EXPECT_EQ(--pp0.begin(), irt0.first);
    EXPECT_EQ(pp0.end(), irt2.first);

    s0.insert(3);
    insert_return_type irt3 = s0.insert(3);
    insert_return_type irt4 = s0.insert(4);

    pp0 = s0.equal_range(3);
    EXPECT_EQ(s0.count(3), 1);
    EXPECT_EQ(*pp0.begin(), 3);
    EXPECT_EQ(*pp0.end(), 4);
    EXPECT_EQ(pp0.begin(), irt2.first);
    EXPECT_EQ(--pp0.begin(), irt1.first);
    EXPECT_EQ(pp0.end(), irt4.first);

    insert_return_type irt5 = s0.insert(0);
    s0.insert(1);
    s0.insert(1);
    s0.insert(1);

    pp0 = s0.equal_range(1);
    EXPECT_EQ(s0.count(1), 1);
    EXPECT_EQ(*pp0.begin(), 1);
    EXPECT_EQ(*pp0.end(), 2);
    EXPECT_EQ(pp0.begin(), irt0.first);
    EXPECT_EQ(--pp0.begin(), irt5.first);
    EXPECT_EQ(pp0.end(), irt1.first);

    insert_return_type irt6 = s0.insert(5);
    s0.insert(5);
    s0.insert(5);

    pp0 = s0.equal_range(5);
    EXPECT_EQ(s0.count(5), 1);
    EXPECT_EQ(*pp0.begin(), 5);
    EXPECT_EQ(pp0.begin(), irt6.first);
    EXPECT_EQ(--pp0.begin(), irt4.first);
    EXPECT_EQ(pp0.end(), s0.end());

    s0.insert(4);
    s0.insert(4);
    s0.insert(4);

    pp0 = s0.equal_range(4);
    EXPECT_EQ(s0.count(4), 1);
    EXPECT_EQ(*pp0.begin(), 4);
    EXPECT_EQ(*pp0.end(), 5);
    EXPECT_EQ(pp0.begin(), irt4.first);
    EXPECT_EQ(--pp0.begin(), irt3.first);
    EXPECT_EQ(pp0.end(), irt6.first);

    s0.insert(0);
    insert_return_type irt7 = s0.insert(0);
    s0.insert(1);

    pp0 = s0.equal_range(0);
    EXPECT_EQ(s0.count(0), 1);
    EXPECT_EQ(*pp0.begin(), 0);
    EXPECT_EQ(*pp0.end(), 1);
    EXPECT_EQ(pp0.begin(), irt5.first);
    EXPECT_EQ(pp0.begin(), s0.begin());
    EXPECT_EQ(pp0.end(), irt0.first);

    const mstd::set<int> &s1 = s0;
    auto pp1 = s1.equal_range(1);
    EXPECT_EQ(s1.count(1), 1);
    EXPECT_EQ(*pp1.begin(), 1);
    EXPECT_EQ(*pp1.end(), 2);
    EXPECT_EQ(pp1.begin(), irt0.first);
    EXPECT_EQ(--pp1.begin(), irt7.first);
    EXPECT_EQ(pp1.end(), irt1.first);
}

struct Cmp
{
    auto operator()(int i, long l) const { return i <=> l; }
    auto operator()(long l, int i) const { return l <=> i; }
    auto operator()(int i, int j) const
    {
        ++count;
        return i <=> j;
    }

    static int count;
};

int Cmp::count = 0;

// using test_type = mstd::set<int, Cmp>;
//
// test_type x{1, 3, 5};
// const test_type &cx = x;

TEST(SetOperations, Test2)
{
    Cmp::count = 0;

    using test_type = mstd::set<int, Cmp>;

    test_type x{1, 3, 5};
    const test_type &cx = x;

    auto it = x.find(1L);
    EXPECT_NE(it, x.end());
    EXPECT_EQ(*it, 1);
    it = x.find(2L);
    EXPECT_EQ(it, x.end());

    auto cit = cx.find(3L);
    EXPECT_NE(cit, cx.end());
    EXPECT_EQ(*cit, 3);
    cit = cx.find(2L);
    EXPECT_EQ(cit, cx.end());

    EXPECT_EQ(Cmp::count, 2);

    static_assert(std::is_same<decltype(it), test_type::iterator>::value,
                  "find returns iterator");
    static_assert(std::is_same<decltype(cit), test_type::const_iterator>::value,
                  "const find returns const_iterator");
}

TEST(SetOperations, Test3)
{
    Cmp::count = 0;

    using test_type = mstd::set<int, Cmp>;

    test_type x{1, 3, 5};
    const test_type &cx = x;

    auto n = x.count(1L);
    EXPECT_EQ(n, 1);
    n = x.count(2L);
    EXPECT_EQ(n, 0);

    auto cn = cx.count(3L);
    EXPECT_EQ(cn, 1);
    cn = cx.count(2L);
    EXPECT_EQ(cn, 0);

    EXPECT_EQ(Cmp::count, 2);
}

TEST(SetOperations, Test4)
{
    Cmp::count = 0;

    using test_type = mstd::set<int, Cmp>;

    test_type x{1, 3, 5};
    const test_type &cx = x;

    auto it = x.lower_bound(1L);
    EXPECT_NE(it, x.end());
    EXPECT_EQ(*it, 1);
    it = x.lower_bound(2L);
    EXPECT_NE(it, x.end());
    EXPECT_EQ(*it, 3);

    auto cit = cx.lower_bound(1L);
    EXPECT_NE(cit, cx.end());
    EXPECT_EQ(*cit, 1);
    cit = cx.lower_bound(2L);
    EXPECT_NE(cit, cx.end());
    EXPECT_EQ(*cit, 3);

    EXPECT_EQ(Cmp::count, 2);

    static_assert(std::is_same<decltype(it), test_type::iterator>::value,
                  "lower_bound returns iterator");
    static_assert(std::is_same<decltype(cit), test_type::const_iterator>::value,
                  "const lower_bound returns const_iterator");
}

TEST(SetOperations, Test5)
{
    Cmp::count = 0;

    using test_type = mstd::set<int, Cmp>;

    test_type x{1, 3, 5};
    const test_type &cx = x;

    auto it = x.upper_bound(1L);
    EXPECT_NE(it, x.end());
    EXPECT_EQ(*it, 3);
    it = x.upper_bound(5L);
    EXPECT_EQ(it, x.end());

    auto cit = cx.upper_bound(1L);
    EXPECT_NE(cit, cx.end());
    EXPECT_EQ(*cit, 3);
    cit = cx.upper_bound(5L);
    EXPECT_EQ(cit, cx.end());

    EXPECT_EQ(Cmp::count, 2);

    static_assert(std::is_same<decltype(it), test_type::iterator>::value,
                  "upper_bound returns iterator");
    static_assert(std::is_same<decltype(cit), test_type::const_iterator>::value,
                  "const upper_bound returns const_iterator");
}

TEST(SetOperations, Test6)
{
    Cmp::count = 0;

    using test_type = mstd::set<int, Cmp>;

    test_type x{1, 3, 5};
    const test_type &cx = x;

    auto r = x.equal_range(1L);
    EXPECT_FALSE(r.empty());
    EXPECT_EQ(*r.begin(), 1);
    r = x.equal_range(2L);
    EXPECT_TRUE(r.empty());
    EXPECT_NE(r.begin(), x.end());

    auto cr = cx.equal_range(1L);
    EXPECT_FALSE(cr.empty());
    EXPECT_EQ(*cr.begin(), 1);
    cr = cx.equal_range(2L);
    EXPECT_TRUE(cr.empty());
    EXPECT_NE(cr.begin(), cx.end());

    EXPECT_EQ(Cmp::count, 2);

    static_assert(std::ranges::range<decltype(r)>,
                  "equal_range returns a range");
    static_assert(std::ranges::range<decltype(cr)>,
                  "const equal_range returns a range");
}

TEST(SetOperations, Test7)
{
    struct I
    {
        int i;
        operator int() const { return i; }
    };

    mstd::set<int> s;
    I i = {};
    [[maybe_unused]] auto iter = s.find(i);
}

TEST(SetOperations, Test8)
{
    struct C
    {
        auto operator()(int l, int r) const { return l <=> r; }

        struct Partition
        {
        };

        std::weak_ordering operator()(int l, Partition) const
        {
            if (l <= 4 && l >= 2)
            {
                return std::weak_ordering::equivalent;
            }
            return l <=> 3;
        }
        std::weak_ordering operator()(Partition, int r) const
        {
            if (r <= 4 && r >= 2)
            {
                return std::weak_ordering::equivalent;
            }
            return 3 <=> r;
        }
    };

    mstd::set<int, C> s{1, 2, 3, 4, 5};

    auto n = s.count(C::Partition{});
    EXPECT_EQ(n, 3);
}
