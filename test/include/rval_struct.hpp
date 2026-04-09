#ifndef RVAL_STRUCT_HPP
#define RVAL_STRUCT_HPP

#include <gtest/gtest.h>

struct rvalstruct
{
    int val;
    bool valid;

    rvalstruct() : val(0), valid(true)
    {
    }

    rvalstruct(int inval) : val(inval), valid(true)
    {
    }

    rvalstruct &
    operator=(int newval)
    {
        val = newval;
        valid = true;
        return *this;
    }

    rvalstruct(const rvalstruct &) = delete;

    rvalstruct(rvalstruct &&in)
    {
        EXPECT_TRUE(in.valid);
        val = in.val;
        in.valid = false;
        valid = true;
    }

    rvalstruct &operator=(const rvalstruct &) = delete;

    rvalstruct &operator=(rvalstruct &&in)
    {
        EXPECT_NE(this, &in);
        EXPECT_TRUE(in.valid);
        val = in.val;
        in.valid = false;
        valid = true;
        return *this;
    }

    inline friend bool operator==(const rvalstruct &lhs, const rvalstruct &rhs) = default;
};

inline std::weak_ordering
operator<=>(const rvalstruct &lhs, const rvalstruct &rhs)
{
    return lhs.val <=> rhs.val;
}

#endif
