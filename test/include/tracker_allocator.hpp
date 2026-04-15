#include <memory>

#include <uneq_allocator.hpp>

template <typename A>
typename A::size_type
max_size(const A &a)
{
    return std::allocator_traits<A>::max_size(a);
}

class tracker_allocator_counter
{
public:
    typedef std::size_t size_type;

    static void
    allocate(size_type blocksize)
    {
        allocationCount_ += blocksize;
    }

    static void
    construct() { ++constructCount_; }

    static void
    destroy() { ++destructCount_; }

    static void
    deallocate(size_type blocksize)
    {
        deallocationCount_ += blocksize;
    }

    static size_type
    get_allocation_count() { return allocationCount_; }

    static size_type
    get_deallocation_count() { return deallocationCount_; }

    static int
    get_construct_count() { return constructCount_; }

    static int
    get_destruct_count() { return destructCount_; }

    static void
    reset()
    {
        allocationCount_ = 0;
        deallocationCount_ = 0;
        constructCount_ = 0;
        destructCount_ = 0;
    }

private:
    static size_type allocationCount_;
    static size_type deallocationCount_;
    static int constructCount_;
    static int destructCount_;
};

template <typename T, typename Alloc = std::allocator<T>>
class tracker_allocator : public Alloc
{
private:
    typedef tracker_allocator_counter counter_type;

    typedef __gnu_cxx::__alloc_traits<Alloc> AllocTraits;

public:
    typedef typename check_consistent_alloc_value_type<T, Alloc>::value_type value_type;
    typedef typename AllocTraits::pointer pointer;
    typedef typename AllocTraits::size_type size_type;

    template <class U>
    struct rebind
    {
        typedef tracker_allocator<U,
                                  typename AllocTraits::template rebind<U>::other>
            other;
    };

    tracker_allocator() = default;
    tracker_allocator(const tracker_allocator &) = default;
    tracker_allocator(tracker_allocator &&) = default;
    tracker_allocator &operator=(const tracker_allocator &) = default;
    tracker_allocator &operator=(tracker_allocator &&) = default;

    // Perfect forwarding constructor.
    template <typename... ArgsT>
    tracker_allocator(ArgsT &&...__args)
        : Alloc(std::forward<ArgsT>(__args)...)
    {
    }

    template <class U>
    tracker_allocator(const tracker_allocator<U,
                                              typename AllocTraits::template rebind<U>::other> &alloc)
        _GLIBCXX_USE_NOEXCEPT
        : Alloc(alloc)
    {
    }

    pointer
    allocate(size_type n, const void * = 0)
    {
        pointer p = AllocTraits::allocate(*this, n);
        counter_type::allocate(n * sizeof(T));
        return p;
    }

    template <typename U, typename... Args>
    void
    construct(U *p, Args &&...args)
    {
        AllocTraits::construct(*this, p, std::forward<Args>(args)...);
        counter_type::construct();
    }

    template <typename U>
    void
    destroy(U *p)
    {
        AllocTraits::destroy(*this, p);
        counter_type::destroy();
    }

    void
    deallocate(pointer p, size_type num)
    {
        counter_type::deallocate(num * sizeof(T));
        AllocTraits::deallocate(*this, p, num);
    }

    // Implement swap for underlying allocators that might need it.
    friend void
    swap(tracker_allocator &a, tracker_allocator &b)
    {
        using std::swap;

        Alloc &aa = a;
        Alloc &ab = b;
        swap(aa, ab);
    }
};

template <class T1, class Alloc1, class T2, class Alloc2>
bool operator==(const tracker_allocator<T1, Alloc1> &lhs,
                const tracker_allocator<T2, Alloc2> &rhs) throw()
{
    const Alloc1 &alloc1 = lhs;
    const Alloc2 &alloc2 = rhs;
    return alloc1 == alloc2;
}

template <class T1, class Alloc1, class T2, class Alloc2>
bool operator!=(const tracker_allocator<T1, Alloc1> &lhs,
                const tracker_allocator<T2, Alloc2> &rhs) throw()
{
    return !(lhs == rhs);
}

bool check_construct_destroy(const char *tag, int expected_c, int expected_d);

template <typename Alloc>
bool check_deallocate_null()
{
    // Let's not core here...
    Alloc a;
    a.deallocate(0, 1);
    a.deallocate(0, 10);
    return true;
}

#if __cpp_exceptions
template <typename Alloc>
bool check_allocate_max_size()
{
    Alloc a;
    try
    {
        (void)a.allocate(std::allocator_traits<Alloc>::max_size(a) + 1);
    }
    catch (std::bad_alloc &)
    {
        return true;
    }
    catch (...)
    {
        throw;
    }
    throw;
}
#endif

