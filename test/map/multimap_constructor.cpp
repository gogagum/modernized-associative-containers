#include <gtest/gtest.h>

#include <map.hpp>

namespace mstd {

TEST(MultimapConstructorTests, FromVectorOfPairs)
{
    std::vector<std::pair<int, int>> nums{{1, 10}, {2, 20}, {2, 20}, {3, 30}};
    auto c1 = multimap(nums.begin(), nums.end());
    auto c2 = multimap(nums);
    auto c3 = nums | std::ranges::to<multimap>();
}

TEST(MultimapConstructorTests, FromVectorOfTuples)
{
    std::vector<std::tuple<int, int>> nums{{1, 10}, {2, 20}, {2, 20}, {3, 30}};
    auto c1 = multimap(nums.begin(), nums.end());
    auto c2 = multimap(nums);
    auto c3 = nums | std::ranges::to<multimap>();
}

TEST(MultimapConstructorTests, FromVectorOfPairsWithConstantKey)
{
    std::vector<std::pair<const int, int>> nums{{1, 10}, {2, 20}, {2, 20}, {3, 30}};
    auto c1 = multimap(nums.begin(), nums.end());
    auto c2 = multimap(nums);
    auto c3 = nums | std::ranges::to<multimap>();
}

TEST(MultimapConstructorTests, FromVectorOfTuplesWithConstantKey)
{
    std::vector<std::tuple<const int, int>> nums{{1, 10}, {2, 20}, {2, 20}, {3, 30}};
    auto c1 = multimap(nums.begin(), nums.end());
    auto c2 = multimap(nums);
    auto c3 = nums | std::ranges::to<multimap>();
}

}
