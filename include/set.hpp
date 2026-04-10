// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_SET
#define MSTD_SET

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <memory>
#include <detail/node_handle.hpp>
#include <detail/iterator/iterator_traits.hpp>
#include <ranges>
#include <detail/tree.hpp>
#include <type_traits>
#include <detail/iterator/erase_if_container.hpp>
#include <detail/type_traits/is_allocator.hpp>
#include <detail/ranges/container_compatible_range.hpp>
#include <detail/utility/compare_three_way.hpp>
#include <memory_resource>
#include <utility>
#include <version>
#include <compare>
#include <initializer_list>
#include <concepts>

namespace mstd {

template <class KeyT, class CompareT = CompareThreeWay, class AllocatorT = std::allocator<KeyT> >
class multiset;

template <class KeyT, class CompareT = CompareThreeWay, class AllocatorT = std::allocator<KeyT> >
class set {
public:
    // types:
    using key_type        = KeyT;
    using value_type      = key_type ;
    using key_compare     = std::type_identity_t<CompareT>;
    using value_compare   = key_compare;
    using allocator_type  = std::type_identity_t<AllocatorT>;
    using reference       = value_type&;
    using const_reference = const value_type&;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

private:
    using Tree_        = Tree<value_type, value_compare, allocator_type>;
    using AllocTraits_ = std::allocator_traits<allocator_type> ;

    static_assert(__check_valid_allocator<allocator_type>::value, "");

    Tree_ tree_;

public:
    using pointer                = Tree_::pointer;
    using const_pointer          = Tree_::const_pointer;
    using size_type              = Tree_::size_type;
    using difference_type        = Tree_::difference_type;
    using iterator               = Tree_::iterator;
    using const_iterator         = Tree_::const_iterator;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    template <class Self>
    using SelfIterator = std::conditional_t<std::is_const_v<Self>, const_iterator, iterator>;
    template <class Self>
    using SelfSubrange = std::pair<SelfIterator<Self>, SelfIterator<Self>>;

    using node_type          = SetNodeHandle<typename Tree_::node, allocator_type>;
    using insert_return_type = __insert_return_type<iterator, node_type>;

    template <class /*Key*/, class /*Compare*/, class /*Allocator*/>
    friend class set;

    template <class /*Key*/, class /*Compare*/, class /*Allocator*/>
    friend class multiset;

    set() noexcept(
        std::is_nothrow_default_constructible_v<allocator_type>
     && std::is_nothrow_default_constructible_v<key_compare>
     && std::is_nothrow_copy_constructible_v<key_compare>)
    : tree_(value_compare()) {}

    explicit set(const value_compare& comp) noexcept(
        std::is_nothrow_default_constructible_v<allocator_type>
     && std::is_nothrow_copy_constructible_v<key_compare>
    )
    : tree_(comp) {}

    explicit set(const value_compare& comp, const allocator_type& alloc)
    : tree_(comp, alloc) {}

    template <class InputIteratorT>
    set(InputIteratorT begin, InputIteratorT end, const value_compare& comp = value_compare())
    : tree_(comp) {
        insert(begin, end);
    }

    template <class InputIteratorT>
    set(InputIteratorT begin, InputIteratorT end, const value_compare& comp, const allocator_type& alloc)
    : tree_(comp, alloc) {
        insert(begin, end);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    set(std::from_range_t,
        RangeT&& range,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type())
    : tree_(comp, alloc) {
        insert_range(std::forward<RangeT>(range));
    }
    
    template <class InputIteratorT>
    set(InputIteratorT begin, InputIteratorT end, const allocator_type& alloc)
    : set(begin, end, key_compare(), alloc) {}
    
    template <_ContainerCompatibleRange<value_type> RangeT>
    set(std::from_range_t, RangeT&& range, const allocator_type& alloc)
    : set(std::from_range, std::forward<RangeT>(range), key_compare(), alloc) {}
    
    set(const set& other) = default;

    set& operator=(const set& other) = default;

    set(set&& other) = default;

    explicit set(const allocator_type& alloc)
    : tree_(alloc) {}

    set(const set& other, const allocator_type& alloc)
    : tree_(other.tree_, alloc) {}

    set(set&& other, const allocator_type& alloc)
    : tree_(std::move(other.tree_), alloc) {}

    set(std::initializer_list<value_type> init_list, const value_compare& comp = value_compare())
    : tree_(comp) {
        insert(init_list.begin(), init_list.end());
    }

    set(std::initializer_list<value_type> init_list, const value_compare& comp, const allocator_type& alloc)
    : tree_(comp, alloc) {
        insert(init_list.begin(), init_list.end());
    }

    set(std::initializer_list<value_type> init_list, const allocator_type& alloc)
    : set(init_list, key_compare(), alloc) {}

    set& operator=(std::initializer_list<value_type> init_list) {
        clear();
        insert(init_list.begin(), init_list.end());
        return *this;
    }

    set& operator=(set&& other) = default;

    ~set() {
        static_assert(sizeof(mstd::__diagnose_non_const_comparator<KeyT, CompareT>()));
    }

    [[nodiscard]] iterator begin() noexcept { return tree_.begin(); }

    [[nodiscard]] const_iterator begin() const noexcept {
        return tree_.begin();
    }

    [[nodiscard]] iterator end() noexcept {
        return tree_.end();
    }

    [[nodiscard]] const_iterator end() const noexcept {
        return tree_.end();
    }

    [[nodiscard]] reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    [[nodiscard]] reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    [[nodiscard]] const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    [[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] const_iterator cend() const noexcept { return end(); }
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }
    [[nodiscard]] const_reverse_iterator crend() const noexcept {
        return rend();
    }

    [[nodiscard]] bool empty() const noexcept { return tree_.size() == 0; }
    [[nodiscard]] size_type size() const noexcept { return tree_.size(); }
    [[nodiscard]] size_type max_size() const noexcept {
        return tree_.max_size();
    }

    // modifiers:
    template <class... ArgsT>
    std::pair<iterator, bool> emplace(ArgsT&&... args) {
        return tree_.emplaceUnique(std::forward<ArgsT>(args)...);
    }

    template <class... ArgsT>
    iterator emplace_hint(const_iterator pos, ArgsT&&... args) {
        return tree_.emplaceHintUnique(pos, std::forward<ArgsT>(args)...).first;
    }

    std::pair<iterator, bool> insert(const value_type& val) {
        return tree_.emplaceUnique(val);
    }

    iterator insert(const_iterator pos, const value_type& val) {
        return tree_.emplaceHintUnique(pos, val).first;
    }

    template <class InputIteratorT>
    void insert(InputIteratorT first, InputIteratorT last) {
        tree_.insertRangeUnique(first, last);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    void insert_range(RangeT&& range) {
        tree_.insertRangeUnique(std::ranges::begin(range), std::ranges::end(range));
    }
    
    std::pair<iterator, bool> insert(value_type&& val) {
        return tree_.emplaceUnique(std::move(val));
    }

    iterator insert(const_iterator pos, value_type&& val) {
        return tree_.emplaceHintUnique(pos, std::move(val)).first;
    }

    void insert(std::initializer_list<value_type> init_list) {
        insert(init_list.begin(), init_list.end());
    }
    
    iterator erase(const_iterator pos) {
        return tree_.erase(pos);
    }

    size_type erase(const key_type& key) {
        return tree_.eraseUnique(key);
    }

    iterator erase(const_iterator first, const_iterator last) {
        return tree_.erase(first, last);
    }

    void clear() noexcept {
        tree_.clear();
    }

    insert_return_type insert(node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            nh.empty() || (nh.get_allocator() == get_allocator()),
            "node_type with incompatible allocator passed to set::insert()"
        );
        return tree_.template nodeHandleInsertUnique<node_type, insert_return_type>(std::move(nh));
    }

    iterator insert(const_iterator hint, node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            nh.empty() || (nh.get_allocator() == get_allocator()),
            "node_type with incompatible allocator passed to set::insert()"
        );
        return tree_.template nodeHandleInsertUnique<node_type>(hint, std::move(nh));
    }

    [[nodiscard]] node_type extract(key_type const& key) {
        return tree_.template nodeHandleExtract<node_type>(key);
    }

    [[nodiscard]] node_type extract(const_iterator iter) {
        return tree_.template nodeHandleExtract<node_type>(iter);
    }

    template <class CompareT2>
    void merge(set<key_type, CompareT2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        tree_.nodeHandleMergeUnique(source.tree_);
    }

    template <class CompareT2>
    void merge(set<key_type, CompareT2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        tree_.nodeHandleMergeUnique(source.tree_);
    }

    template <class CompareT2>
    void merge(multiset<key_type, CompareT2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        tree_.nodeHandleMergeUnique(source.tree_);
    }

    template <class CompareT2>
    void merge(multiset<key_type, CompareT2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        tree_.nodeHandleMergeUnique(source.tree_);
    }
    
    void swap(set& other) noexcept(std::is_nothrow_swappable_v<Tree_>) {
        tree_.swap(other.tree_);
    }

    [[nodiscard]] allocator_type get_allocator() const noexcept {
        return tree_.alloc();
    }

    [[nodiscard]] key_compare key_comp() const {
        return tree_.value_comp();
    }

    [[nodiscard]] value_compare value_comp() const {
        return tree_.value_comp();
    }

    // set operations:
    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfIterator<Self> find(this Self& self, const TransparentKey& transparent_key) {
        return self.tree_.find(transparent_key);
    }

    template <typename TransparentKey>
    [[nodiscard]] size_type count(const TransparentKey& transparent_key) const {
        return tree_.countMulti(transparent_key);
    }

    template <typename TransparentKey>
    [[nodiscard]] bool contains(const TransparentKey& transparent_key) const {
        return find(transparent_key) != end();
    }

    template <class Self>
    [[nodiscard]] SelfIterator<Self> lower_bound(this Self& self, const key_type& key) {
        return self.tree_.lowerBoundUnique(key);
    }

    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfIterator<Self> lower_bound(this Self& self, const TransparentKey& transparent_key) {
        return self.tree_.lowerBoundMulti(transparent_key);
    }

    template <class Self>
    [[nodiscard]] SelfIterator<Self> upper_bound(this Self& self, const key_type& key) {
        return self.tree_.upperBoundUnique(key);
    }

    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfIterator<Self> upper_bound(this Self& self, const TransparentKey& transparent_key) {
        return self.tree_.upperBoundMulti(transparent_key);
    }

    template <class Self>
    [[nodiscard]] SelfSubrange<Self> equal_range(this Self& self, const key_type& key) {
        return self.tree_.equalRangeUnique(key);
    }

    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfSubrange<Self> equal_range(this Self& self, const TransparentKey& transparent_key) {
        return self.tree_.equalRangeMulti(transparent_key);
    }

    template <class, class...>
    friend struct __specialized_algorithm;
};

template <
    class InputIteratorT
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<std::iter_value_t<InputIteratorT>>
>
requires __has_input_iterator_category<InputIteratorT>
      && __is_allocator_v<AllocatorT>
      && (!__is_allocator_v<CompareT>)
set(InputIteratorT, InputIteratorT, CompareT = CompareT(), AllocatorT = AllocatorT())
-> set<std::iter_value_t<InputIteratorT>, CompareT, AllocatorT>;

template <
    std::ranges::input_range RangeT
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<std::ranges::range_value_t<RangeT>>
>
requires __is_allocator_v<AllocatorT> && (!__is_allocator_v<CompareT>)
set(std::from_range_t, RangeT&&, CompareT = CompareT(), AllocatorT = AllocatorT())
-> set<std::ranges::range_value_t<RangeT>, CompareT, AllocatorT>;

template <
    class KeyT
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<KeyT>
>
requires __is_allocator_v<AllocatorT> && (!__is_allocator_v<CompareT>)
set(std::initializer_list<KeyT>, CompareT = CompareT(), AllocatorT = AllocatorT())
-> set<KeyT, CompareT, AllocatorT>;

template <class InputIteratorT, class AllocatorT>
requires __has_input_iterator_category<InputIteratorT> && __is_allocator_v<AllocatorT>
set(InputIteratorT, InputIteratorT, AllocatorT)
-> set<std::iter_value_t<InputIteratorT>, CompareThreeWay, AllocatorT>;

template <std::ranges::input_range RangeT, class AllocatorT>
requires __is_allocator_v<AllocatorT>
set(std::from_range_t, RangeT&&, AllocatorT)
-> set<std::ranges::range_value_t<RangeT>, CompareThreeWay, AllocatorT>;

template <class KeyT, class AllocatorT>
requires __is_allocator_v<AllocatorT>
set(std::initializer_list<KeyT>, AllocatorT) -> set<KeyT, CompareThreeWay, AllocatorT>;

template <class KeyT, class CompareT, class AllocatorT>
inline bool
operator==(const set<KeyT, CompareT, AllocatorT>& lhs, const set<KeyT, CompareT, AllocatorT>& rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class KeyT, class CompareT, class AllocatorT>
auto
operator<=>(const set<KeyT, CompareT, AllocatorT>& lhs, const set<KeyT, CompareT, AllocatorT>& rhs) {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), CompareT{});
}

// specialized algorithms:
template <class KeyT, class CompareT, class AllocatorT>
inline void swap(set<KeyT, CompareT, AllocatorT>& lhs, set<KeyT, CompareT, AllocatorT>& rhs)
noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template <class KeyT, class CompareT, class AllocatorT, class PredicateT>
inline typename set<KeyT, CompareT, AllocatorT>::size_type
erase_if(set<KeyT, CompareT, AllocatorT>& container, PredicateT pred) {
    return mstd::erase_if_container(container, pred);
}

template <class KeyT, class CompareT, class AllocatorT>
class multiset {
public:
    // types:
    using key_type = KeyT;
    using value_type = key_type;
    using key_compare = std::type_identity_t<CompareT>;
    using value_compare = key_compare;
    using allocator_type = std::type_identity_t<AllocatorT>;
    using reference = value_type&;
    using const_reference = const value_type&;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

private:
    using Tree_        = Tree<value_type, value_compare, allocator_type>;
    using AllocTraits_ = std::allocator_traits<allocator_type> ;

    static_assert(__check_valid_allocator<allocator_type>::value);

    Tree_ tree_;

public:
    using pointer                = Tree_::pointer;
    using const_pointer          = Tree_::const_pointer;
    using size_type              = Tree_::size_type;
    using difference_type        = Tree_::difference_type;
    using iterator               = Tree_::const_iterator;
    using const_iterator         = Tree_::const_iterator;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using node_type              = SetNodeHandle<typename Tree_::node, allocator_type>;

    template <class Self>
    using SelfIterator = std::conditional_t<std::is_const_v<Self>, const_iterator, iterator>;
    template <class Self>
    using SelfSubrange = std::pair<SelfIterator<Self>, SelfIterator<Self>>;

    template <class /*Key*/, class /*Compare*/, class /*Alloc*/>
    friend class set;
    template <class /*Key2*/, class /*Compare*/, class /*Alloc*/>
    friend class multiset;

    // construct/copy/destroy:
    multiset() noexcept(
        std::is_nothrow_default_constructible_v<allocator_type>
     && std::is_nothrow_default_constructible_v<key_compare>
     && std::is_nothrow_copy_constructible_v<key_compare>
    )
    : tree_(value_compare()) {}

    explicit multiset(const value_compare& comp) noexcept(
        std::is_nothrow_default_constructible_v<allocator_type>
     && std::is_nothrow_copy_constructible_v<key_compare>
    )
    : tree_(comp) {}

    explicit multiset(const value_compare& comp, const allocator_type& alloc)
    : tree_(comp, alloc) {}

    template <class InputIteratorT>
    multiset(InputIteratorT begin, InputIteratorT end, const value_compare& comp = value_compare())
    : tree_(comp) {
        insert(begin, end);
    }

    template <class InputIteratorT>
    multiset(InputIteratorT begin, InputIteratorT end, const allocator_type& alloc)
    : multiset(begin, end, key_compare(), alloc) {}

    template <class InputIteratorT>
    multiset(InputIteratorT begin, InputIteratorT end, const value_compare& comp, const allocator_type& alloc)
    : tree_(comp, alloc) {
        insert(begin, end);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    multiset(std::from_range_t,
             RangeT&& range,
             const key_compare& comp = key_compare(),
             const allocator_type& alloc = allocator_type())
    : tree_(comp, alloc) {
        insert_range(std::forward<RangeT>(range));
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    multiset(std::from_range_t, RangeT&& range, const allocator_type& alloc)
    : multiset (std::from_range, std::forward<RangeT>(range), key_compare(), alloc) {}
    
    multiset(const multiset& other) = default;

    multiset& operator=(const multiset& other) = default;

    multiset(multiset&& other) = default;

    multiset(multiset&& other, const allocator_type& alloc)
    : tree_(std::move(other.tree_), alloc) {}

    explicit multiset(const allocator_type& alloc) : tree_(alloc) {}
    multiset(const multiset& other, const allocator_type& alloc)
    : tree_(other.tree_, alloc) {}

    multiset(std::initializer_list<value_type> init_list, const value_compare& comp = value_compare())
    : tree_(comp) {
        insert(init_list.begin(), init_list.end());
    }

    multiset(std::initializer_list<value_type> init_list, const value_compare& comp, const allocator_type& alloc)
    : tree_(comp, alloc) {
        insert(init_list.begin(), init_list.end());
    }

    multiset(std::initializer_list<value_type> init_list, const allocator_type& alloc)
    : multiset(init_list, key_compare(), alloc) {}

    multiset& operator=(std::initializer_list<value_type> init_list) {
        clear();
        insert(init_list.begin(), init_list.end());
        return *this;
    }

    multiset& operator=(multiset&& other) = default;

    ~multiset() {
        static_assert(sizeof(mstd::__diagnose_non_const_comparator<KeyT, CompareT>()), "");
    }

    [[nodiscard]] iterator begin() noexcept { return tree_.begin(); }
    [[nodiscard]] const_iterator begin() const noexcept { return tree_.begin(); }
    [[nodiscard]] iterator end() noexcept { return tree_.end(); }
    [[nodiscard]] const_iterator end() const noexcept { return tree_.end(); }

    [[nodiscard]] reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    [[nodiscard]] reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    [[nodiscard]] const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    [[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] const_iterator cend() const noexcept { return end(); }
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    [[nodiscard]] const_reverse_iterator crend() const noexcept {
        return rend();
    }

    [[nodiscard]] bool empty() const noexcept { return tree_.size() == 0; }
    [[nodiscard]] size_type size() const noexcept { return tree_.size(); }
    [[nodiscard]] size_type max_size() const noexcept { return tree_.max_size(); }

    // modifiers:
    template <class... ArgsT>
    iterator emplace(ArgsT&&... args) {
        return tree_.emplaceMulti(std::forward<ArgsT>(args)...);
    }

    template <class... ArgsT>
    iterator emplace_hint(const_iterator pos, ArgsT&&... args) {
        return tree_.emplaceHintMulti(pos, std::forward<ArgsT>(args)...);
    }

    iterator insert(const value_type& value) {
        return tree_.emplaceMulti(value);
    }

    iterator insert(const_iterator pos, const value_type& value) {
        return tree_.emplaceHintMulti(pos, value);
    }

    template <class InputIteratorT>
    void insert(InputIteratorT begin, InputIteratorT end) {
        tree_.insertRangeMulti(begin, end);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    void insert_range(RangeT&& range) {
        tree_.insertRangeMulti(std::ranges::begin(range), std::ranges::end(range));
    }
    
    iterator insert(value_type&& value) { return tree_.emplaceMulti(std::move(value)); }

    iterator insert(const_iterator pos, value_type&& value) {
        return tree_.emplaceHintMulti(pos, std::move(value));
    }

    void insert(std::initializer_list<value_type> init_list) {
        insert(init_list.begin(), init_list.end());
    }

    iterator erase(const_iterator pos) { return tree_.erase(pos); }

    size_type erase(const key_type& key) { return tree_.eraseMulti(key); }

    iterator erase(const_iterator begin, const_iterator end) {
        return tree_.erase(begin, end);
    }

    void clear() noexcept { tree_.clear(); }

    iterator insert(node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            nh.empty() || nh.get_allocator() == get_allocator(),
            "node_type with incompatible allocator passed to multiset::insert()"
        );
        return tree_.template nodeHandleInsertMulti<node_type>(std::move(nh));
    }

    iterator insert(const_iterator hint, node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            nh.empty() || nh.get_allocator() == get_allocator(),
            "node_type with incompatible allocator passed to multiset::insert()"
        );
        return tree_.template nodeHandleInsertMulti<node_type>(hint, std::move(nh));
    }

    [[nodiscard]] node_type extract(key_type const& key) {
        return tree_.template nodeHandleExtract<node_type>(key);
    }

    [[nodiscard]] node_type extract(const_iterator pos) {
        return tree_.template nodeHandleExtract<node_type>(pos);
    }

    template <class CompareT2>
    void merge(multiset<key_type, CompareT2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        tree_.nodeHandleMergeMulti(source.tree_);
    }

    template <class CompareT2>
    void merge(multiset<key_type, CompareT2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        tree_.nodeHandleMergeMulti(source.tree_);
    }

    template <class CompareT2>
    void merge(set<key_type, CompareT2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), 
            "merging container with incompatible allocator"
        );
        tree_.nodeHandleMergeMulti(source.tree_);
    }

    template <class CompareT2>
    void merge(set<key_type, CompareT2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        tree_.nodeHandleMergeMulti(source.tree_);
    }

    void swap(multiset& other) noexcept(std::is_nothrow_swappable_v<Tree_>) {
        tree_.swap(other.tree_);
    }

    [[nodiscard]] allocator_type get_allocator() const noexcept { return tree_.alloc(); }
    [[nodiscard]] key_compare key_comp() const { return tree_.value_comp(); }
    [[nodiscard]] value_compare value_comp() const { return tree_.value_comp(); }

    // set operations:
    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfIterator<Self> find(this Self& self, const TransparentKey& transparent_key) {
        return self.tree_.find(transparent_key);
    }

    template <typename TransparentKey>
    [[nodiscard]] size_type count(const TransparentKey& transparent_key) const {
        return tree_.countMulti(transparent_key);
    }

    template <typename TransparentKey>
    [[nodiscard]] bool contains(const TransparentKey& transparent_key) const {
        return find(transparent_key) != end();
    }

    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfIterator<Self> lower_bound(this Self& self, const TransparentKey& transparent_key) {
        return self.tree_.lowerBoundMulti(transparent_key);
    }

    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfIterator<Self> upper_bound(this Self& self, const TransparentKey& transparent_key) {
        return self.tree_.upperBoundMulti(transparent_key);
    }

    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfSubrange<Self> equal_range(this Self& self, const TransparentKey& transparent_key) {
        return self.tree_.equalRangeMulti(transparent_key);
    }
    
    template <class, class...>
    friend struct __specialized_algorithm;
};

template <
    class InputIteratorT
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<std::iter_value_t<InputIteratorT>>
>
requires __has_input_iterator_category<InputIteratorT>
      && __is_allocator_v<AllocatorT>
      && (!__is_allocator_v<CompareT>)
multiset(InputIteratorT, InputIteratorT, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multiset<std::iter_value_t<InputIteratorT>, CompareT, AllocatorT>;

template <
    std::ranges::input_range RangeT
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<std::ranges::range_value_t<RangeT>>
>
requires __is_allocator_v<AllocatorT> && (!__is_allocator_v<CompareT>)
multiset(std::from_range_t, RangeT&&, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multiset<std::ranges::range_value_t<RangeT>, CompareT, AllocatorT>;

template <class KeyT, class CompareT = CompareThreeWay, class AllocatorT = std::allocator<KeyT>>
requires __is_allocator_v<AllocatorT> && (!__is_allocator_v<CompareT>)
multiset(std::initializer_list<KeyT>, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multiset<KeyT, CompareT, AllocatorT>;

template <class InputIteratorT, class AllocatorT>
requires (__has_input_iterator_category<InputIteratorT> && __is_allocator_v<AllocatorT>)
multiset(InputIteratorT, InputIteratorT, AllocatorT)
-> multiset<
       std::iter_value_t<InputIteratorT>
     , CompareThreeWay
     , AllocatorT
   >;

template <std::ranges::input_range RangeT, class AllocatorT>
requires __is_allocator_v<AllocatorT>
multiset(std::from_range_t, RangeT&&, AllocatorT)
-> multiset<
       std::ranges::range_value_t<RangeT>
     , CompareThreeWay
     , AllocatorT
   >;

template <class KeyT, class AllocatorT>
requires __is_allocator_v<AllocatorT>
multiset(std::initializer_list<KeyT>, AllocatorT)
-> multiset<KeyT, CompareThreeWay, AllocatorT>;

template <class KeyT, class CompareT, class AllocatorT>
inline bool
operator==(const multiset<KeyT, CompareT, AllocatorT>& lhs, const multiset<KeyT, CompareT, AllocatorT>& rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class KeyT, class CompareT, class AllocatorT>
auto
operator<=>(const multiset<KeyT, CompareT, AllocatorT>& lhs, const multiset<KeyT, CompareT, AllocatorT>& rhs) {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), CompareT{});
}

template <class KeyT, class CompareT, class AllocatorT>
inline void
swap(multiset<KeyT, CompareT, AllocatorT>& lhs, multiset<KeyT, CompareT, AllocatorT>& rhs)
noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template <class KeyT, class CompareT, class AllocatorT, class PredicateT>
inline typename multiset<KeyT, CompareT, AllocatorT>::size_type
erase_if(multiset<KeyT, CompareT, AllocatorT>& container, PredicateT pred) {
    return mstd::erase_if_container(container, pred);
}

namespace pmr {
    template <class _KeyT, class _CompareT = CompareThreeWay>
    using set = mstd::set<_KeyT, _CompareT, std::pmr::polymorphic_allocator<_KeyT>>;

    template <class _KeyT, class _CompareT = CompareThreeWay>
    using multiset = mstd::multiset<_KeyT, _CompareT, std::pmr::polymorphic_allocator<_KeyT>>;
} // namespace pmr
} // namespace mstd


#endif // MSTD_SET
