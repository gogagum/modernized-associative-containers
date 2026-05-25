#include <gtest/gtest.h>

#include <concepts>
#include <ranges>
#include <rval_struct.hpp>
#include <test_values.hpp>

#include <set.hpp>

namespace mstd {

// Associative container iterators are not random access
static_assert(!std::totally_ordered<set<int>::iterator>);
static_assert(!std::three_way_comparable<set<int>::iterator>);

static_assert(requires(set<int>& x) {
    { x.find(1L) } -> std::same_as<set<int>::iterator>;
    { x.upper_bound(1L) } -> std::same_as<set<int>::iterator>;
    { x.lower_bound(1L) } -> std::same_as<set<int>::iterator>;
    { x.equal_range(1L) } -> std::ranges::input_range;
});

static_assert(requires(const set<int>& x) {
    { x.find(1L) } -> std::same_as<set<int>::const_iterator>;
    { x.upper_bound(1L) } -> std::same_as<set<int>::const_iterator>;
    { x.lower_bound(1L) } -> std::same_as<set<int>::const_iterator>;
    { x.equal_range(1L) } -> std::ranges::input_range;
});

TEST(SetEmplaceTest, Test2) {
    set<test::aggressive_aggregate> x;
    auto [it, was_emplaced] = x.emplace(1, 2);
    EXPECT_EQ(it->a, 1);
    EXPECT_EQ(it->b, 2);
    it = x.emplace(2).position;
    EXPECT_EQ(it->a, 2);
    EXPECT_EQ(it->b, 0);
    it = x.emplace().position;
    EXPECT_EQ(it->a, 0);
    EXPECT_EQ(it->b, 0);
}

TEST(SetEmplaceTest, TestHint) {
    set<test::aggressive_aggregate> x;
    auto                            it = x.emplace_hint(x.begin(), 3, 2);
    EXPECT_EQ(it->a, 3);
    EXPECT_EQ(it->b, 2);
    it = x.emplace_hint(x.begin(), 4);
    EXPECT_EQ(it->a, 4);
    EXPECT_EQ(it->b, 0);
    it = x.emplace_hint(x.begin());
    EXPECT_EQ(it->a, 0);
    EXPECT_EQ(it->b, 0);
}

TEST(SetContains, Test1) {
    set<int> m;
    EXPECT_FALSE(m.contains(0));
    EXPECT_FALSE(m.contains(1));
    m.insert(0);
    EXPECT_TRUE(m.contains(0));
    EXPECT_FALSE(m.contains(1));
    m.insert(1);
    EXPECT_TRUE(m.contains(0));
    EXPECT_TRUE(m.contains(1));
}

TEST(SetContains, Test2) {
    set<int> m;
    EXPECT_FALSE(m.contains(test::Zero{}));
    EXPECT_FALSE(m.contains(test::One{}));
    m.insert(0);
    EXPECT_TRUE(m.contains(test::Zero{}));
    EXPECT_FALSE(m.contains(test::One{}));
    m.insert(1);
    EXPECT_TRUE(m.contains(test::Zero{}));
    EXPECT_TRUE(m.contains(test::One{}));
}

TEST(SetCount, Test1) {
    set<int> s0;
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

    set<int> s1(s0);
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

TEST(SetEqualRange, Test1) {
    set<test::AllEqual> s;
    test::AllEqual      x;
    std::ignore = s.equal_range(x);
    std::ignore = std::as_const(s).equal_range(x);
}

TEST(SetEmplace, Test1) {
    auto s      = set<test::PathPoint, test::PathPointCmp>{};
    auto coord1 = std::vector{0.0, 1.0, 2.0};

    auto ret = s.emplace('a', coord1);
    EXPECT_TRUE(ret.inserted);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(ret.position->getType(), 'a');

    coord1[0] = 3.0;
    ret       = s.emplace('a', coord1);
    EXPECT_FALSE(ret.inserted);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(ret.position->getType(), 'a');
    EXPECT_EQ(ret.position->getCoords()[0], 0.0);

    auto it = s.emplace_hint(s.begin(), 'b', coord1);
    EXPECT_NE(it, s.end());
    EXPECT_EQ(it->getType(), 'b');
    EXPECT_EQ(it->getCoords()[0], 3.0);

    double* px = &coord1[0];
    ret        = s.emplace('c', std::move(coord1));
    EXPECT_TRUE(ret.inserted);
    EXPECT_EQ(ret.position->getType(), 'c');
    EXPECT_EQ(&(ret.position->getCoords()[0]), px);
}

TEST(SetInsert, Test1) {
    set<int> s0, s1;

    s0.insert(1);
    s1.insert(s1.end(), 1);
    EXPECT_EQ(s0, s1);

    s0.insert(3);
    s1.insert(s1.begin(), 3);
    EXPECT_EQ(s0, s1);

    s0.insert(4);
    auto iter1 = s1.insert(s1.end(), 4);
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

TEST(SetInsert, Test2) {
    auto s = set<test::rvalstruct>{};
    EXPECT_TRUE(s.empty());

    auto p = s.insert(test::rvalstruct(1));
    EXPECT_TRUE(p.inserted);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(std::distance(s.begin(), s.end()), 1);
    EXPECT_EQ(p.position, s.begin());
    EXPECT_EQ((*p.position).val, 1);
}

TEST(SetInsert, Test3) {
    auto s = set<test::rvalstruct>{};
    EXPECT_TRUE(s.empty());

    auto p1 = s.insert(test::rvalstruct(2));
    auto p2 = s.insert(test::rvalstruct(2));
    EXPECT_TRUE(p1.inserted);
    EXPECT_FALSE(p2.inserted);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(p1.position, p2.position);
    EXPECT_EQ((*p1.position).val, 2);
}

TEST(SetInsert, Test4) {
    auto s = set<test::rvalstruct>{};
    EXPECT_TRUE(s.empty());

    auto p = s.insert(s.begin(), test::rvalstruct(1));
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(std::distance(s.begin(), s.end()), 1);
    EXPECT_EQ(p, s.begin());
    EXPECT_EQ(p->val, 1);
}

TEST(SetInsert, Test5) {
    auto s = set<test::rvalstruct>{};
    EXPECT_TRUE(s.empty());

    auto p1 = s.insert(s.begin(), test::rvalstruct(2));
    auto p2 = s.insert(p1, test::rvalstruct(2));
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(p1, p2);
    EXPECT_EQ(p1->val, 2);
}

TEST(SetInsertTest, TestRvalStruct1) {
    auto s = set<test::rvalstruct>{};
    EXPECT_TRUE(s.empty());

    auto [i, was_inserted] = s.insert(test::rvalstruct(1));
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(std::distance(s.begin(), s.end()), 1);
    EXPECT_EQ(i, s.begin());
    EXPECT_EQ(i->val, 1);
}

TEST(SetInsertTest, TestRvalStruct2) {
    auto s = set<test::rvalstruct>{};
    EXPECT_TRUE(s.empty());

    s.insert(test::rvalstruct(2));
    auto [i, wa_inserted] = s.insert(test::rvalstruct(2));
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(std::distance(s.begin(), s.end()), 1);
    EXPECT_EQ(i->val, 2);

    auto i2 = s.begin();
    ++i2;
    EXPECT_EQ(s.begin()->val, 2);
    EXPECT_EQ(i->val, 2);
}

TEST(SetOperations, Test1) {
    auto s0 = set<int>{};

    std::ranges::input_range auto pp0 = s0.equal_range(1);
    EXPECT_EQ(s0.count(1), 0);
    EXPECT_EQ(pp0.begin(), s0.end());
    EXPECT_EQ(pp0.end(), s0.end());

    auto irt0 = s0.insert(1);
    auto irt1 = s0.insert(2);
    auto irt2 = s0.insert(3);

    pp0 = s0.equal_range(2);
    EXPECT_EQ(s0.count(2), 1);
    EXPECT_EQ(*pp0.begin(), 2);
    EXPECT_EQ(*pp0.end(), 3);
    EXPECT_EQ(pp0.begin(), irt1.position);
    EXPECT_EQ(--pp0.begin(), irt0.position);
    EXPECT_EQ(pp0.end(), irt2.position);

    s0.insert(3);
    auto irt3 = s0.insert(3);
    auto irt4 = s0.insert(4);

    pp0 = s0.equal_range(3);
    EXPECT_EQ(s0.count(3), 1);
    EXPECT_EQ(*pp0.begin(), 3);
    EXPECT_EQ(*pp0.end(), 4);
    EXPECT_EQ(pp0.begin(), irt2.position);
    EXPECT_EQ(--pp0.begin(), irt1.position);
    EXPECT_EQ(pp0.end(), irt4.position);

    auto irt5 = s0.insert(0);
    s0.insert(1);
    s0.insert(1);
    s0.insert(1);

    pp0 = s0.equal_range(1);
    EXPECT_EQ(s0.count(1), 1);
    EXPECT_EQ(*pp0.begin(), 1);
    EXPECT_EQ(*pp0.end(), 2);
    EXPECT_EQ(pp0.begin(), irt0.position);
    EXPECT_EQ(--pp0.begin(), irt5.position);
    EXPECT_EQ(pp0.end(), irt1.position);

    auto irt6 = s0.insert(5);
    s0.insert(5);
    s0.insert(5);

    pp0 = s0.equal_range(5);
    EXPECT_EQ(s0.count(5), 1);
    EXPECT_EQ(*pp0.begin(), 5);
    EXPECT_EQ(pp0.begin(), irt6.position);
    EXPECT_EQ(--pp0.begin(), irt4.position);
    EXPECT_EQ(pp0.end(), s0.end());

    s0.insert(4);
    s0.insert(4);
    s0.insert(4);

    pp0 = s0.equal_range(4);
    EXPECT_EQ(s0.count(4), 1);
    EXPECT_EQ(*pp0.begin(), 4);
    EXPECT_EQ(*pp0.end(), 5);
    EXPECT_EQ(pp0.begin(), irt4.position);
    EXPECT_EQ(--pp0.begin(), irt3.position);
    EXPECT_EQ(pp0.end(), irt6.position);

    s0.insert(0);
    auto irt7 = s0.insert(0);
    s0.insert(1);

    pp0 = s0.equal_range(0);
    EXPECT_EQ(s0.count(0), 1);
    EXPECT_EQ(*pp0.begin(), 0);
    EXPECT_EQ(*pp0.end(), 1);
    EXPECT_EQ(pp0.begin(), irt5.position);
    EXPECT_EQ(pp0.begin(), s0.begin());
    EXPECT_EQ(pp0.end(), irt0.position);

    const set<int>& s1  = s0;
    auto            pp1 = s1.equal_range(1);
    EXPECT_EQ(s1.count(1), 1);
    EXPECT_EQ(*pp1.begin(), 1);
    EXPECT_EQ(*pp1.end(), 2);
    EXPECT_EQ(pp1.begin(), irt0.position);
    EXPECT_EQ(--pp1.begin(), irt7.position);
    EXPECT_EQ(pp1.end(), irt1.position);
}

TEST(SetOperationsTest, Test2) {
    auto m = set<int>{};
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

TEST(SetOperationsTest, Test3) {
    auto m = set<int>{};
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

TEST(SetOperationsTest, Test4) {
    set<int> ms0;
    EXPECT_EQ(ms0.count(0), 0);
    EXPECT_EQ(ms0.count(1), 0);

    ms0.insert(1);
    EXPECT_EQ(ms0.count(0), 0);
    EXPECT_EQ(ms0.count(1), 1);

    ms0.insert(1);
    EXPECT_EQ(ms0.count(0), 0);
    EXPECT_EQ(ms0.count(1), 1);

    ms0.insert(2);
    EXPECT_EQ(ms0.count(2), 1);

    ms0.insert(3);
    ms0.insert(3);
    ms0.insert(3);
    EXPECT_EQ(ms0.count(3), 1);

    ms0.erase(2);
    EXPECT_EQ(ms0.count(2), 0);

    ms0.erase(0);
    EXPECT_EQ(ms0.count(0), 0);

    set<int> ms1(ms0);
    EXPECT_EQ(ms1.count(0), 0);
    EXPECT_EQ(ms1.count(1), 1);
    EXPECT_EQ(ms1.count(2), 0);
    EXPECT_EQ(ms1.count(3), 1);

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
    EXPECT_EQ(ms1.count(5), 1);

    ms1.erase(1);
    EXPECT_EQ(ms1.count(1), 0);

    ms1.erase(ms1.find(5));
    EXPECT_EQ(ms1.count(5), 0);

    ms1.insert(1);
    ms1.insert(1);
    EXPECT_EQ(ms1.count(1), 1);

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

TEST(SetOperations, Test2) {
    Cmp::count = 0;

    auto x = set<int, Cmp>{1, 3, 5};

    auto it = x.find(1L);
    EXPECT_NE(it, x.end());
    EXPECT_EQ(*it, 1);
    it = x.find(2L);
    EXPECT_EQ(it, x.end());

    auto cit = std::as_const(x).find(3L);
    EXPECT_NE(cit, std::as_const(x).end());
    EXPECT_EQ(*cit, 3);
    cit = std::as_const(x).find(2L);
    EXPECT_EQ(cit, std::as_const(x).end());

    EXPECT_EQ(Cmp::count, 2);
}

TEST(SetOperations, Test3) {
    Cmp::count = 0;

    auto x = set<int, Cmp>{1, 3, 5};

    auto n = x.count(1L);
    EXPECT_EQ(n, 1);
    n = x.count(2L);
    EXPECT_EQ(n, 0);

    auto cn = std::as_const(x).count(3L);
    EXPECT_EQ(cn, 1);
    cn = std::as_const(x).count(2L);
    EXPECT_EQ(cn, 0);

    EXPECT_EQ(Cmp::count, 2);
}

TEST(SetOperations, Test4) {
    Cmp::count = 0;

    auto x = set<int, Cmp>{1, 3, 5};

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

    EXPECT_EQ(Cmp::count, 2);
}

TEST(SetOperations, Test5) {
    Cmp::count = 0;

    auto x = set<int, Cmp>{1, 3, 5};

    auto it = x.upper_bound(1L);
    EXPECT_NE(it, x.end());
    EXPECT_EQ(*it, 3);
    it = x.upper_bound(5L);
    EXPECT_EQ(it, x.end());

    auto cit = std::as_const(x).upper_bound(1L);
    EXPECT_NE(cit, std::as_const(x).end());
    EXPECT_EQ(*cit, 3);
    cit = std::as_const(x).upper_bound(5L);
    EXPECT_EQ(cit, std::as_const(x).end());

    EXPECT_EQ(Cmp::count, 2);
}

TEST(SetOperations, Test6) {
    Cmp::count = 0;

    auto x = set<int, Cmp>{1, 3, 5};

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

    EXPECT_EQ(Cmp::count, 2);
}

TEST(SetOperations, Test7) {
    set<int>              s;
    test::I               i    = {};
    [[maybe_unused]] auto iter = s.find(i);
}

TEST(SetOperations, Test8) {
    auto s = set<int, test::WithPartition>{1, 2, 3, 4, 5};

    auto n = s.count(test::WithPartition::Partition{});
    EXPECT_EQ(n, 3);
}

} // namespace mstd
