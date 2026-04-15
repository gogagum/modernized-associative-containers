#ifndef TEST_UNEQ_ALLOCATOR_HPP
#define TEST_UNEQ_ALLOCATOR_HPP

#include <memory>
#include <unordered_map>

#include <gtest/gtest.h>

struct uneq_allocator_base
{
    typedef std::unordered_map<void *, int> map_type;

    // Avoid static initialization troubles and/or bad interactions
    // with tests linking testsuite_allocator.o and playing globally
    // with operator new/delete.
    static map_type &
    get_map()
    {
        static map_type alloc_map;
        return alloc_map;
    }
};

template <typename T, typename Alloc, typename = typename Alloc::value_type>
struct check_consistent_alloc_value_type;

template <typename T, typename Alloc>
struct check_consistent_alloc_value_type<T, Alloc, T>
{
    typedef T value_type;
};

template <typename Tp, typename Alloc = std::allocator<Tp>>
class uneq_allocator
    : private uneq_allocator_base,
      public Alloc
{
    typedef __gnu_cxx::__alloc_traits<Alloc> AllocTraits;

public:
    typedef typename check_consistent_alloc_value_type<Tp, Alloc>::value_type
        value_type;
    typedef typename AllocTraits::size_type size_type;
    typedef typename AllocTraits::pointer pointer;
    typedef std::true_type propagate_on_container_swap;
    typedef std::false_type is_always_equal;

    template <typename Tp1>
    struct rebind
    {
        typedef uneq_allocator<Tp1,
                               typename AllocTraits::template rebind<Tp1>::other>
            other;
    };

    constexpr uneq_allocator() noexcept
        : personality(0) {}

    constexpr uneq_allocator(int person) noexcept
        : personality(person) {}

    uneq_allocator(const uneq_allocator &) = default;
    uneq_allocator(uneq_allocator &&) = default;

    template <typename Tp1>
    constexpr uneq_allocator(const uneq_allocator<Tp1, typename AllocTraits::template rebind<Tp1>::other> &b) noexcept
        : personality(b.get_personality()) {}

    constexpr int get_personality() const { return personality; }

    constexpr pointer
    allocate(size_type n, const void * = 0)
    {
        pointer p = AllocTraits::allocate(*this, n);

        if (std::__is_constant_evaluated())
            return p;

        try
        {
            get_map().insert(map_type::value_type(reinterpret_cast<void *>(p),
                                                  personality));
        }
        catch (...)
        {
            AllocTraits::deallocate(*this, p, n);
            throw;
        }

        return p;
    }

    constexpr void deallocate(pointer p, size_type n)
    {
        EXPECT_NE(p, nullptr);

        if (!std::__is_constant_evaluated())
        {
            map_type::iterator it = get_map().find(reinterpret_cast<void *>(p));
            EXPECT_NE(it, get_map().end());

            // Enforce requirements in Table 32 about deallocation vs
            // allocator equality.
            EXPECT_EQ(it->second, personality);

            get_map().erase(it);
        }

        AllocTraits::deallocate(*this, p, n);
    }

    // Not copy assignable...
    uneq_allocator &
    operator=(const uneq_allocator &) = delete;

    // ... but still moveable if base allocator is.
    uneq_allocator &
    operator=(uneq_allocator &&) = default;

private:
    // ... yet swappable!
    friend constexpr void
    swap(uneq_allocator &a, uneq_allocator &b)
    {
        std::swap(a.personality, b.personality);
        using std::swap;
        swap(static_cast<Alloc &>(a), static_cast<Alloc &>(b));
    }

    template <typename Tp1>
    friend constexpr bool
    operator==(const uneq_allocator &a,
               const uneq_allocator<Tp1,
                                    typename AllocTraits::template rebind<Tp1>::other> &b)
    {
        return a.personality == b.get_personality();
    }

    template <typename Tp1>
    friend constexpr bool
    operator!=(const uneq_allocator &a,
               const uneq_allocator<Tp1,
                                    typename AllocTraits::template rebind<Tp1>::other> &b)
    {
        return !(a == b);
    }

    int personality;
};

#endif
