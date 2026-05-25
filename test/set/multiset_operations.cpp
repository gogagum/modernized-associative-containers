#include <gtest/gtest.h>

#include <concepts>
#include <ranges>
#include <rval_struct.hpp>
#include <test_values.hpp>

#include <set.hpp>

namespace mstd {

// Associative container iterators are not random access
static_assert(!std::totally_ordered<multiset<int>::iterator>);
static_assert(!std::three_way_comparable<multiset<int>::iterator>);

static_assert(requires(multiset<int>& x) {
    { x.find(1L) } -> std::same_as<multiset<int>::iterator>;
    { x.upper_bound(1L) } -> std::same_as<multiset<int>::iterator>;
    { x.lower_bound(1L) } -> std::same_as<multiset<int>::iterator>;
    { x.equal_range(1L) } -> std::ranges::input_range;
});

static_assert(requires(const multiset<int>& x) {
    { x.find(1L) } -> std::same_as<multiset<int>::const_iterator>;
    { x.upper_bound(1L) } -> std::same_as<multiset<int>::const_iterator>;
    { x.lower_bound(1L) } -> std::same_as<multiset<int>::const_iterator>;
    { x.equal_range(1L) } -> std::ranges::input_range;
});

TEST(MultisetEmplaceTest, Test2) {
    multiset<test::aggressive_aggregate> x;
    auto                                 it = x.emplace(1, 2);
    EXPECT_EQ(it->a, 1);
    EXPECT_EQ(it->b, 2);
    it = x.emplace(2);
    EXPECT_EQ(it->a, 2);
    EXPECT_EQ(it->b, 0);
    it = x.emplace();
    EXPECT_EQ(it->a, 0);
    EXPECT_EQ(it->b, 0);
}

TEST(MultisetEmplaceTest, TestHint) {
    multiset<test::aggressive_aggregate> x;
    auto                                 it = x.emplace_hint(x.begin(), 3, 2);
    EXPECT_EQ(it->a, 3);
    EXPECT_EQ(it->b, 2);
    it = x.emplace_hint(x.begin(), 4);
    EXPECT_EQ(it->a, 4);
    EXPECT_EQ(it->b, 0);
    it = x.emplace_hint(x.begin());
    EXPECT_EQ(it->a, 0);
    EXPECT_EQ(it->b, 0);
}

TEST(MultisetEmplaceTest, Test1) {
    auto ms     = multiset<test::PathPoint, test::PathPointCmp>{};
    auto coord1 = std::vector{0.0, 1.0, 2.0};

    auto it = ms.emplace('a', coord1);
    EXPECT_EQ(ms.size(), 1);
    EXPECT_EQ(it->getType(), 'a');

    coord1[0] = 3.0;
    it        = ms.emplace('a', coord1);
    EXPECT_EQ(ms.size(), 2);
    EXPECT_EQ(it->getType(), 'a');
    EXPECT_EQ(it->getCoords()[0], 3.0);

    it = ms.emplace_hint(ms.begin(), 'b', coord1);
    EXPECT_NE(it, ms.end());
    EXPECT_EQ(it->getType(), 'b');
    EXPECT_EQ(it->getCoords()[0], 3.0);

    double* px = &coord1[0];
    it         = ms.emplace('c', std::move(coord1));
    EXPECT_EQ(ms.size(), 4);
    EXPECT_EQ(it->getType(), 'c');
    EXPECT_EQ(&(it->getCoords()[0]), px);
}

TEST(MultisetInsertTest, Test1) {
    multiset<int> ms0, ms1;

    ms0.insert(1);
    ms1.insert(ms1.end(), 1);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(3);
    ms1.insert(ms1.begin(), 3);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(4);
    auto iter1 = ms1.insert(ms1.end(), 4);
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

TEST(MultisetInsertTest, Test2) {
    multiset<int> ms0, ms1;

    ms0.insert(1);
    ms1.insert(ms1.end(), 1);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(3);
    ms1.insert(ms1.begin(), 3);
    EXPECT_EQ(ms0, ms1);

    ms0.insert(4);
    auto iter1 = ms1.insert(ms1.end(), 4);
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

TEST(MultisetInsertTest, TestRvalStruct1) {
    auto s = multiset<test::rvalstruct>{};
    EXPECT_TRUE(s.empty());

    auto i = s.insert(test::rvalstruct(1));
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(std::distance(s.begin(), s.end()), 1);
    EXPECT_EQ(i, s.begin());
    EXPECT_EQ((*i).val, 1);
}

TEST(MultisetInsertTest, TestRvalStruct2) {
    auto s = multiset<test::rvalstruct>{};
    EXPECT_TRUE(s.empty());

    s.insert(test::rvalstruct(2));
    auto i = s.insert(test::rvalstruct(2));
    EXPECT_EQ(s.size(), 2);
    EXPECT_EQ(std::distance(s.begin(), s.end()), 2);
    EXPECT_EQ((*i).val, 2);

    auto i2 = s.begin();
    ++i2;
    EXPECT_EQ(i, i2);
    EXPECT_EQ(s.begin()->val, 2);
    EXPECT_EQ(i->val, 2);
}

TEST(MultisetOperationsTest, Test1) {
    multiset<int> ms0;

    {
        auto [begin, end] = ms0.equal_range(1);
        EXPECT_EQ(ms0.count(1), 0);
        EXPECT_EQ(begin, ms0.end());
        EXPECT_EQ(end, ms0.end());
    }

    auto iter0 = ms0.insert(1);
    auto iter1 = ms0.insert(2);
    auto iter2 = ms0.insert(3);

    {
        auto [begin, end] = ms0.equal_range(2);
        EXPECT_EQ(ms0.count(2), 1);
        EXPECT_EQ(*begin, 2);
        EXPECT_EQ(*end, 3);
        EXPECT_EQ(begin, iter1);
        EXPECT_EQ(--begin, iter0);
        EXPECT_EQ(end, iter2);
    }

    ms0.insert(3);
    auto iter3 = ms0.insert(3);
    auto iter4 = ms0.insert(4);

    {
        auto [begin, end] = ms0.equal_range(3);
        EXPECT_EQ(ms0.count(3), 3);
        EXPECT_EQ(*begin, 3);
        EXPECT_EQ(*end, 4);
        EXPECT_EQ(begin, iter2);
        EXPECT_EQ(--begin, iter1);
        EXPECT_EQ(end, iter4);
    }

    auto iter5 = ms0.insert(0);
    ms0.insert(1);
    ms0.insert(1);
    ms0.insert(1);

    {
        auto [begin, end] = ms0.equal_range(1);
        EXPECT_EQ(ms0.count(1), 4);
        EXPECT_EQ(*begin, 1);
        EXPECT_EQ(*end, 2);
        EXPECT_EQ(begin, iter0);
        EXPECT_EQ(--begin, iter5);
        EXPECT_EQ(end, iter1);
    }

    auto iter6 = ms0.insert(5);
    ms0.insert(5);
    ms0.insert(5);

    {
        auto [begin, end] = ms0.equal_range(5);
        EXPECT_EQ(ms0.count(5), 3);
        EXPECT_EQ(*begin, 5);
        EXPECT_EQ(begin, iter6);
        EXPECT_EQ(--begin, iter4);
        EXPECT_EQ(end, ms0.end());
    }

    ms0.insert(4);
    ms0.insert(4);
    ms0.insert(4);

    {
        auto [begin, end] = ms0.equal_range(4);
        EXPECT_EQ(ms0.count(4), 4);
        EXPECT_EQ(*begin, 4);
        EXPECT_EQ(*end, 5);
        EXPECT_EQ(begin, iter4);
        EXPECT_EQ(--begin, iter3);
        EXPECT_EQ(end, iter6);
    }

    ms0.insert(0);
    auto iter7 = ms0.insert(0);
    ms0.insert(1);

    {
        auto [begin, end] = ms0.equal_range(0);
        EXPECT_EQ(ms0.count(0), 3);
        EXPECT_EQ(*begin, 0);
        EXPECT_EQ(*end, 1);
        EXPECT_EQ(begin, iter5);
        EXPECT_EQ(begin, ms0.begin());
        EXPECT_EQ(end, iter0);
    }

    {
        auto [begin, end] = std::as_const(ms0).equal_range(1);
        EXPECT_EQ(ms0.count(1), 5);
        EXPECT_EQ(*begin, 1);
        EXPECT_EQ(*end, 2);
        EXPECT_EQ(begin, iter0);
        EXPECT_EQ(--begin, iter7);
        EXPECT_EQ(end, iter1);
    }
}

TEST(MultisetOperationsTest, Test2) {
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

TEST(MultisetOperationsTest, Test3) {
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

TEST(MultisetOperationsTest, Test4) {
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

struct Cmp {
    auto operator()(int i, long l) const {
        return i <=> l;
    }
    auto operator()(long l, int i) const {
        return l <=> i;
    }
    auto operator()(int i, int j) const {
        ++count;
        return i <=> j;
    }

    static int count;
};

int Cmp::count = 0;

TEST(MultisetOperations, Test2) {
    Cmp::count = 0;

    auto        x  = multiset<int, Cmp>{1, 3, 5};
    const auto& cx = x;

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

    EXPECT_EQ(Cmp::count, 5);
}

TEST(MultisetOperations, Test3) {
    Cmp::count = 0;

    auto x = multiset<int, Cmp>{1, 3, 5};

    auto n = x.count(1L);
    EXPECT_EQ(n, 1);
    n = x.count(2L);
    EXPECT_EQ(n, 0);

    auto cn = std::as_const(x).count(3L);
    EXPECT_EQ(cn, 1);
    cn = std::as_const(x).count(2L);
    EXPECT_EQ(cn, 0);

    EXPECT_EQ(Cmp::count, 5);
}

TEST(MultisetOperations, Test4) {
    Cmp::count = 0;

    auto x = multiset<int, Cmp>{1, 3, 5};

    auto it = x.lower_bound(1L);
    EXPECT_NE(it, x.end());
    EXPECT_EQ(*it, 1);
    it = x.lower_bound(2L);
    EXPECT_NE(it, x.end());
    EXPECT_EQ(*it, 3);

    auto cit = std::as_const(x).lower_bound(1L);
    EXPECT_NE(cit, std::as_const(x).end());
    EXPECT_EQ(*cit, 1);
    cit = std::as_const(x).lower_bound(2L);
    EXPECT_NE(cit, std::as_const(x).end());
    EXPECT_EQ(*cit, 3);

    EXPECT_EQ(Cmp::count, 5);
}

TEST(MultisetOperations, Test5) {
    Cmp::count = 0;

    auto        x  = multiset<int, Cmp>{1, 3, 5};
    const auto& cx = x;

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

    EXPECT_EQ(Cmp::count, 5);
}

TEST(MultisetOperations, Test6) {
    Cmp::count = 0;

    auto x = multiset<int, Cmp>{1, 3, 5};

    auto r = x.equal_range(1L);
    EXPECT_FALSE(r.empty());
    EXPECT_EQ(*r.begin(), 1);
    r = x.equal_range(2L);
    EXPECT_TRUE(r.empty());
    EXPECT_NE(r.begin(), x.end());

    auto cr = std::as_const(x).equal_range(1L);
    EXPECT_FALSE(cr.empty());
    EXPECT_EQ(*cr.begin(), 1);
    cr = std::as_const(x).equal_range(2L);
    EXPECT_TRUE(cr.empty());
    EXPECT_NE(cr.begin(), std::as_const(x).end());

    EXPECT_EQ(Cmp::count, 5);
}

TEST(MultisetOperations, Test7) {
    multiset<int>         s;
    test::I               i    = {};
    [[maybe_unused]] auto iter = s.find(i);
}

TEST(MultisetOperations, Test8) {
    auto s = multiset<int, test::WithPartition>{1, 2, 3, 3, 4, 5};

    auto n = s.count(test::WithPartition::Partition{});
    EXPECT_EQ(n, 4);
}

} // namespace mstd
