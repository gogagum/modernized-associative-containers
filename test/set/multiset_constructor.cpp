#include <gtest/gtest.h>

#include <set.hpp>

namespace mstd {

TEST(MultisetConstructorTests, FromVector)
{
    auto nums = std::vector{1, 2, 2, 3};
    auto c1 = multiset(nums.begin(), nums.end());
    auto c2 = multiset(nums);
    auto c3 = nums | std::ranges::to<multiset>();
}

}  // namespace mstd
