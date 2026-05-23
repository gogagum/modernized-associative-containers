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
#include <ranges>
#include <detail/tree.hpp>
#include <type_traits>
#include <detail/iterator/erase_if_container.hpp>
#include <detail/concepts/allocator_concept.hpp>
#include <detail/concepts/compare_concepts.hpp>
#include <detail/concepts/container_compatible_range.hpp>
#include <utility/compare_three_way.hpp>
#include <memory_resource>
#include <utility>
#include <version>
#include <initializer_list>

namespace mstd {

template <
    class KeyT
  , OrdersAtLeastWeakly<KeyT> CompareT = compare_three_way
  , Allocator AllocatorT = std::allocator<KeyT>
>
class multiset;

template <
    class KeyT
  , OrdersAtLeastWeakly<KeyT> CompareT = compare_three_way
  , Allocator AllocatorT               = std::allocator<KeyT>
>
class set {
public:
    // types:
    using key_type        = KeyT;
    using value_type      = key_type ;
    using key_compare     = std::type_identity_t<CompareT>;
    using allocator_type  = std::type_identity_t<AllocatorT>;
    using reference       = value_type&;
    using const_reference = const value_type&;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

private:
    using Tree_        = Tree<value_type, std::identity, key_compare, allocator_type>;
    using AllocTraits_ = std::allocator_traits<allocator_type>;

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
    using node_type                      = NodeHandle<typename Tree_::node, allocator_type>;
    using node_handle_insert_return_type = NodeHandleInsertReturnType<iterator, node_type>;
    using insert_return_type             = InsertReturnType<iterator>;

    template <class Key2, OrdersAtLeastWeakly<Key2> CompareT2, Allocator AllocatorT2>
    friend class set;

    template <class Key2, OrdersAtLeastWeakly<Key2> CompareT2, Allocator AllocatorT2>
    friend class multiset;

    set() noexcept(
        std::is_nothrow_default_constructible_v<allocator_type>
     && std::is_nothrow_default_constructible_v<key_compare>
     && std::is_nothrow_copy_constructible_v<key_compare>)
    : tree_(key_compare()) {}

    explicit set(const key_compare& comp) noexcept(
        std::is_nothrow_default_constructible_v<allocator_type>
     && std::is_nothrow_copy_constructible_v<key_compare>
    )
    : tree_(comp) {}

    explicit set(const key_compare& comp, const allocator_type& alloc)
    : tree_(comp, alloc) {}

    template <
        _ContainerCompatibleIterator<value_type> IteratorT
      , std::sentinel_for<IteratorT> SentinelT
    >
    set(IteratorT begin, SentinelT end, const key_compare& comp = key_compare())
    : tree_(comp) { insert(begin, end); }

    template <
        _ContainerCompatibleIterator<value_type> IteratorT
      , std::sentinel_for<IteratorT> SentinelT
    >
    set(IteratorT begin,
        SentinelT end,
        const key_compare& comp,
        const allocator_type& alloc)
    : tree_(comp, alloc) { insert(begin, end); }

    template <_ContainerCompatibleRange<value_type> RangeT>
    set(RangeT&& range,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type())
    : tree_(comp, alloc) { insert(std::forward<RangeT>(range)); }
    
    template <
        _ContainerCompatibleIterator<value_type> IteratorT
      , std::sentinel_for<IteratorT> SentinelT
    >
    set(IteratorT begin, SentinelT end, const allocator_type& alloc)
    : set(begin, end, key_compare(), alloc) {}
    
    template <_ContainerCompatibleRange<value_type> RangeT>
    set(RangeT&& range, const allocator_type& alloc)
    : set(std::forward<RangeT>(range), key_compare(), alloc) {}
    
    set(const set& other) = default;

    set& operator=(const set& other) = default;

    set(set&& other) = default;

    explicit set(const allocator_type& alloc)
    : tree_(alloc) {}

    set(const set& other, const allocator_type& alloc)
    : tree_(other.tree_, alloc) {}

    set(set&& other, const allocator_type& alloc)
    : tree_(std::move(other.tree_), alloc) {}

    set(std::initializer_list<value_type> init_list,
        const key_compare& comp = key_compare())
    : tree_(comp) {
        insert(init_list.begin(), init_list.end());
    }

    set(std::initializer_list<value_type> init_list,
        const key_compare& comp,
        const allocator_type& alloc)
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

    ~set() = default;

    template <class Self>
    [[nodiscard]] auto begin(this Self& self) noexcept {
        return self.tree_.begin();
    }

    template <class Self>
    [[nodiscard]] auto end(this Self& self) noexcept {
        return self.tree_.end();
    }

    template <class Self>
    [[nodiscard]] auto rbegin(this Self& self) noexcept {
        return std::reverse_iterator(self.end());
    }

    template <class Self>
    [[nodiscard]] auto rend(this Self& self) noexcept {
        return std::reverse_iterator<decltype(self.begin())>(self.begin());
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
    insert_return_type emplace(ArgsT&&... args) {
        return tree_.emplaceUnique(std::forward<ArgsT>(args)...);
    }

    template <class... ArgsT>
    iterator emplace_hint(const_iterator pos, ArgsT&&... args) {
        return tree_
            .emplaceHintUnique(pos, std::forward<ArgsT>(args)...)
            .position;
    }

    template <std::convertible_to<value_type> TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    insert_return_type insert(TransparentKey&& val) {
        return tree_.emplaceUnique(std::forward<TransparentKey>(val));
    }

    template <std::convertible_to<value_type> TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    iterator insert(const_iterator pos, TransparentKey&& val) {
        return tree_
            .emplaceHintUnique(pos, std::forward<TransparentKey>(val))
            .position;
    }

    template <
        _ContainerCompatibleIterator<value_type> IteratorT
      , std::sentinel_for<IteratorT> Sentinel
    >
    void insert(IteratorT first, Sentinel last) {
        tree_.insertRangeUnique(first, last);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    void insert(RangeT&& range) {
        tree_.insertRangeUnique(
            std::ranges::begin(range),
            std::ranges::end(range)
        );
    }

    //void insert(std::initializer_list<value_type> init_list) {
    //    insert(init_list.begin(), init_list.end());
    //}
    
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

    node_handle_insert_return_type insert(node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            nh.empty() || (nh.get_allocator() == get_allocator()),
            "node_type with incompatible allocator passed to set::insert()"
        );
        return tree_.template nodeHandleInsertUnique<node_type>(std::move(nh));
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
        return tree_.key_comp();
    }

    // set operations:
    template <
        class Self
      , typename TransparentKey
    > requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] auto find(this Self& self, const TransparentKey& k) {
        return self.tree_.find(k);
    }

    template <typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] size_type count(const TransparentKey& k) const {
        if constexpr (std::is_same_v<std::remove_cvref_t<TransparentKey>, key_type>) {
            return tree_.countUnique(k);
        } else {
            return tree_.countMulti(k);
        }
    }

    template <typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] bool contains(const TransparentKey& k) const {
        return find(k) != end();
    }

    template <class Self, typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] auto lower_bound(this Self& self, const TransparentKey& k) {
        if constexpr (std::same_as<std::remove_cvref_t<TransparentKey>, key_type>) {
            return self.tree_.lowerBoundUnique(k);
        } else {
            return self.tree_.lowerBoundMulti(k);
        }
    }

    template <class Self, typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] auto upper_bound(this Self& self, const TransparentKey& k) {
        if constexpr (std::same_as<std::remove_cvref_t<TransparentKey>, key_type>) {
            return self.tree_.upperBoundUnique(k);
        } else {
            return self.tree_.upperBoundMulti(k);
        }
    }

    template <class Self, typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] auto equal_range(this Self& self, const TransparentKey& k) {
        if constexpr (std::same_as<std::remove_cvref_t<TransparentKey>, key_type>) {
            return self.tree_.equalRangeUnique(k);
        } else {
            return self.tree_.equalRangeMulti(k);
        }
    }
};

template <
    std::input_iterator IteratorT
  , OrdersAtLeastWeakly<std::iter_value_t<IteratorT>> CompareT = compare_three_way
  , Allocator AllocatorT = std::allocator<std::iter_value_t<IteratorT>>
>
set(IteratorT, IteratorT, CompareT = CompareT(), AllocatorT = AllocatorT())
-> set<std::iter_value_t<IteratorT>, CompareT, AllocatorT>;

template <
    std::ranges::input_range RangeT
  , OrdersAtLeastWeakly<std::ranges::range_value_t<RangeT>> CompareT = compare_three_way
  , Allocator AllocatorT = std::allocator<std::ranges::range_value_t<RangeT>>
>
set(RangeT&&, CompareT = CompareT(), AllocatorT = AllocatorT())
-> set<std::ranges::range_value_t<RangeT>, CompareT, AllocatorT>;

template <
    class KeyT
  , OrdersAtLeastWeakly<KeyT> CompareT = compare_three_way
  , Allocator AllocatorT = std::allocator<KeyT>
>
set(std::initializer_list<KeyT>, CompareT = CompareT(), AllocatorT = AllocatorT())
-> set<KeyT, CompareT, AllocatorT>;

template <
    std::input_iterator IteratorT
  , std::sentinel_for<IteratorT> Sentinel
  , Allocator AllocatorT
>
set(IteratorT, Sentinel, AllocatorT)
-> set<std::iter_value_t<IteratorT>, compare_three_way, AllocatorT>;

template <std::ranges::input_range RangeT, Allocator AllocatorT>
set(RangeT&&, AllocatorT)
-> set<std::ranges::range_value_t<RangeT>, compare_three_way, AllocatorT>;

template <class KeyT, Allocator AllocatorT>
set(std::initializer_list<KeyT>, AllocatorT) -> set<KeyT, compare_three_way, AllocatorT>;

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

template <
    class KeyT
  , OrdersAtLeastWeakly<KeyT> CompareT
  , Allocator AllocatorT
>
class multiset {
public:
    // types:
    using key_type        = KeyT;
    using value_type      = key_type;
    using key_compare     = std::type_identity_t<CompareT>;
    using allocator_type  = std::type_identity_t<AllocatorT>;
    using reference       = value_type&;
    using const_reference = const value_type&;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

private:
    using Tree_        = Tree<value_type, std::identity, key_compare, allocator_type>;
    using AllocTraits_ = std::allocator_traits<allocator_type> ;

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
    using node_type              = NodeHandle<typename Tree_::node, allocator_type>;

    template <class Key2, OrdersAtLeastWeakly<Key2> CompareT2, Allocator AllocatorT2>
    friend class set;
    template <class Key2, OrdersAtLeastWeakly<Key2> CompareT2, Allocator AllocatorT2>
    friend class multiset;

    // construct/copy/destroy:
    multiset() noexcept(
        std::is_nothrow_default_constructible_v<allocator_type>
     && std::is_nothrow_default_constructible_v<key_compare>
     && std::is_nothrow_copy_constructible_v<key_compare>
    )
    : tree_(key_compare()) {}

    explicit multiset(const key_compare& comp) noexcept(
        std::is_nothrow_default_constructible_v<allocator_type>
     && std::is_nothrow_copy_constructible_v<key_compare>
    )
    : tree_(comp) {}

    explicit multiset(const key_compare& comp, const allocator_type& alloc)
    : tree_(comp, alloc) {}

    template <
        _ContainerCompatibleIterator<value_type> IteratorT
      , std::sentinel_for<IteratorT> SentinelT
    >
    multiset(IteratorT begin,
             SentinelT end,
             const key_compare& comp = key_compare())
    : tree_(comp) {
        insert(begin, end);
    }

    template <
        _ContainerCompatibleIterator<value_type> IteratorT
      , std::sentinel_for<IteratorT> SentinelT
    >
    multiset(IteratorT begin, SentinelT end, const allocator_type& alloc)
    : multiset(begin, end, key_compare(), alloc) {}

    template <
        _ContainerCompatibleIterator<value_type> IteratorT
      , std::sentinel_for<IteratorT> SentinelT
    >
    multiset(IteratorT begin,
             SentinelT end,
             const key_compare& comp,
             const allocator_type& alloc)
    : tree_(comp, alloc) { insert(begin, end); }

    template <_ContainerCompatibleRange<value_type> RangeT>
    multiset(RangeT&& range,
             const key_compare& comp = key_compare(),
             const allocator_type& alloc = allocator_type())
    : tree_(comp, alloc) {
        insert(std::forward<RangeT>(range));
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    multiset(RangeT&& range, const allocator_type& alloc)
    : multiset(std::forward<RangeT>(range), key_compare(), alloc) {}
    
    multiset(const multiset& other) = default;

    multiset& operator=(const multiset& other) = default;

    multiset(multiset&& other) = default;

    multiset(multiset&& other, const allocator_type& alloc)
    : tree_(std::move(other.tree_), alloc) {}

    explicit multiset(const allocator_type& alloc) : tree_(alloc) {}
    multiset(const multiset& other, const allocator_type& alloc)
    : tree_(other.tree_, alloc) {}

    multiset(std::initializer_list<value_type> init_list,
             const key_compare& comp = key_compare())
    : tree_(comp) { insert(init_list.begin(), init_list.end()); }

    multiset(std::initializer_list<value_type> init_list,
             const key_compare& comp,
             const allocator_type& alloc)
    : tree_(comp, alloc) { insert(init_list.begin(), init_list.end());}

    multiset(std::initializer_list<value_type> init_list,
             const allocator_type& alloc)
    : multiset(init_list, key_compare(), alloc) {}

    multiset& operator=(std::initializer_list<value_type> init_list) {
        clear();
        insert(init_list.begin(), init_list.end());
        return *this;
    }

    multiset& operator=(multiset&& other) = default;

    ~multiset() = default;
    
    template <class Self>
    [[nodiscard]] auto begin(this Self& self) noexcept {
        return self.tree_.begin();
    }

    template <class Self>
    [[nodiscard]] auto end(this Self& self) noexcept {
        return self.tree_.end();
    }

    template <class Self>
    [[nodiscard]] auto rbegin(this Self& self) noexcept {
        return std::reverse_iterator(self.end());
    }

    template <class Self>
    [[nodiscard]] auto rend(this Self& self) noexcept {
        return std::reverse_iterator(self.begin());
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

    template <std::convertible_to<value_type> TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, TransparentKey, value_type>
    iterator insert(TransparentKey&& value) {
        return tree_.emplaceMulti(std::forward<TransparentKey>(value));
    }

    template <std::convertible_to<value_type> TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, TransparentKey, value_type>
    iterator insert(const_iterator pos, TransparentKey&& value) {
        return tree_.emplaceHintMulti(pos, std::forward<TransparentKey>(value));
    }

    template <
        _ContainerCompatibleIterator<value_type> IteratorT
      , std::sentinel_for<IteratorT> SentinelT
    >
    void insert(IteratorT begin, SentinelT end) {
        tree_.insertRangeMulti(begin, end);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    void insert(RangeT&& range) {
        tree_.insertRangeMulti(std::ranges::begin(range), std::ranges::end(range));
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
    [[nodiscard]] key_compare key_comp() const { return tree_.key_comp(); }

    // set operations:
    template <class Self, typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] auto find(this Self& self, const TransparentKey& k) {
        return self.tree_.find(k);
    }

    template <typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] size_type count(const TransparentKey& k) const {
        return tree_.countMulti(k);
    }

    template <typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] bool contains(const TransparentKey& k) const {
        return find(k) != end();
    }

    template <class Self, typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] auto lower_bound(this Self& self, const TransparentKey& k) {
        return self.tree_.lowerBoundMulti(k);
    }

    template <class Self, typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] auto upper_bound(this Self& self, const TransparentKey& k) {
        return self.tree_.upperBoundMulti(k);
    }

    template <class Self, typename TransparentKey>
    requires OrdersWithAtLeastWeakly<key_compare, key_type, TransparentKey>
    [[nodiscard]] auto equal_range(this Self& self, const TransparentKey& k) {
        return self.tree_.equalRangeMulti(k);
    }
};

template <
    std::input_iterator IteratorT
  , std::sentinel_for<IteratorT> SentinelT
  , OrdersAtLeastWeakly<std::iter_value_t<IteratorT>> CompareT = compare_three_way
  , Allocator AllocatorT                                       = std::allocator<std::iter_value_t<IteratorT>>
>
multiset(IteratorT, SentinelT, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multiset<std::iter_value_t<IteratorT>, CompareT, AllocatorT>;

template <
    std::ranges::input_range RangeT
  , OrdersAtLeastWeakly<std::ranges::range_value_t<RangeT>> CompareT = compare_three_way
  , Allocator AllocatorT                                             = std::allocator<std::ranges::range_value_t<RangeT>>
>
multiset(RangeT&&, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multiset<std::ranges::range_value_t<RangeT>, CompareT, AllocatorT>;

template <
    class KeyT
  , OrdersAtLeastWeakly<KeyT> CompareT = compare_three_way
  , Allocator AllocatorT = std::allocator<KeyT>
>
multiset(std::initializer_list<KeyT>, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multiset<KeyT, CompareT, AllocatorT>;

template <
    std::input_iterator IteratorT
  , std::sentinel_for<IteratorT> SentinelT
  , Allocator AllocatorT
>
multiset(IteratorT, SentinelT, AllocatorT)
-> multiset<
       std::iter_value_t<IteratorT>
     , compare_three_way
     , AllocatorT
   >;

template <std::ranges::input_range RangeT, Allocator AllocatorT>
multiset(RangeT&&, AllocatorT)
-> multiset<
       std::ranges::range_value_t<RangeT>
     , compare_three_way
     , AllocatorT
   >;

template <class KeyT, Allocator AllocatorT>
multiset(std::initializer_list<KeyT>, AllocatorT)
-> multiset<KeyT, compare_three_way, AllocatorT>;

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
inline void swap(multiset<KeyT, CompareT, AllocatorT>& lhs, multiset<KeyT, CompareT, AllocatorT>& rhs)
noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template <class KeyT, class CompareT, class AllocatorT, class PredicateT>
inline typename multiset<KeyT, CompareT, AllocatorT>::size_type
erase_if(multiset<KeyT, CompareT, AllocatorT>& container, PredicateT pred) {
    return mstd::erase_if_container(container, pred);
}

namespace pmr {
    template <class _KeyT, class _CompareT = compare_three_way>
    using set = mstd::set<_KeyT, _CompareT, std::pmr::polymorphic_allocator<_KeyT>>;

    template <class _KeyT, class _CompareT = compare_three_way>
    using multiset = mstd::multiset<_KeyT, _CompareT, std::pmr::polymorphic_allocator<_KeyT>>;
} // namespace pmr
} // namespace mstd


#endif // MSTD_SET
