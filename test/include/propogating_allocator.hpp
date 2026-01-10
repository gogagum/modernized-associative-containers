#include "uneq_allocator.hpp"

template<typename Tp, bool Propagate, typename Alloc = std::allocator<Tp>>
    class propagating_allocator : public uneq_allocator<Tp, Alloc>
    {
      typedef __gnu_cxx::__alloc_traits<Alloc> AllocTraits;

      typedef uneq_allocator<Tp, Alloc> base_alloc;
      _GLIBCXX14_CONSTEXPR void
      swap_base(base_alloc& b)
      {
	using std::swap;
	swap(b, static_cast<base_alloc&>(*this));
      }

      typedef std::integral_constant<bool, Propagate> trait_type;

    public:
      // default allocator_traits::rebind_alloc would select
      // uneq_allocator::rebind so we must define rebind here
      template<typename Up>
	struct rebind
	{
	  typedef propagating_allocator<Up, Propagate,
		typename AllocTraits::template rebind<Up>::other> other;
	};

      constexpr
      propagating_allocator(int i) noexcept
      : base_alloc(i)
      { }

      template<typename Up>
	constexpr
	propagating_allocator(const propagating_allocator<Up, Propagate,
			      typename AllocTraits::template rebind<Up>::other>& a)
	noexcept
	: base_alloc(a)
	{ }

      propagating_allocator() noexcept = default;

      propagating_allocator(const propagating_allocator&) noexcept = default;

      _GLIBCXX14_CONSTEXPR
      propagating_allocator&
      operator=(const propagating_allocator& a) noexcept
      {
	static_assert(Propagate, "assigning propagating_allocator<T, true>");
	propagating_allocator(a).swap_base(*this);
	return *this;
      }

      template<bool P2>
	_GLIBCXX14_CONSTEXPR
	propagating_allocator&
	operator=(const propagating_allocator<Tp, P2, Alloc>& a) noexcept
  	{
	  static_assert(P2, "assigning propagating_allocator<T, true>");
	  propagating_allocator(a).swap_base(*this);
	  return *this;
  	}

      // postcondition: LWG2593 a.get_personality() un-changed.
      constexpr
      propagating_allocator(propagating_allocator&& a) noexcept
      : base_alloc(static_cast<base_alloc&&>(a))
      { }

      // postcondition: LWG2593 a.get_personality() un-changed
      _GLIBCXX14_CONSTEXPR
      propagating_allocator&
      operator=(propagating_allocator&& a) noexcept
      {
	propagating_allocator(std::move(a)).swap_base(*this);
	return *this;
      }

      typedef trait_type propagate_on_container_copy_assignment;
      typedef trait_type propagate_on_container_move_assignment;
      typedef trait_type propagate_on_container_swap;

      constexpr propagating_allocator
      select_on_container_copy_construction() const
      { return Propagate ? *this : propagating_allocator(); }
    };