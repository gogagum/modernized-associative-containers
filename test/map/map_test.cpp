#include <gtest/gtest.h>
#include <rval_struct.hpp>
#include <counter_type.hpp>

#include <map.hpp>
#include <string>

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

struct Mapped
{
    Mapped() = default;
    explicit Mapped(const Mapped &) = default;
};

struct DefaultConstructibleType
{
    int val;

    DefaultConstructibleType() : val(123)
    {
    }

    DefaultConstructibleType(const DefaultConstructibleType &) = delete;
    DefaultConstructibleType(DefaultConstructibleType &&) = delete;

    DefaultConstructibleType &operator=(int x)
    {
        val = x;
        return *this;
    }
};

TEST(MapTest, SubstractTest)
{
    mstd::map<int, Mapped> m1;
    m1[0] = Mapped();

    mstd::map<int, rvalstruct> m2;
    m2[0] = rvalstruct(13);

    mstd::map<int, DefaultConstructibleType> m3;
    EXPECT_EQ(m3[0].val, 123);
    EXPECT_EQ(m3.size(), 1);
    m3[0] = 2;
    EXPECT_EQ(m3[0].val, 2);

    mstd::map<counter_type, int> m4;
    EXPECT_EQ(m4[counter_type(1)], 0);
    EXPECT_EQ(counter_type::specialize_count, 1);
    EXPECT_EQ(counter_type::copy_count, 0);
    EXPECT_EQ(counter_type::move_count, 1);

    counter_type k(2);
    counter_type::reset();

    EXPECT_EQ(m4[k], 0);
    EXPECT_EQ(counter_type::copy_count, 1);
    EXPECT_EQ(counter_type::move_count, 0);
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

TEST(MapEmplaceTest, Test1)
{
    typedef mstd::map<char, std::vector<double>> Map;
    Map m;

    std::vector<double> coord1 = {0.0, 1.0, 2.0};

    auto ret = m.emplace('a', coord1);
    EXPECT_TRUE(ret.second);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(ret.first->key(), 'a');

    coord1[0] = 3.0;
    ret = m.emplace('a', coord1);
    EXPECT_TRUE(!ret.second);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(ret.first->key(), 'a');
    EXPECT_EQ(ret.first->value()[0], 0.0);

    auto it = m.emplace_hint(m.begin(), 'b', coord1);
    EXPECT_NE(it, m.end());
    EXPECT_EQ(it->key(), 'b');
    EXPECT_EQ(it->value()[0], 3.0);

    double *px = &coord1[0];
    ret = m.emplace('c', std::move(coord1));
    EXPECT_TRUE(ret.second);
    EXPECT_EQ(ret.first->key(), 'c');
    EXPECT_EQ(&(ret.first->value()[0]), px);
}

TEST(MapEmplaceTest, Test2)
{
    typedef mstd::map<char, PathPoint> Map;
    Map m;

    std::vector<double> coord1 = {0.0, 1.0, 2.0};

    auto ret = m.emplace(std::piecewise_construct,
                         std::make_tuple('a'), std::make_tuple('a', coord1));
    EXPECT_TRUE(ret.second);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(ret.first->key(), 'a');

    coord1[0] = 3.0;
    ret = m.emplace(std::piecewise_construct,
                    std::make_tuple('a'), std::make_tuple('b', coord1));
    EXPECT_TRUE(!ret.second);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(ret.first->key(), 'a');
    EXPECT_EQ(ret.first->value().getCoords()[0], 0.0);

    auto it = m.emplace_hint(m.begin(), std::piecewise_construct,
                             std::make_tuple('b'), std::make_tuple('c', coord1));
    EXPECT_NE(it, m.end());
    EXPECT_EQ(it->key(), 'b');
    EXPECT_EQ(it->value().getCoords()[0], 3.0);

    double *px = &coord1[0];
    ret = m.emplace(std::piecewise_construct,
                    std::make_tuple('c'), std::make_tuple('d', std::move(coord1)));
    EXPECT_TRUE(ret.second);
    EXPECT_EQ(ret.first->key(), 'c');
    EXPECT_EQ(&(ret.first->value().getCoords()[0]), px);
}

TEST(MapInsertTest, Test2)
{
    typedef mstd::map<int, int> Map;
    Map M;
    Map::iterator hint;

    hint = M.insert(Map::value_type(7, 0)).first;

    M.insert(hint, Map::value_type(8, 1));
    M.insert(M.begin(), Map::value_type(9, 2));

    EXPECT_NE(M.find(7), M.end());
    EXPECT_NE(M.find(8), M.end());
    EXPECT_NE(M.find(9), M.end());
}

struct Val
{
    bool moved_from_ctor = false;
    bool moved_from_assign = false;
    int val;
    Val(int val = 0) : val(val) {}
    Val(const Val &other) : val(other.val)
    {
    }
    Val(Val &&other) : val(other.val)
    {
        other.moved_from_ctor = true;
    }
    Val &operator=(Val &&other)
    {
        val = other.val;
        other.moved_from_assign = true;
        return *this;
    }
    Val &operator=(const Val &other)
    {
        val = other.val;
        return *this;
    }
};

auto operator<=>(const Val &a, const Val &b)
{
    return a.val <=> b.val;
}

TEST(MapInsertOrAssignTest, Test1)
{
    typedef mstd::map<int, Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(0, Val(5));
    EXPECT_TRUE(res1.second);
    EXPECT_NE(res1.first, m.end());
    EXPECT_EQ(m[0].val, 5);
    Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(0, std::move(v1));
    EXPECT_FALSE(res2.second);
    EXPECT_EQ(res2.first, res1.first);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_TRUE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    v1.moved_from_assign = false;
    auto res3 = m.insert_or_assign(1, std::move(v1));
    EXPECT_NE(res3.first, res1.first);
    EXPECT_NE(res3.first, m.end());
    EXPECT_TRUE(res3.second);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_EQ(m[1].val, 6);
    EXPECT_TRUE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 2);
}

TEST(MapInsertOrAssignTest, Test2)
{
    typedef mstd::map<int, Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(m.begin(), 0, Val(5));
    EXPECT_NE(res1, m.end());
    EXPECT_EQ(m[0].val, 5);
    Val v1{6};
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
    typedef mstd::map<Val, Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(0, Val(5));
    EXPECT_TRUE(res1.second);
    EXPECT_NE(res1.first, m.end());
    EXPECT_EQ(m[0].val, 5);
    Val k1{0};
    Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(std::move(k1), std::move(v1));
    EXPECT_FALSE(res2.second);
    EXPECT_EQ(res2.first, res1.first);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(k1.moved_from_ctor);
    EXPECT_FALSE(k1.moved_from_assign);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_TRUE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    Val k2{1};
    v1.moved_from_assign = false;
    auto res3 = m.insert_or_assign(std::move(k2), std::move(v1));
    EXPECT_NE(res3.first, res1.first);
    EXPECT_NE(res3.first, m.end());
    EXPECT_TRUE(res3.second);
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
    typedef mstd::map<Val, Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(m.begin(), 0, Val(5));
    EXPECT_NE(res1, m.end());
    EXPECT_EQ(m[0].val, 5);
    Val k1{0};
    Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(m.begin(), std::move(k1), std::move(v1));
    EXPECT_EQ(res2, res1);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(k1.moved_from_ctor);
    EXPECT_FALSE(k1.moved_from_assign);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_TRUE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    Val k2{1};
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
    typedef mstd::map<int, Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(0, Val(5));
    EXPECT_TRUE(res1.second);
    EXPECT_NE(res1.first, m.end());
    EXPECT_EQ(m[0].val, 5);
    Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(0, v1);
    EXPECT_FALSE(res2.second);
    EXPECT_EQ(res2.first, res1.first);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    auto res3 = m.insert_or_assign(1, v1);
    EXPECT_NE(res3.first, res1.first);
    EXPECT_NE(res3.first, m.end());
    EXPECT_TRUE(res3.second);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_EQ(m[1].val, 6);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 2);
}

TEST(MapInsertOrAssignTest, Test6)
{
    typedef mstd::map<int, Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(m.begin(), 0, Val(5));
    EXPECT_NE(res1, m.end());
    EXPECT_EQ(m[0].val, 5);
    Val v1{6};
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
    typedef mstd::map<Val, Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(0, Val(5));
    EXPECT_TRUE(res1.second);
    EXPECT_NE(res1.first, m.end());
    EXPECT_EQ(m[0].val, 5);
    Val k1{0};
    Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(k1, v1);
    EXPECT_FALSE(res2.second);
    EXPECT_EQ(res2.first, res1.first);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(k1.moved_from_ctor);
    EXPECT_FALSE(k1.moved_from_assign);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    Val k2{1};
    auto res3 = m.insert_or_assign(k2, v1);
    EXPECT_NE(res3.first, res1.first); 
    EXPECT_NE(res3.first, m.end());
    EXPECT_TRUE(res3.second);
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
    typedef mstd::map<Val, Val> Map;
    Map m;
    auto res1 = m.insert_or_assign(m.begin(), 0, Val(5));
    EXPECT_NE(res1, m.end());
    EXPECT_EQ(m[0].val, 5);
    Val k1{0};
    Val v1{6};
    EXPECT_EQ(m.size(), 1);
    auto res2 = m.insert_or_assign(m.begin(), k1, v1);
    EXPECT_EQ(res2, res1);
    EXPECT_EQ(m[0].val, 6);
    EXPECT_FALSE(k1.moved_from_ctor);
    EXPECT_FALSE(k1.moved_from_assign);
    EXPECT_FALSE(v1.moved_from_ctor);
    EXPECT_FALSE(v1.moved_from_assign);
    EXPECT_EQ(m.size(), 1);
    Val k2{1};
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
  typedef mstd::map<int, Val> Map;
  Map m;
  auto res1 = m.try_emplace(0, Val(5));
  EXPECT_TRUE(res1.second);
  EXPECT_NE(res1.first, m.end());
  EXPECT_EQ(m[0].val, 5);
  Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(0, std::move(v1));
  EXPECT_FALSE(res2.second);
  EXPECT_EQ(res2.first, res1.first);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  auto res3 = m.try_emplace(1, std::move(v1));
  EXPECT_NE(res3.first, res1.first); EXPECT_NE(res3.first, m.end());
  EXPECT_TRUE(res3.second);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_EQ(m[1].val, 6);
  EXPECT_TRUE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 2);
}

TEST(MapTryEmplace, Test2)
{
  typedef mstd::map<int, Val> Map;
  Map m;
  auto res1 = m.try_emplace(m.begin(), 0, Val(5));
  EXPECT_NE(res1, m.end());
  EXPECT_EQ(m[0].val, 5);
  Val v1{6};
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
  typedef mstd::map<Val, Val> Map;
  Map m;
  auto res1 = m.try_emplace(0, Val(5));
  EXPECT_TRUE(res1.second);
  EXPECT_NE(res1.first, m.end());
  EXPECT_EQ(m[0].val, 5);
  Val k1{0};
  Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(std::move(k1), std::move(v1));
  EXPECT_FALSE(res2.second);
  EXPECT_EQ(res2.first, res1.first);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(k1.moved_from_ctor);
  EXPECT_FALSE(k1.moved_from_assign);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  Val k2{1};
  auto res3 = m.try_emplace(std::move(k2), std::move(v1));
  EXPECT_NE(res3.first, res1.first); EXPECT_NE(res3.first, m.end());
  EXPECT_TRUE(res3.second);
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
  typedef mstd::map<Val, Val> Map;
  Map m;
  auto res1 = m.try_emplace(m.begin(), 0, Val(5));
  EXPECT_NE(res1, m.end());
  EXPECT_EQ(m[0].val, 5);
  Val k1{0};
  Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(m.begin(), std::move(k1), std::move(v1));
  EXPECT_EQ(res2, res1);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(k1.moved_from_ctor);
  EXPECT_FALSE(k1.moved_from_assign);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  Val k2{1};
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
  typedef mstd::map<int, Val> Map;
  Map m;
  auto res1 = m.try_emplace(0, Val(5));
  EXPECT_TRUE(res1.second);
  EXPECT_NE(res1.first, m.end());
  EXPECT_EQ(m[0].val, 5);
  Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(0, v1);
  EXPECT_FALSE(res2.second);
  EXPECT_EQ(res2.first, res1.first);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  auto res3 = m.try_emplace(1, v1);
  EXPECT_NE(res3.first, res1.first); EXPECT_NE(res3.first, m.end());
  EXPECT_TRUE(res3.second);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_EQ(m[1].val, 6);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 2);
}

TEST(MapTryEmplace, Test6)
{
  typedef mstd::map<int, Val> Map;
  Map m;
  auto res1 = m.try_emplace(m.begin(), 0, Val(5));
  EXPECT_NE(res1, m.end());
  EXPECT_EQ(m[0].val, 5);
  Val v1{6};
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
  typedef mstd::map<Val, Val> Map;
  Map m;
  auto res1 = m.try_emplace(0, Val(5));
  EXPECT_TRUE(res1.second);
  EXPECT_NE(res1.first, m.end());
  EXPECT_EQ(m[0].val, 5);
  Val k1{0};
  Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(k1, v1);
  EXPECT_FALSE(res2.second);
  EXPECT_EQ(res2.first, res1.first);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(k1.moved_from_ctor);
  EXPECT_FALSE(k1.moved_from_assign);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  Val k2{1};
  auto res3 = m.try_emplace(k2, v1);
  EXPECT_NE(res3.first, res1.first); EXPECT_NE(res3.first, m.end());
  EXPECT_TRUE(res3.second);
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
  typedef mstd::map<Val, Val> Map;
  Map m;
  auto res1 = m.try_emplace(m.begin(), 0, Val(5));
  EXPECT_NE(res1, m.end());
  EXPECT_EQ(m[0].val, 5);
  Val k1{0};
  Val v1{6};
  EXPECT_EQ(m.size(), 1);
  auto res2 = m.try_emplace(m.begin(), k1, v1);
  EXPECT_EQ(res2, res1);
  EXPECT_EQ(m[0].val, 5);
  EXPECT_FALSE(k1.moved_from_ctor);
  EXPECT_FALSE(k1.moved_from_assign);
  EXPECT_FALSE(v1.moved_from_ctor);
  EXPECT_FALSE(v1.moved_from_assign);
  EXPECT_EQ(m.size(), 1);
  Val k2{1};
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
