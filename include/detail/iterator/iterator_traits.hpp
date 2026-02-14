// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_ITERATOR_ITERATOR_TRAITS_HPP
#define MSTD_ITERATOR_ITERATOR_TRAITS_HPP

#include <concepts>
#include <detail/config.hpp>
#include <detail/type_traits/is_referencable.hpp>
#include <detail/type_traits/disjunction.hpp>
#include <detail/type_traits/nat.hpp>
#include <detail/type_traits/detected_or.hpp>
#include <cstddef>
#include <utility>
#include <tuple>
#include <type_traits>

namespace mstd {

template <class _Tp>
concept __dereferenceable = requires(_Tp& __t) {
  { *__t } -> __referenceable; // not required to be equality-preserving
};

// [iterator.traits]
template <__dereferenceable _Tp>
using iter_reference_t = decltype(*std::declval<_Tp&>());

template <class _Iter>
struct iterator_traits;

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
struct contiguous_iterator_tag : public random_access_iterator_tag {};

// The `cpp17-*-iterator` exposition-only concepts have very similar names to the `Cpp17*Iterator` named requirements
// from `[iterator.cpp17]`. To avoid confusion between the two, the exposition-only concepts have been banished to
// a "detail" namespace indicating they have a niche use-case.
namespace __iterator_traits_detail {
template <class _Ip>
concept __cpp17_iterator = requires(_Ip __i) {
  { *__i } -> __referenceable;
  { ++__i } -> std::same_as<_Ip&>;
  { *__i++ } -> __referenceable;
} && std::copyable<_Ip>;

template <class _Ip>
concept __cpp17_input_iterator = __cpp17_iterator<_Ip> && std::equality_comparable<_Ip> && requires(_Ip __i) {
  typename std::incrementable_traits<_Ip>::difference_type;
  typename std::indirectly_readable_traits<_Ip>::value_type;
  typename std::common_reference_t<iter_reference_t<_Ip>&&, typename std::indirectly_readable_traits<_Ip>::value_type&>;
  typename std::common_reference_t<decltype(*__i++)&&, typename std::indirectly_readable_traits<_Ip>::value_type&>;
  requires std::signed_integral<typename std::incrementable_traits<_Ip>::difference_type>;
};

template <class _Ip>
concept __cpp17_forward_iterator =
    __cpp17_input_iterator<_Ip> && std::constructible_from<_Ip> && std::is_reference_v<iter_reference_t<_Ip>> &&
    std::same_as<std::remove_cvref_t<iter_reference_t<_Ip>>, typename std::indirectly_readable_traits<_Ip>::value_type> &&
    requires(_Ip __i) {
      { __i++ } -> std::convertible_to<_Ip const&>;
      { *__i++ } -> std::same_as<iter_reference_t<_Ip>>;
    };

template <class _Ip>
concept __cpp17_bidirectional_iterator = __cpp17_forward_iterator<_Ip> && requires(_Ip __i) {
  { --__i } -> std::same_as<_Ip&>;
  { __i-- } -> std::convertible_to<_Ip const&>;
  { *__i-- } -> std::same_as<iter_reference_t<_Ip>>;
};

template <class _Ip>
concept __cpp17_random_access_iterator =
    __cpp17_bidirectional_iterator<_Ip> && std::totally_ordered<_Ip> &&
    requires(_Ip __i, typename std::incrementable_traits<_Ip>::difference_type __n) {
      { __i += __n } -> std::same_as<_Ip&>;
      { __i -= __n } -> std::same_as<_Ip&>;
      { __i + __n } -> std::same_as<_Ip>;
      { __n + __i } -> std::same_as<_Ip>;
      { __i - __n } -> std::same_as<_Ip>;
      { __i - __i } -> std::same_as<decltype(__n)>; // NOLINT(misc-redundant-expression) ; This is llvm.org/PR54114
      { __i[__n] } -> std::convertible_to<iter_reference_t<_Ip>>;
    };
} // namespace __iterator_traits_detail

template <class _Ip>
concept __has_member_reference = requires { typename _Ip::reference; };

template <class _Ip>
concept __has_member_pointer = requires { typename _Ip::pointer; };

template <class _Ip>
concept __has_member_iterator_category = requires { typename _Ip::iterator_category; };

template <class _Ip>
concept __specifies_members = requires {
  typename _Ip::value_type;
  typename _Ip::difference_type;
  requires __has_member_reference<_Ip>;
  requires __has_member_iterator_category<_Ip>;
};

// Otherwise, `pointer` names `void`.
template <class>
struct __iterator_traits_member_pointer_or_arrow_or_void {
  using type = void;
};

// [iterator.traits]/3.2.1
// If the qualified-id `I::pointer` is valid and denotes a type, `pointer` names that type.
template <__has_member_pointer _Ip>
struct __iterator_traits_member_pointer_or_arrow_or_void<_Ip> {
  using type = typename _Ip::pointer;
};

// Otherwise, if `decltype(declval<I&>().operator->())` is well-formed, then `pointer` names that
// type.
template <class _Ip>
  requires requires(_Ip& __i) { __i.operator->(); } && (!__has_member_pointer<_Ip>)
struct __iterator_traits_member_pointer_or_arrow_or_void<_Ip> {
  using type = decltype(std::declval<_Ip&>().operator->());
};

// Otherwise, `reference` names `iter-reference-t<I>`.
template <class _Ip>
struct __iterator_traits_member_reference {
  using type = iter_reference_t<_Ip>;
};

// [iterator.traits]/3.2.2
// If the qualified-id `I::reference` is valid and denotes a type, `reference` names that type.
template <__has_member_reference _Ip>
struct __iterator_traits_member_reference<_Ip> {
  using type = typename _Ip::reference;
};

// [iterator.traits]/3.2.3.4
// input_iterator_tag
template <class _Ip>
struct __deduce_iterator_category {
  using type = input_iterator_tag;
};

// [iterator.traits]/3.2.3.1
// `random_access_iterator_tag` if `I` satisfies `cpp17-random-access-iterator`, or otherwise
template <__iterator_traits_detail::__cpp17_random_access_iterator _Ip>
struct __deduce_iterator_category<_Ip> {
  using type = random_access_iterator_tag;
};

// [iterator.traits]/3.2.3.2
// `bidirectional_iterator_tag` if `I` satisfies `cpp17-bidirectional-iterator`, or otherwise
template <__iterator_traits_detail::__cpp17_bidirectional_iterator _Ip>
struct __deduce_iterator_category<_Ip> {
  using type = bidirectional_iterator_tag;
};

// [iterator.traits]/3.2.3.3
// `forward_iterator_tag` if `I` satisfies `cpp17-forward-iterator`, or otherwise
template <__iterator_traits_detail::__cpp17_forward_iterator _Ip>
struct __deduce_iterator_category<_Ip> {
  using type = forward_iterator_tag;
};

template <class _Ip>
struct __iterator_traits_iterator_category : __deduce_iterator_category<_Ip> {};

// [iterator.traits]/3.2.3
// If the qualified-id `I::iterator-category` is valid and denotes a type, `iterator-category` names
// that type.
template <__has_member_iterator_category _Ip>
struct __iterator_traits_iterator_category<_Ip> {
  using type = typename _Ip::iterator_category;
};

// otherwise, it names void.
template <class>
struct __iterator_traits_difference_type {
  using type = void;
};

// If the qualified-id `incrementable_traits<I>::difference_type` is valid and denotes a type, then
// `difference_type` names that type;
template <class _Ip>
  requires requires { typename std::incrementable_traits<_Ip>::difference_type; }
struct __iterator_traits_difference_type<_Ip> {
  using type = typename std::incrementable_traits<_Ip>::difference_type;
};

// [iterator.traits]/3.4
// Otherwise, `iterator_traits<I>` has no members by any of the above names.
template <class>
struct __iterator_traits {};

template <class _Tp>
using __pointer_member = typename _Tp::pointer;

// [iterator.traits]/3.1
// If `I` has valid ([temp.deduct]) member types `difference-type`, `value-type`, `reference`, and
// `iterator-category`, then `iterator-traits<I>` has the following publicly accessible members:
template <__specifies_members _Ip>
struct __iterator_traits<_Ip> {
  using iterator_category = typename _Ip::iterator_category;
  using value_type        = typename _Ip::value_type;
  using difference_type   = typename _Ip::difference_type;
  using pointer           = __detected_or_t<void, __pointer_member, _Ip>;
  using reference         = typename _Ip::reference;
};

template <class _Ip>
struct iterator_traits : __iterator_traits<_Ip> {
  using __primary_template = iterator_traits;
};

template <class _Tp>
  requires std::is_object_v<_Tp>
struct iterator_traits<_Tp*> {
  typedef ptrdiff_t difference_type;
  typedef std::remove_cv_t<_Tp> value_type;
  typedef _Tp* pointer;
  typedef _Tp& reference;
  typedef random_access_iterator_tag iterator_category;
  typedef contiguous_iterator_tag iterator_concept;
};

template <class _Tp>
using __iterator_category = typename _Tp::iterator_category;

template <class _Tp>
using __iterator_concept = typename _Tp::iterator_concept;

template <class _Tp, class _Up>
using __has_iterator_category_convertible_to =
    std::is_convertible<__detected_or_t<__nat, __iterator_category, std::iterator_traits<_Tp> >, _Up>;

template <class _Tp, class _Up>
using __has_iterator_concept_convertible_to =
    std::is_convertible<__detected_or_t<__nat, __iterator_concept, _Tp>, _Up>;

template <class _Tp>
using __has_input_iterator_category = __has_iterator_category_convertible_to<_Tp, input_iterator_tag>;

template <class _Tp>
using __has_forward_iterator_category =
    __has_iterator_category_convertible_to<_Tp, forward_iterator_tag>;

template <class _Tp>
using __has_bidirectional_iterator_category =
    __has_iterator_category_convertible_to<_Tp, bidirectional_iterator_tag>;

template <class _Tp>
using __has_random_access_iterator_category =
    __has_iterator_category_convertible_to<_Tp, random_access_iterator_tag>;

// _MSTD_is_contiguous_iterator determines if an iterator is known by
// libc++ to be contiguous, either because it advertises itself as such
// (in C++20) or because it is a pointer type or a known trivial wrapper
// around a (possibly fancy) pointer type, such as __wrap_iter<T*>.
// Such iterators receive special "contiguous" optimizations in
// std::copy and std::sort.
//
template <class _Tp>
struct _MSTD_is_contiguous_iterator
    : _Or< __has_iterator_category_convertible_to<_Tp, contiguous_iterator_tag>,
           __has_iterator_concept_convertible_to<_Tp, contiguous_iterator_tag> > {};

// Any native pointer which is an iterator is also a contiguous iterator.
template <class _Up>
struct _MSTD_is_contiguous_iterator<_Up*> : std::true_type {};

template <class _Iter>
class __wrap_iter;

template <class _Tp>
using __has_exactly_input_iterator_category =
    std::integral_constant<bool,
                      __has_iterator_category_convertible_to<_Tp, input_iterator_tag>::value &&
                          !__has_iterator_category_convertible_to<_Tp, forward_iterator_tag>::value>;

template <class _Tp>
using __has_exactly_forward_iterator_category =
    std::integral_constant<bool,
                      __has_iterator_category_convertible_to<_Tp, forward_iterator_tag>::value &&
                          !__has_iterator_category_convertible_to<_Tp, bidirectional_iterator_tag>::value>;

template <class _Tp>
using __has_exactly_bidirectional_iterator_category =
    std::integral_constant<bool,
                      __has_iterator_category_convertible_to<_Tp, bidirectional_iterator_tag>::value &&
                          !__has_iterator_category_convertible_to<_Tp, random_access_iterator_tag>::value>;

template <class InputIteratorT>
using __iterator_value_type = typename iterator_traits<InputIteratorT>::value_type;

template <class InputIteratorT>
using __iter_key_type = std::remove_const_t<std::tuple_element_t<0, __iterator_value_type<InputIteratorT>>>;

template <class InputIteratorT>
using __iter_mapped_type = std::tuple_element_t<1, __iterator_value_type<InputIteratorT>>;

template <class InputIteratorT>
using __iter_to_alloc_type =
    std::pair<const std::tuple_element_t<0, __iterator_value_type<InputIteratorT>>,
         std::tuple_element_t<1, __iterator_value_type<InputIteratorT>>>;

template <class _Iter>
using __iterator_iterator_category = typename iterator_traits<_Iter>::iterator_category;

template <class _Iter>
using __iterator_pointer = typename iterator_traits<_Iter>::pointer;

template <class _Iter>
using __iterator_difference_type = typename iterator_traits<_Iter>::difference_type;

template <class _Iter>
using __iterator_reference = typename iterator_traits<_Iter>::reference;

} // namespace mstd

#endif // MSTD_ITERATOR_ITERATOR_TRAITS_HPP
