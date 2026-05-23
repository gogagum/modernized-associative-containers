#include <gtest/gtest.h>

#include <map.hpp>

#include <test_values.hpp>

namespace mstd {


TEST(MapEmplaceTest, Test1)
{
    typedef mstd::map<char, std::vector<double>> Map;
    Map m;

    std::vector<double> coord1 = {0.0, 1.0, 2.0};

    auto ret = m.emplace('a', coord1);
    EXPECT_TRUE(ret.inserted);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(ret.position->key(), 'a');

    coord1[0] = 3.0;
    ret = m.emplace('a', coord1);
    EXPECT_TRUE(!ret.inserted);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(ret.position->key(), 'a');
    EXPECT_EQ(ret.position->value()[0], 0.0);

    auto it = m.emplace_hint(m.begin(), 'b', coord1);
    EXPECT_NE(it, m.end());
    EXPECT_EQ(it->key(), 'b');
    EXPECT_EQ(it->value()[0], 3.0);

    double *px = &coord1[0];
    ret = m.emplace('c', std::move(coord1));
    EXPECT_TRUE(ret.inserted);
    EXPECT_EQ(ret.position->key(), 'c');
    EXPECT_EQ(&(ret.position->value()[0]), px);
}

TEST(MapEmplaceTest, Test2)
{
    mstd::map<char, test::PathPoint> m;

    std::vector<double> coord1 = {0.0, 1.0, 2.0};

    auto ret = m.emplace(std::piecewise_construct,
                         std::tuple('a'), std::tuple('a', coord1));
    EXPECT_TRUE(ret.inserted);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(ret.position->key(), 'a');

    coord1[0] = 3.0;
    ret = m.emplace(std::piecewise_construct,
                    std::tuple('a'), std::tuple('b', coord1));
    EXPECT_TRUE(!ret.inserted);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(ret.position->key(), 'a');
    EXPECT_EQ(ret.position->value().getCoords()[0], 0.0);

    auto it = m.emplace_hint(m.begin(), std::piecewise_construct,
                             std::tuple('b'), std::tuple('c', coord1));
    EXPECT_NE(it, m.end());
    EXPECT_EQ(it->key(), 'b');
    EXPECT_EQ(it->value().getCoords()[0], 3.0);

    double *px = &coord1[0];
    ret = m.emplace(std::piecewise_construct,
                    std::tuple('c'), std::tuple('d', std::move(coord1)));
    EXPECT_TRUE(ret.inserted);
    EXPECT_EQ(ret.position->key(), 'c');
    EXPECT_EQ(&(ret.position->value().getCoords()[0]), px);
}

TEST(MapInsertTest, Test2)
{
    typedef mstd::map<int, int> Map;
    Map M;
    Map::iterator hint;

    hint = M.insert(Map::value_type(7, 0)).position;

    M.insert(hint, Map::value_type(8, 1));
    M.insert(M.begin(), Map::value_type(9, 2));

    EXPECT_NE(M.find(7), M.end());
    EXPECT_NE(M.find(8), M.end());
    EXPECT_NE(M.find(9), M.end());
}

TEST(MapInsertTest, Test1)
{
    typedef mstd::map<int, const int> MapInt;

    MapInt m;

    for (unsigned i = 0; i < 10; ++i)
        m.insert(MapInt::value_type(i, i));

    for (MapInt::const_iterator i = m.begin(); i != m.end(); ++i)
    {
    }

    for (MapInt::const_iterator i = m.begin(); m.end() != i; ++i)
    {
    }
}

TEST(MapInsertOrAssignTest, Test1)
{
    typedef mstd::map<int, mstd::test::Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(0, mstd::test::Val(5));
    EXPECT_TRUE(res1.inserted);
    EXPECT_NE(res1.position, m.end());
    EXPECT_EQ(m[0].val, 5);
    mstd::test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(0, std::move(v1));
    EXPECT_FALSE(res2.inserted);
    EXPECT_EQ(res2.position, res1.position);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_TRUE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    v1.moved_from_assign = false;
    auto res3 = m.insert_or_assign(1, std::move(v1));
    EXPECT_NE(res3.position, res1.position);
    EXPECT_NE(res3.position, m.end());
    EXPECT_TRUE(res3.inserted);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_EQ(m[1].val, 6);
    EXPECT_TRUE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 2);
}

TEST(MapInsertOrAssignTest, Test2)
{
    typedef mstd::map<int, mstd::test::Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(m.begin(), 0, mstd::test::Val(5));
    EXPECT_NE(res1, m.end());
    EXPECT_EQ(m[0].val, 5);
    mstd::test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(m.begin(), 0, std::move(v1));
    EXPECT_EQ(res2, res1);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_TRUE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    v1.moved_from_assign = false;
    auto res3 = m.insert_or_assign(m.begin(), 1, std::move(v1));
    EXPECT_NE(res3, res1);
    EXPECT_NE(res3, m.end());
    EXPECT_EQ(m[0].val, 6);
    EXPECT_EQ(m[1].val, 6);
    EXPECT_TRUE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 2);
}

TEST(MapInsertOrAssignTest, Test3)
{
    typedef mstd::map<mstd::test::Val, mstd::test::Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(0, mstd::test::Val(5));
    EXPECT_TRUE(res1.inserted);
    EXPECT_NE(res1.position, m.end());
    EXPECT_EQ(m[0].val, 5);
    mstd::test::Val k1{0};
    mstd::test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(std::move(k1), std::move(v1));
    EXPECT_FALSE(res2.inserted);
    EXPECT_EQ(res2.position, res1.position);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(k1.moved_from_ctor);
    EXPECT_FALSE(k1.moved_from_assign);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_TRUE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    mstd::test::Val k2{1};
    v1.moved_from_assign = false;
    auto res3 = m.insert_or_assign(std::move(k2), std::move(v1));
    EXPECT_NE(res3.position, res1.position);
    EXPECT_NE(res3.position, m.end());
    EXPECT_TRUE(res3.inserted);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_EQ(m[1].val, 6);
    EXPECT_TRUE(k2.moved_from_ctor);
    EXPECT_FALSE(k2.moved_from_assign);
    EXPECT_TRUE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 2);
}

TEST(MapInsertOrAssignTest, Test4)
{
    typedef mstd::map<mstd::test::Val, mstd::test::Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(m.begin(), 0, mstd::test::Val(5));
    EXPECT_NE(res1, m.end());
    EXPECT_EQ(m[0].val, 5);
    mstd::test::Val k1{0};
    mstd::test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(m.begin(), std::move(k1), std::move(v1));
    EXPECT_EQ(res2, res1);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(k1.moved_from_ctor);
    EXPECT_FALSE(k1.moved_from_assign);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_TRUE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    mstd::test::Val k2{1};
    v1.moved_from_assign = false;
    auto res3 = m.insert_or_assign(m.begin(), std::move(k2), std::move(v1));
    EXPECT_NE(res3, res1);
    EXPECT_NE(res3, m.end());
    EXPECT_EQ(m[0].val, 6);
    EXPECT_EQ(m[1].val, 6);
    EXPECT_TRUE(k2.moved_from_ctor);
    EXPECT_FALSE(k2.moved_from_assign); 
    EXPECT_TRUE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 2);
}

TEST(MapInsertOrAssignTest, Test5)
{
    typedef mstd::map<int, mstd::test::Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(0, mstd::test::Val(5));
    EXPECT_TRUE(res1.inserted);
    EXPECT_NE(res1.position, m.end());
    EXPECT_EQ(m[0].val, 5);
    mstd::test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(0, v1);
    EXPECT_FALSE(res2.inserted);
    EXPECT_EQ(res2.position, res1.position);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    auto res3 = m.insert_or_assign(1, v1);
    EXPECT_NE(res3.position, res1.position);
    EXPECT_NE(res3.position, m.end());
    EXPECT_TRUE(res3.inserted);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_EQ(m[1].val, 6);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 2);
}

TEST(MapInsertOrAssignTest, Test6)
{
    typedef mstd::map<int, mstd::test::Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(m.begin(), 0, mstd::test::Val(5));
    EXPECT_NE(res1, m.end());
    EXPECT_EQ(m[0].val, 5);
    mstd::test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(m.begin(), 0, v1);
    EXPECT_EQ(res2, res1);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    auto res3 = m.insert_or_assign(m.begin(), 1, v1);
    EXPECT_NE(res3, res1);
    EXPECT_NE(res3, m.end());
    EXPECT_EQ(m[0].val, 6);
    EXPECT_EQ(m[1].val, 6);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 2);
}

TEST(MapInsertOrAssignTest, Test7)
{
    typedef mstd::map<mstd::test::Val, mstd::test::Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(0, mstd::test::Val(5));
    EXPECT_TRUE(res1.inserted);
    EXPECT_NE(res1.position, m.end());
    EXPECT_EQ(m[0].val, 5);
    mstd::test::Val k1{0};
    mstd::test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(k1, v1);
    EXPECT_FALSE(res2.inserted);
    EXPECT_EQ(res2.position, res1.position);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(k1.moved_from_ctor);
    EXPECT_FALSE(k1.moved_from_assign);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    mstd::test::Val k2{1};
    auto res3 = m.insert_or_assign(k2, v1);
    EXPECT_NE(res3.position, res1.position); 
    EXPECT_NE(res3.position, m.end());
    EXPECT_TRUE(res3.inserted);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_EQ(m[1].val, 6);
    EXPECT_FALSE(k2.moved_from_ctor);
    EXPECT_FALSE(k2.moved_from_assign);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 2);
}

TEST(MapInsertOrAssignTest, Test8)
{
    typedef mstd::map<mstd::test::Val, mstd::test::Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(m.begin(), 0, mstd::test::Val(5));
    EXPECT_NE(res1, m.end());
    EXPECT_EQ(m[0].val, 5);
    mstd::test::Val k1{0};
    mstd::test::Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(m.begin(), k1, v1);
    EXPECT_EQ(res2, res1);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(k1.moved_from_ctor);
    EXPECT_FALSE(k1.moved_from_assign);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    mstd::test::Val k2{1};
    auto res3 = m.insert_or_assign(m.begin(), k2, v1);
    EXPECT_NE(res3, res1); 
    EXPECT_NE(res3, m.end());
    EXPECT_EQ(m[0].val, 6);
    EXPECT_EQ(m[1].val, 6);
    EXPECT_FALSE(k2.moved_from_ctor);
    EXPECT_FALSE(k2.moved_from_assign);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 2);
}

TEST(MapTryEmplace, Test1)
{
  typedef mstd::map<int, mstd::test::Val> Map;
  Map m;
  auto res1 = m.try_emplace(0, mstd::test::Val(5));
  EXPECT_TRUE(res1.inserted);
  EXPECT_NE(res1.position, m.end());
  EXPECT_EQ(m[0].val, 5);
  mstd::test::Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(0, std::move(v1));
  EXPECT_FALSE(res2.inserted);
  EXPECT_EQ(res2.position, res1.position);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  auto res3 = m.try_emplace(1, std::move(v1));
  EXPECT_NE(res3.position, res1.position);
  EXPECT_NE(res3.position, m.end());
  EXPECT_TRUE(res3.inserted);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_EQ(m[1].val, 6);
  EXPECT_TRUE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 2);
}

TEST(MapTryEmplace, Test2)
{
  typedef mstd::map<int, mstd::test::Val> Map;
  Map m;
  auto res1 = m.try_emplace(m.begin(), 0, mstd::test::Val(5));
  EXPECT_NE(res1, m.end());
  EXPECT_EQ(m[0].val, 5);
  mstd::test::Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(m.begin(), 0, std::move(v1));
  EXPECT_EQ(res2, res1);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  auto res3 = m.try_emplace(m.begin(), 1, std::move(v1));
  EXPECT_NE(res3, res1); EXPECT_NE(res3, m.end());
  EXPECT_EQ(m[0].val, 5);
  EXPECT_EQ(m[1].val, 6);
  EXPECT_TRUE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 2);
}

TEST(MapTryEmplace, Test3)
{
  typedef mstd::map<mstd::test::Val, mstd::test::Val> Map;
  Map m;
  auto res1 = m.try_emplace(0, mstd::test::Val(5));
  EXPECT_TRUE(res1.inserted);
  EXPECT_NE(res1.position, m.end());
  EXPECT_EQ(m[0].val, 5);
  mstd::test::Val k1{0};
  mstd::test::Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(std::move(k1), std::move(v1));
  EXPECT_FALSE(res2.inserted);
  EXPECT_EQ(res2.position, res1.position);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(k1.moved_from_ctor);
  EXPECT_FALSE(k1.moved_from_assign);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  mstd::test::Val k2{1};
  auto res3 = m.try_emplace(std::move(k2), std::move(v1));
  EXPECT_NE(res3.position, res1.position);
  EXPECT_NE(res3.position, m.end());
  EXPECT_TRUE(res3.inserted);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_EQ(m[1].val, 6);
  EXPECT_TRUE(k2.moved_from_ctor);
  EXPECT_FALSE(k2.moved_from_assign);
  EXPECT_TRUE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 2);
}

TEST(MapTryEmplace, Test4)
{
  typedef mstd::map<mstd::test::Val, mstd::test::Val> Map;
  Map m;
  auto res1 = m.try_emplace(m.begin(), 0, mstd::test::Val(5));
  EXPECT_NE(res1, m.end());
  EXPECT_EQ(m[0].val, 5);
  mstd::test::Val k1{0};
  mstd::test::Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(m.begin(), std::move(k1), std::move(v1));
  EXPECT_EQ(res2, res1);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(k1.moved_from_ctor);
  EXPECT_FALSE(k1.moved_from_assign);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  mstd::test::Val k2{1};
  auto res3 = m.try_emplace(m.begin(), std::move(k2), std::move(v1));
  EXPECT_NE(res3, res1); EXPECT_NE(res3, m.end());
  EXPECT_EQ(m[0].val, 5);
  EXPECT_EQ(m[1].val, 6);
  EXPECT_TRUE(k2.moved_from_ctor);
  EXPECT_FALSE(k2.moved_from_assign);
  EXPECT_TRUE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 2);
}

TEST(MapTryEmplace, Test5)
{
  typedef mstd::map<int, mstd::test::Val> Map;
  Map m;
  auto res1 = m.try_emplace(0, mstd::test::Val(5));
  EXPECT_TRUE(res1.inserted);
  EXPECT_NE(res1.position, m.end());
  EXPECT_EQ(m[0].val, 5);
  mstd::test::Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(0, v1);
  EXPECT_FALSE(res2.inserted);
  EXPECT_EQ(res2.position, res1.position);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  auto res3 = m.try_emplace(1, v1);
  EXPECT_NE(res3.position, res1.position);
  EXPECT_NE(res3.position, m.end());
  EXPECT_TRUE(res3.inserted);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_EQ(m[1].val, 6);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 2);
}

TEST(MapTryEmplace, Test6)
{
  typedef mstd::map<int, mstd::test::Val> Map;
  Map m;
  auto res1 = m.try_emplace(m.begin(), 0, mstd::test::Val(5));
  EXPECT_NE(res1, m.end());
  EXPECT_EQ(m[0].val, 5);
  mstd::test::Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(m.begin(), 0, v1);
  EXPECT_EQ(res2, res1);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  auto res3 = m.try_emplace(m.begin(), 1, v1);
  EXPECT_NE(res3, res1); EXPECT_NE(res3, m.end());
  EXPECT_EQ(m[0].val, 5);
  EXPECT_EQ(m[1].val, 6);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 2);
}

TEST(MapTryEmplace, Test7)
{
  typedef mstd::map<mstd::test::Val, mstd::test::Val> Map;
  Map m;
  auto res1 = m.try_emplace(0, mstd::test::Val(5));
  EXPECT_TRUE(res1.inserted);
  EXPECT_NE(res1.position, m.end());
  EXPECT_EQ(m[0].val, 5);
  mstd::test::Val k1{0};
  mstd::test::Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(k1, v1);
  EXPECT_FALSE(res2.inserted);
  EXPECT_EQ(res2.position, res1.position);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(k1.moved_from_ctor);
  EXPECT_FALSE(k1.moved_from_assign);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  mstd::test::Val k2{1};
  auto res3 = m.try_emplace(k2, v1);
  EXPECT_NE(res3.position, res1.position);
  EXPECT_NE(res3.position, m.end());
  EXPECT_TRUE(res3.inserted);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_EQ(m[1].val, 6);
  EXPECT_FALSE(k2.moved_from_ctor);
  EXPECT_FALSE(k2.moved_from_assign);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 2);
}

TEST(MapTryEmplace, Test8)
{
  typedef mstd::map<mstd::test::Val, mstd::test::Val> Map;
  Map m;
  auto res1 = m.try_emplace(m.begin(), 0, mstd::test::Val(5));
  EXPECT_NE(res1, m.end());
  EXPECT_EQ(m[0].val, 5);
  mstd::test::Val k1{0};
  mstd::test::Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(m.begin(), k1, v1);
  EXPECT_EQ(res2, res1);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(k1.moved_from_ctor);
  EXPECT_FALSE(k1.moved_from_assign);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  mstd::test::Val k2{1};
  auto res3 = m.try_emplace(m.begin(), k2, v1);
  EXPECT_NE(res3, res1); EXPECT_NE(res3, m.end());
  EXPECT_EQ(m[0].val, 5);
  EXPECT_EQ(m[1].val, 6);
  EXPECT_FALSE(k2.moved_from_ctor);
  EXPECT_FALSE(k2.moved_from_assign);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 2);
}

}