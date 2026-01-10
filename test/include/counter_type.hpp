#ifndef COUNTER_TYPE_HPP
#define COUNTER_TYPE_HPP


#include <cstdint>

struct counter_type
{
    // Constructor counters:
    static int default_count;
    static int specialize_count;
    static int copy_count;
    static int copy_assign_count;
    static int less_compare_count;
    static int move_count;
    static int move_assign_count;
    static int destructor_count;

    int val;

    counter_type() : val(0)
    {
        ++default_count;
    }

    counter_type(int inval) : val(inval)
    {
        ++specialize_count;
    }

    counter_type(const counter_type &in) : val(in.val)
    {
        ++copy_count;
    }

    ~counter_type()
    {
        ++destructor_count;
    }

    counter_type &
    operator=(const counter_type &in)
    {
        val = in.val;
        ++copy_assign_count;
        return *this;
    }

    counter_type(counter_type &&in) noexcept
    {
        val = in.val;
        ++move_count;
    }

    counter_type &
    operator=(counter_type &&rhs) noexcept
    {
        val = rhs.val;
        ++move_assign_count;
        return *this;
    }

    static void
    reset()
    {
        default_count = 0;
        specialize_count = 0;
        copy_count = 0;
        copy_assign_count = 0;
        less_compare_count = 0;
        move_count = 0;
        move_assign_count = 0;
        destructor_count = 0;
    }

    bool operator==(const counter_type &rhs) const
    {
        return val == rhs.val;
    }

    bool operator<(const counter_type &rhs) const
    {
        ++less_compare_count;
        return val < rhs.val;
    }
};

int counter_type::default_count = 0;
int counter_type::specialize_count = 0;
int counter_type::copy_count = 0;
int counter_type::copy_assign_count = 0;
int counter_type::less_compare_count = 0;
int counter_type::move_count = 0;
int counter_type::move_assign_count = 0;
int counter_type::destructor_count = 0;

struct counter_type_hasher
{
    std::size_t operator()(const counter_type &c) const
    {
        return c.val;
    }
};

#endif
