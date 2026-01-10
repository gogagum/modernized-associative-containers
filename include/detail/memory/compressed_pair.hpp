// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_MEMORY_COMPRESSED_PAIR_HPP
#define MSTD_MEMORY_COMPRESSED_PAIR_HPP

#include <detail/config.hpp>
#include <cstddef>
#include <type_traits>
#include <detail/type_traits/datasizeof.hpp>

namespace mstd {

// ================================================================================================================== //
// The utilities here are for staying ABI compatible with the legacy `__compressed_pair`. They should not be used      //
// for new data structures. Use `[[no_unique_address]]` for new data structures instead (but make sure you        //
// understand how it works).                                                                                          //
// ================================================================================================================== //

// On GCC, the first member is aligned to the alignment of the second member to force padding in front of the compressed
// pair in case there are members before it.
//
// For example:
// (assuming x86-64 linux)
// class SomeClass {
//   uint32_t member1;
//   MSTD_COMPRESSED_PAIR(uint32_t, member2, uint64_t, member3);
// }
//
// The layout with __compressed_pair is:
// member1 - offset: 0,  size: 4
// padding - offset: 4,  size: 4
// member2 - offset: 8,  size: 4
// padding - offset: 12, size: 4
// member3 - offset: 16, size: 8
//
// If the [[gnu::aligned]] wasn't there, the layout would instead be:
// member1 - offset: 0, size: 4
// member2 - offset: 4, size: 4
// member3 - offset: 8, size: 8
//
// Furthermore, that alignment must be the same as what was used in the old __compressed_pair layout, so we must
// handle reference types specially since alignof(T&) == alignof(T).
// See https://llvm.org/PR118559.
//
// On Clang, this is unnecessary, since we use anonymous structs instead, which automatically handle the alignment
// correctly.

#ifndef MSTD_ABI_NO_COMPRESSED_PAIR_PADDING

template <class _Tp>
inline const size_t __compressed_pair_alignment = alignof(_Tp);

template <class _Tp>
inline const size_t __compressed_pair_alignment<_Tp&> = alignof(void*);

template <class _ToPad>
inline const bool __is_reference_or_unpadded_object =
    (std::is_empty<_ToPad>::value && !std::is_final_v<_ToPad>) || sizeof(_ToPad) == __datasizeof_v<_ToPad>;

template <class _Tp>
inline const bool __is_reference_or_unpadded_object<_Tp&> = true;

template <class _Tp>
inline const bool __is_reference_or_unpadded_object<_Tp&&> = true;

template <class _ToPad, bool _Empty = __is_reference_or_unpadded_object<_ToPad> >
class __compressed_pair_padding {
  char __padding_[sizeof(_ToPad) - __datasizeof_v<_ToPad>] = {};
};

template <class _ToPad>
class __compressed_pair_padding<_ToPad, true> {};

#  define MSTD_COMPRESSED_ELEMENT(T1, Initializer1)                                                                 \
    [[no_unique_address]] T1 Initializer1;                                                                         \
    [[no_unique_address]] ::mstd::__compressed_pair_padding<T1> MSTD_CONCAT3(__padding_, __LINE__, _)

// TODO: Fix the ABI for GCC as well once https://gcc.gnu.org/bugzilla/show_bug.cgi?id=121637 is fixed
#  ifdef MSTD_COMPILER_GCC
#    define MSTD_COMPRESSED_PAIR(T1, Initializer1, T2, Initializer2)                                                \
      [[no_unique_address]] __attribute__((__aligned__(::mstd::__compressed_pair_alignment<T2>))) T1 Initializer1;  \
      [[no_unique_address]] ::mstd::__compressed_pair_padding<T1> MSTD_CONCAT3(__padding1_, __LINE__, _);        \
      [[no_unique_address]] T2 Initializer2;                                                                       \
      [[no_unique_address]] ::mstd::__compressed_pair_padding<T2> MSTD_CONCAT3(__padding2_, __LINE__, _)

#    define MSTD_COMPRESSED_TRIPLE(T1, Initializer1, T2, Initializer2, T3, Initializer3)                            \
      [[no_unique_address]]                                                                                        \
      __attribute__((__aligned__(::mstd::__compressed_pair_alignment<T2>),                                              \
                     __aligned__(::mstd::__compressed_pair_alignment<T3>))) T1 Initializer1;                            \
      [[no_unique_address]] ::mstd::__compressed_pair_padding<T1> MSTD_CONCAT3(__padding1_, __LINE__, _);        \
      [[no_unique_address]] T2 Initializer2;                                                                       \
      [[no_unique_address]] ::mstd::__compressed_pair_padding<T2> MSTD_CONCAT3(__padding2_, __LINE__, _);        \
      [[no_unique_address]] T3 Initializer3;                                                                       \
      [[no_unique_address]] ::mstd::__compressed_pair_padding<T3> MSTD_CONCAT3(__padding3_, __LINE__, _)
#  else
#    define MSTD_COMPRESSED_PAIR(T1, Initializer1, T2, Initializer2)                                                \
      struct {                                                                                                         \
        [[no_unique_address]] T1 Initializer1;                                                                     \
        [[no_unique_address]] ::mstd::__compressed_pair_padding<T1> MSTD_CONCAT3(__padding1_, __LINE__, _);      \
        [[no_unique_address]] T2 Initializer2;                                                                     \
        [[no_unique_address]] ::mstd::__compressed_pair_padding<T2> MSTD_CONCAT3(__padding2_, __LINE__, _);      \
      }

#    define MSTD_COMPRESSED_TRIPLE(T1, Initializer1, T2, Initializer2, T3, Initializer3)                            \
      struct {                                                                                                         \
        [[no_unique_address]] T1 Initializer1;                                                                     \
        [[no_unique_address]] ::mstd::__compressed_pair_padding<T1> MSTD_CONCAT3(__padding1_, __LINE__, _);      \
        [[no_unique_address]] T2 Initializer2;                                                                     \
        [[no_unique_address]] ::mstd::__compressed_pair_padding<T2> MSTD_CONCAT3(__padding2_, __LINE__, _);      \
        [[no_unique_address]] T3 Initializer3;                                                                     \
        [[no_unique_address]] ::mstd::__compressed_pair_padding<T3> MSTD_CONCAT3(__padding3_, __LINE__, _);      \
      }
#  endif

#else
#  define MSTD_COMPRESSED_ELEMENT(T1, Initializer1) [[no_unique_address]] T1 Initializer1

#  define MSTD_COMPRESSED_PAIR(T1, Name1, T2, Name2)                                                                \
    [[no_unique_address]] T1 Name1;                                                                                \
    [[no_unique_address]] T2 Name2

#  define MSTD_COMPRESSED_TRIPLE(T1, Name1, T2, Name2, T3, Name3)                                                   \
    [[no_unique_address]] T1 Name1;                                                                                \
    [[no_unique_address]] T2 Name2;                                                                                \
    [[no_unique_address]] T3 Name3
#endif // MSTD_ABI_NO_COMPRESSED_PAIR_PADDING

} // namespace mstd

#endif // MSTD_MEMORY_COMPRESSED_PAIR_HPP
