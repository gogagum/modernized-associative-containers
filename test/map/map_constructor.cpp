#include <gtest/gtest.h>

#include <vector>
#include <map.hpp>

namespace mstd {

TEST(MapConstructorTests, FromVectorOfPairs)
{
    std::vector<std::pair<int, int>> nums{{1, 10}, {2, 20}, {2, 20}, {3, 30}};
    auto c1 = map(nums.begin(), nums.end());
    auto c2 = map(nums);
    auto c3 = nums | std::ranges::to<map>();
}

TEST(MapConstructorTests, FromVectorOfTuples)
{
    std::vector<std::tuple<int, int>> nums{{1, 10}, {2, 20}, {2, 20}, {3, 30}};
    auto c1 = map(nums.begin(), nums.end());
    auto c2 = map(nums);
    auto c3 = nums | std::ranges::to<map>();
}

TEST(MapConstructorTests, FromVectorOfPairsWithConstantKey)
{
    std::vector<std::pair<const int, int>> nums{{1, 10}, {2, 20}, {2, 20}, {3, 30}};
    auto c1 = map(nums.begin(), nums.end());
    auto c2 = map(nums);
    auto c3 = nums | std::ranges::to<map>();
}

TEST(MapConstructorTests, FromVectorOfTuplesWithConstantKey)
{
    std::vector<std::tuple<const int, int>> nums{{1, 10}, {2, 20}, {2, 20}, {3, 30}};
    auto c1 = map(nums.begin(), nums.end());
    auto c2 = map(nums);
    auto c3 = nums | std::ranges::to<map>();
}

}

