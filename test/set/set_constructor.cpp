#include <gtest/gtest.h>

#include <vector>

#include <set.hpp>

namespace mstd {

TEST(SetConstructorTests, FromVector)
{
    std::vector nums{1, 2, 2, 3};
    auto c1 = set(nums.begin(), nums.end());
    auto c2 = set(nums);
    auto c3 = nums | std::ranges::to<set>();
}

} // namespace mstd
