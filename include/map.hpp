// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD_MAP
#define MSTD_MAP

#include <algorithm>
#include <cassert>
#include <detail/iterator/erase_if_container.hpp>
#include <iterator>
#include <memory>
#include <detail/iterator/iterator_traits.hpp>
#include <detail/iterator/range_iterator_traits.hpp>
#include <detail/iterator/erase_if_container.hpp>
#include <memory_resource>
#include <detail/node_handle.hpp>
#include <ranges>
#include <detail/tree.hpp>
#include <detail/concepts/allocator_concept.hpp>
#include <detail/concepts/compare_concepts.hpp>
#include <detail/concepts/container_compatible_range.hpp>
#include <type_traits>
#include <utility>
#include <detail/utility/compare_three_way.hpp>
#include <tuple>
#include <version>
#include <initializer_list>
#include <detail/map_value.hpp>

namespace mstd {

template <class KeyT, class ValueT, class CompareT = CompareThreeWay, class AllocatorT = std::allocator<MapValue<KeyT, ValueT>>>
class multimap;

template <class KeyT, class ValueT, class CompareT = CompareThreeWay, class AllocatorT = std::allocator<MapValue<KeyT, ValueT>>>
class map {
public:
    // types:
    using key_type        = KeyT;
    using mapped_type     = ValueT;
    using value_type      = MapValue<KeyT, ValueT>;
    using key_compare     = std::type_identity_t<CompareT>;
    using allocator_type  = std::type_identity_t<AllocatorT>;
    using reference       = value_type&;
    using const_reference = const value_type&;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

private:
    using Tree_         = mstd::Tree<value_type, typename value_type::KeyProj, key_compare, allocator_type>;
    using AllocTraits_  = std::allocator_traits<allocator_type>;

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
    using SelfSubrange = std::ranges::subrange<SelfIterator<Self>>;

    using node_type = NodeHandle<typename Tree_::node, allocator_type>;
    using insert_return_type = InsertReturnType<iterator, node_type>;

    template <class /*Key*/, class /*Value*/, class /*Compare*/, class /*Allocator*/>
    friend class map;
    template <class /*Key*/, class /*Value*/, class /*Compare*/, class /*Allocator*/>
    friend class multimap;

    map() noexcept(
       std::is_nothrow_default_constructible_v<allocator_type>
    && std::is_nothrow_default_constructible_v<key_compare>
    && std::is_nothrow_copy_constructible_v<key_compare>
    )
    : tree_(key_compare()) {}

    explicit map(const key_compare& comp) noexcept(
       std::is_nothrow_default_constructible_v<allocator_type>
    && std::is_nothrow_copy_constructible_v<key_compare>
    )
    : tree_(comp) {}

    explicit map(const key_compare& comp, const allocator_type& alloc)
    : tree_(comp, typename Tree_::allocator_type(alloc)) {}

    template <_ContainerCompatibleIterator<value_type> IteratorT, std::sentinel_for<IteratorT> SentinelT>
    map(IteratorT begin, SentinelT end, const key_compare& comp = key_compare())
    : tree_(ValueCompare_(comp)) {
        insert(begin, end);
    }

    template <_ContainerCompatibleIterator<value_type> IteratorT, std::sentinel_for<IteratorT> SentinelT>
    map(IteratorT begin, SentinelT end, const key_compare& comp, const allocator_type& alloc)
    : tree_(ValueCompare_(comp), typename Tree_::allocator_type(alloc)) {
        insert(begin, end);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    map(std::from_range_t,
        RangeT&& range,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type())
    : tree_(ValueCompare_(comp), typename Tree_::allocator_type(alloc)) {
        insert_range(std::forward<RangeT>(range));
    }
    
    template <std::input_iterator IteratorT, std::sentinel_for<IteratorT> SentinelT>
    map(IteratorT begin, SentinelT end, const allocator_type& alloc)
    : map(begin, end, key_compare(), alloc) {}

    template <_ContainerCompatibleRange<value_type> RangeT>
    map(std::from_range_t, RangeT&& range, const allocator_type& alloc)
    : map(std::from_range, std::forward<RangeT>(range), key_compare(), alloc) {}

    map(const map& other) = default;

    map& operator=(const map& other) = default;

    map(map&& other) = default;

    map(map&& other, const allocator_type& alloc)
    : tree_(std::move(other.tree_), alloc) {}

    map& operator=(map&& other) = default;

    map(std::initializer_list<value_type> init_list, const key_compare& comp = key_compare())
    : tree_(ValueCompare_(comp)) {
        insert(init_list.begin(), init_list.end());
    }

    map(std::initializer_list<value_type> init_list, const key_compare& comp, const allocator_type& alloc)
    : tree_(ValueCompare_(comp), typename Tree_::allocator_type(alloc)) {
        insert(init_list.begin(), init_list.end());
    }

    map(std::initializer_list<value_type> init_list, const allocator_type& alloc)
    : map(init_list, key_compare(), alloc) {}

    map& operator=(std::initializer_list<value_type> init_list) {
        clear();
        insert(init_list.begin(), init_list.end());
        return *this;
    }

    explicit map(const allocator_type& alloc) : tree_(typename Tree_::allocator_type(alloc)) {}

    map(const map& other, const allocator_type& alloc) : tree_(other.tree_, alloc) {}

    ~map() = default;

    template <class Self>
    [[nodiscard]] SelfIterator<Self> begin(this Self& self) noexcept {
        return self.tree_.begin();
    }

    template <class Self>
    [[nodiscard]] SelfIterator<Self> end(this Self& self) noexcept {
        return self.tree_.end();
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
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    [[nodiscard]] const_reverse_iterator crend() const noexcept { return rend(); }

    [[nodiscard]] bool empty() const noexcept { return tree_.size() == 0; }
    [[nodiscard]] size_type size() const noexcept { return tree_.size(); }
    [[nodiscard]] size_type max_size() const noexcept { return tree_.max_size(); }

    mapped_type& operator[](const key_type& key) {
        return tree_
            .emplaceUnique(
                std::piecewise_construct,
                std::forward_as_tuple(key),
                std::forward_as_tuple()
            )
            .first
            ->value();
    }

    mapped_type& operator[](key_type&& key) {
        return tree_
            .emplaceUnique(
                std::piecewise_construct,
                std::forward_as_tuple(std::move(key)),
                std::forward_as_tuple()
            )
            .first
            ->value();
    }

    template <class Self, class TransparentKey>
    requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] mapped_type& at(this Self& self, TransparentKey&& arg) {
        auto [_, child] = self.tree_.find_equivalent(arg);
        if (child == nullptr) {
            throw std::out_of_range("map::at:  key not found");
        }
        return static_cast<NodePointer_>(child)->get_value().value();
    }

    [[nodiscard]] allocator_type get_allocator() const noexcept {
        return allocator_type(tree_.alloc());
    }
    [[nodiscard]] key_compare key_comp() const { return tree_.key_comp(); }

    template <class... ArgsT>
    std::pair<iterator, bool> emplace(ArgsT&&... args) {
        return tree_.emplaceUnique(std::forward<ArgsT>(args)...);
    }

    template <class... ArgsT>
    iterator emplace_hint(const_iterator pos, ArgsT&&... args) {
        return tree_
            .emplaceHintUnique(pos, std::forward<ArgsT>(args)...)
            .first;
    }

    template <class P>
    requires std::is_constructible_v<value_type, P>
    std::pair<iterator, bool> insert(P&& value) {
        return tree_.emplaceUnique(std::forward<P>(value));
    }

    template <class P>
    requires std::is_constructible_v<value_type, P>
    iterator insert(const_iterator pos, P&& value) {
        return tree_
            .emplaceHintUnique(pos.i_, std::forward<P>(value))
            .first;
    }

    std::pair<iterator, bool> insert(const value_type& value) {
        return tree_.emplaceUnique(value);
    }

    iterator insert(const_iterator pos, const value_type& value) {
        return tree_.emplaceHintUnique(pos.i_, value).first;
    }

    std::pair<iterator, bool> insert(value_type&& value) {
        return tree_.emplaceUnique(std::move(value));
    }

    iterator insert(const_iterator pos, value_type&& value) {
        return tree_.emplaceHintUnique(pos, std::move(value)).first;
    }

    void insert(std::initializer_list<value_type> init_list) {
        insert(init_list.begin(), init_list.end());
    }

    template <_ContainerCompatibleIterator<value_type> IteratorT, std::sentinel_for<IteratorT> SentinelT>
    void insert(IteratorT first, SentinelT last) {
        tree_.insertRangeUnique(first, last);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    void insert_range(RangeT&& range) {
        tree_.insertRangeUnique(
            std::ranges::begin(range),
            std::ranges::end(range)
        );
    }

    template <class... ArgsT>
    std::pair<iterator, bool> try_emplace(const key_type& k, ArgsT&&... args) {
        return tree_.emplaceUnique(
            std::piecewise_construct,
            std::forward_as_tuple(k),
            std::forward_as_tuple(std::forward<ArgsT>(args)...)
        );
    }

    template <class... ArgsT>
    std::pair<iterator, bool> try_emplace(key_type&& k, ArgsT&&... args) {
        return tree_.emplaceUnique(
            std::piecewise_construct,
            std::forward_as_tuple(std::move(k)),
            std::forward_as_tuple(std::forward<ArgsT>(args)...)
        );
    }

    template <class... ArgsT>
    iterator try_emplace(const_iterator hint, const key_type& k, ArgsT&&... args) {
        return tree_.emplaceHintUnique(
            hint,
            std::piecewise_construct,
            std::forward_as_tuple(k),
            std::forward_as_tuple(std::forward<ArgsT>(args)...)
        ).first;
    }

    template <class... ArgsT>
    iterator try_emplace(const_iterator hint, key_type&& k, ArgsT&&... args) {
        return tree_.emplaceHintUnique(
            hint,
            std::piecewise_construct,
            std::forward_as_tuple(std::move(k)),
            std::forward_as_tuple(std::forward<ArgsT>(args)...)
        ).first;
    }

    template <class _Vp>
    std::pair<iterator, bool> insert_or_assign(const key_type& k, _Vp&& v) {
        auto result = tree_.emplaceUnique(k, std::forward<_Vp>(v));
        auto& [iter, inserted] = result;
        if (!inserted) {
            iter->value() = std::forward<_Vp>(v);
        }
        return result;
    }

    template <class _Vp>
    std::pair<iterator, bool> insert_or_assign(key_type&& k, _Vp&& v) {
        auto result = tree_.emplaceUnique(std::move(k), std::forward<_Vp>(v));
        auto& [iter, inserted] = result;
        if (!inserted) {
            iter->value() = std::forward<_Vp>(v);
        }
        return result;
    }

    template <class _Vp>
    iterator insert_or_assign(const_iterator hint, const key_type& k, _Vp&& v) {
        auto [r, inserted] = tree_.emplaceHintUnique(hint, k, std::forward<_Vp>(v));
        if (!inserted) {
            r->value() = std::forward<_Vp>(v);
        }
        return r;
    }

    template <class _Vp>
    iterator insert_or_assign(const_iterator hint, key_type&& k, _Vp&& v) {
        auto [r, inserted] = tree_.emplaceHintUnique(hint, std::move(k), std::forward<_Vp>(v));
        if (!inserted) {
            r->value() = std::forward<_Vp>(v);
        }
        return r;
    }

    iterator erase(const_iterator p) { return tree_.erase(p.i_); }
    iterator erase(iterator p) { return tree_.erase(p.i_); }
    size_type erase(const key_type& k) { return tree_.eraseUnique(k); }
    iterator erase(const_iterator f, const_iterator l) { return tree_.erase(f.i_, l.i_); }
    
    void clear() noexcept { tree_.clear(); }

    insert_return_type insert(node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(nh.empty() || nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to map::insert()");
        return tree_.template nodeHandleInsertUnique< node_type, insert_return_type>(std::move(nh));
    }
    iterator insert(const_iterator __hint, node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(nh.empty() || nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to map::insert()");
        return tree_.template nodeHandleInsertUnique<node_type>(__hint.i_, std::move(nh));
    }
    [[nodiscard]] node_type extract(key_type const& __key) {
        return tree_.template nodeHandleExtract<node_type>(__key);
    }
    [[nodiscard]] node_type extract(const_iterator __it) {
        return tree_.template nodeHandleExtract<node_type>(__it.i_);
    }
    template <class CompareT2>
    void merge(map<key_type, mapped_type, CompareT2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeUnique(source.tree_);
    }
    template <class CompareT2>
    void merge(map<key_type, mapped_type, CompareT2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeUnique(source.tree_);
    }
    template <class CompareT2>
    void merge(multimap<key_type, mapped_type, CompareT2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeUnique(source.tree_);
    }
    template <class CompareT2>
    void merge(multimap<key_type, mapped_type, CompareT2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeUnique(source.tree_);
    }

    void swap(map& __m) noexcept(std::is_nothrow_swappable_v<Tree_>) { tree_.swap(__m.tree_); }

    template <class Self, typename TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] SelfIterator<Self> find(this Self& self, const TransparentKey& key) {
        return self.tree_.find(key);
    }

    template <typename TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] size_type count(const TransparentKey& key) const {
        if constexpr (std::is_same_v<std::remove_cvref_t<TransparentKey>, key_type>) {
            return tree_.countUnique(key);
        } else {
            return tree_.countMulti(key);
        }
    }

    template <typename TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] bool contains(const TransparentKey& k) const {
        return find(k) != end();
    }

    template <class Self, typename TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] SelfIterator<Self> lower_bound(this Self& self, const TransparentKey& k) {
        if constexpr (std::is_same_v<std::remove_cvref_t<TransparentKey>, key_type>) {
            return self.tree_.lowerBoundUnique(k);
        } else {
            return self.tree_.lowerBoundMulti(k);
        }
    }

    template <class Self, typename TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] SelfIterator<Self> upper_bound(this Self& self, const TransparentKey& k) {
        if constexpr (std::is_same_v<std::remove_cvref_t<TransparentKey>, key_type>) {
            return self.tree_.upperBoundUnique(k);
        } else {
            return self.tree_.upperBoundMulti(k);
        }
    }

    template <class Self, typename TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] SelfSubrange<Self> equal_range(this Self& self, const TransparentKey& k) {
        if constexpr (std::is_same_v<std::remove_cvref_t<TransparentKey>, key_type>) {
            return self.tree_.equalRangeUnique(k);
        } else {
            return self.tree_.equalRangeMulti(k);
        }
    }

private:
    using NodePointer_ = Tree_::node_pointer;
};

template <
    std::input_iterator IteratorT
  , OrdersAtLeastWeakly<__iter_to_alloc_type<IteratorT>> CompareT = CompareThreeWay
  , Allocator AllocatorT                                          = std::allocator<__iter_to_alloc_type<IteratorT>>
>
map(
    IteratorT,
    IteratorT,
    CompareT = CompareT(),
    AllocatorT = AllocatorT()
) -> map<
        __iter_key_type<IteratorT>
      , __iter_mapped_type<IteratorT>
      , CompareT, AllocatorT
>;

template <
    std::ranges::input_range RangeT
  , OrdersAtLeastWeakly<__range_to_alloc_type<RangeT>> CompareT = CompareThreeWay
  , Allocator AllocatorT                                        = std::allocator<__range_to_alloc_type<RangeT>>
>
map(
    std::from_range_t,
    RangeT&&,
    CompareT = CompareT(),
    AllocatorT = AllocatorT()
) -> map<
         __range_key_type<RangeT>
       , __range_mapped_type<RangeT>
       , CompareT
       , AllocatorT
     >;

template <
    class KeyT
  , class ValueT
  , OrdersAtLeastWeakly<KeyT> CompareT = CompareThreeWay
  , Allocator AllocatorT               = std::allocator<MapValue<KeyT, ValueT>>
>
map(
    std::initializer_list<MapValue<KeyT, ValueT>>,
    CompareT = CompareT(),
    AllocatorT = AllocatorT()
) -> map<
         std::remove_const_t<KeyT>
       , ValueT
       , CompareT
       , AllocatorT
     >;

template <std::input_iterator IteratorT, Allocator AllocatorT>
map(IteratorT, IteratorT, AllocatorT)
-> map<
    __iter_key_type<IteratorT>
  , __iter_mapped_type<IteratorT>
  , CompareThreeWay
  , AllocatorT
>;

template<std::ranges::input_range RangeT, Allocator AllocatorT>
map(std::from_range_t, RangeT&&, AllocatorT)
-> map<
       __range_key_type<RangeT>
     , __range_mapped_type<RangeT>
     , CompareThreeWay
     , AllocatorT
   >;

template<class KeyT, class ValueT, Allocator AllocatorT>
map(std::initializer_list<std::pair<KeyT, ValueT>>, AllocatorT)
-> map<
       std::remove_const_t<KeyT>
     , ValueT
     , CompareThreeWay
     , AllocatorT
   >;

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
inline bool
operator==(const map<KeyT, ValueT, CompareT, AllocatorT>& x, const map<KeyT, ValueT, CompareT, AllocatorT>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
auto
operator<=>(const map<KeyT, ValueT, CompareT, AllocatorT>& x, const map<KeyT, ValueT, CompareT, AllocatorT>& y) {
    using Map = map<KeyT, ValueT, CompareT, AllocatorT>;
    
    return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(), y.end(), [](const Map::value_type& val1, const Map::value_type& val2) {
        const auto key_cmp = CompareT{}(val1.key(), val2.key());
        using CmpRes = decltype(key_cmp);
        if (key_cmp == 0) {
            if (val1.value() < val2.value()) {
                return CmpRes::less;
            } else if (val2.value() < val1.value()) {
                return CmpRes::greater;
            } else {
                return CmpRes::equivalent;
            }
        }
        return key_cmp;
    });
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
inline void
swap(map<KeyT, ValueT, CompareT, AllocatorT>& x, map<KeyT, ValueT, CompareT, AllocatorT>& y)
noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT, class PredicateT>
inline typename map<KeyT, ValueT, CompareT, AllocatorT>::size_type
erase_if(map<KeyT, ValueT, CompareT, AllocatorT>& container, PredicateT pred) {
    return mstd::erase_if_container(container, pred);
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
class multimap {
public:
    using key_type        = KeyT;
    using mapped_type     = ValueT;
    using value_type      = MapValue<KeyT, ValueT>;
    using key_compare     = std::type_identity_t<CompareT>;
    using allocator_type  = std::type_identity_t<AllocatorT>;
    using reference       = value_type& ;
    using const_reference = const value_type&;

private:
    using ValueType_    = MapValue<key_type, mapped_type>;
    using Tree_         = Tree<ValueType_, typename ValueType_::KeyProj, key_compare, allocator_type>;
    using AllocTraits_  = std::allocator_traits<allocator_type>;

    Tree_ tree_;

public:
    using pointer                = AllocTraits_::pointer;
    using const_pointer          = AllocTraits_::const_pointer;
    using size_type              = AllocTraits_::size_type;
    using difference_type        = AllocTraits_::difference_type;
    using iterator               = Tree_::iterator;
    using const_iterator         = Tree_::const_iterator;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    template <class Self>
    using SelfIterator = std::conditional_t<std::is_const_v<Self>, const_iterator, iterator>;
    template <class Self>
    using SelfSubrange = std::ranges::subrange<SelfIterator<Self>>;

    using node_type = NodeHandle<typename Tree_::node, allocator_type>;

    template <class /*Key*/, class /*Value*/, class /*Comp*/, class /*Alloc*/>
    friend class map;
    template <class /*Key*/, class /*Value*/, class /*Comp*/, class /*Alloc*/>
    friend class multimap;

    multimap() noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value
     && std::is_nothrow_default_constructible<key_compare>::value
     && std::is_nothrow_copy_constructible<key_compare>::value
    )
    : tree_(key_compare()) {}

    explicit multimap(const key_compare& comp) noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value
     && std::is_nothrow_copy_constructible<key_compare>::value
    )
    : tree_(comp) {}

    explicit multimap(const key_compare& comp, const allocator_type& alloc)
    : tree_(comp, typename Tree_::allocator_type(alloc)) {}

    template <_ContainerCompatibleIterator<value_type> IteratorT, std::sentinel_for<IteratorT> SentinelT>
    multimap(IteratorT begin, SentinelT end, const key_compare& comp = key_compare())
    : tree_(comp) {
        insert(begin, end);
    }

    template <_ContainerCompatibleIterator<value_type> IteratorT, std::sentinel_for<IteratorT> SentinelT>
    multimap(IteratorT begin, SentinelT end, const key_compare& comp, const allocator_type& alloc)
    : tree_(comp, typename Tree_::allocator_type(alloc)) {
        insert(begin, end);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    multimap(std::from_range_t,
             RangeT&& range,
             const key_compare& comp = key_compare(),
             const allocator_type& alloc = allocator_type())
    : tree_(comp, typename Tree_::allocator_type(alloc)) {
        insert_range(std::forward<RangeT>(range));
    }

    template <_ContainerCompatibleIterator<value_type> IteratorT, std::sentinel_for<IteratorT> SentinelT>
    multimap(IteratorT begin, SentinelT end, const allocator_type& alloc)
    : multimap(begin, end, key_compare(), alloc) {}
    
    template <_ContainerCompatibleRange<value_type> RangeT>
    multimap(std::from_range_t, RangeT&& range, const allocator_type& alloc)
    : multimap(std::from_range, std::forward<RangeT>(range), key_compare(), alloc) {}
    
    multimap(const multimap& other) = default;

    multimap& operator=(const multimap& other) = default;

    multimap(multimap&& other) = default;

    multimap(multimap&& other, const allocator_type& alloc)
    : tree_(std::move(other.tree_), alloc) {}

    multimap& operator=(multimap&& other) = default;

    multimap(std::initializer_list<value_type> init_list, const key_compare& comp = key_compare())
    : tree_(comp) {
        insert(init_list.begin(), init_list.end());
    }

    multimap(std::initializer_list<value_type> init_list, const key_compare& comp, const allocator_type& alloc)
    : tree_(comp, typename Tree_::allocator_type(alloc)) {
        insert(init_list.begin(), init_list.end());
    }

    multimap(std::initializer_list<value_type> init_list, const allocator_type& alloc)
    : multimap(init_list, key_compare(), alloc) {}
    
    multimap& operator=(std::initializer_list<value_type> init_list) {
        clear();
        insert(init_list.begin(), init_list.end());
        return *this;
    }

    explicit multimap(const allocator_type& allocator)
    : tree_(typename Tree_::allocator_type(allocator)) {}

    multimap(const multimap& other, const allocator_type& alloc)
    : tree_(other.tree_, alloc) {}

    ~multimap() = default;

    template <class Self>
    [[nodiscard]] SelfIterator<Self> begin(this Self& self) noexcept { return self.tree_.begin(); }
    template <class Self>
    [[nodiscard]] SelfIterator<Self> end(this Self& self) noexcept { return self.tree_.end(); }

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
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    [[nodiscard]] const_reverse_iterator crend() const noexcept { return rend(); }

    [[nodiscard]] bool empty() const noexcept { return tree_.size() == 0; }
    [[nodiscard]] size_type size() const noexcept { return tree_.size(); }
    [[nodiscard]] size_type max_size() const noexcept { return tree_.max_size(); }

    [[nodiscard]] allocator_type get_allocator() const noexcept {
        return allocator_type(tree_.alloc());
    }
    [[nodiscard]] key_compare key_comp() const { return tree_.key_comp(); }

    template <class... ArgsT>
    iterator emplace(ArgsT&&... args) {
        return tree_.emplaceMulti(std::forward<ArgsT>(args)...);
    }

    template <class... ArgsT>
    iterator emplace_hint(const_iterator pos, ArgsT&&... args) {
        return tree_.emplaceHintMulti(pos.i_, std::forward<ArgsT>(args)...);
    }

    template <class _Pp>
    requires std::is_constructible_v<value_type, _Pp>
    iterator insert(_Pp&& pos) {
        return tree_.emplaceMulti(std::forward<_Pp>(pos));
    }

    template <class _Pp>
    requires std::is_constructible_v<value_type, _Pp>
    iterator insert(const_iterator pos, _Pp&& val) {
        return tree_.emplaceHintMulti(pos.i_, std::forward<_Pp>(val));
    }

    iterator insert(value_type&& value) {
        return tree_.emplaceMulti(std::move(value));
    }

    iterator insert(const_iterator pos, value_type&& __v) {
        return tree_.emplaceHintMulti(pos.i_, std::move(__v));
    }

    void insert(std::initializer_list<value_type> init_list) {
        insert(init_list.begin(), init_list.end());
    }

    iterator insert(const value_type& __v) {
        return tree_.emplaceMulti(__v);
    }

    iterator insert(const_iterator pos, const value_type& __v) {
        return tree_.emplaceHintMulti(pos.i_, __v);
    }

    template <_ContainerCompatibleIterator<value_type> IteratorT, std::sentinel_for<IteratorT> SentinelT>
    void insert(IteratorT begin, SentinelT end) {
        tree_.insertRangeMulti(begin, end);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    void insert_range(RangeT&& range) {
        tree_.insertRangeMulti(std::ranges::begin(range),  std::ranges::end(range));
    }

    iterator erase(const_iterator pos) {
        return tree_.erase(pos.i_);
    }

    iterator erase(iterator pos) {
        return tree_.erase(pos.i_);
    }

    size_type erase(const key_type& k) {
        return tree_.eraseMulti(k);
    }

    iterator erase(const_iterator begin, const_iterator end) {
        return tree_.erase(begin.i_, end.i_);
    }

    iterator insert(node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            nh.empty() || nh.get_allocator() == get_allocator(),
            "node_type with incompatible allocator passed to multimap::insert()"
        );
        return tree_.template nodeHandleInsertMulti<node_type>(std::move(nh));
    }
    iterator insert(const_iterator __hint, node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            nh.empty() || nh.get_allocator() == get_allocator(),
            "node_type with incompatible allocator passed to multimap::insert()"
        );
        return tree_.template nodeHandleInsertMulti<node_type>(__hint.i_, std::move(nh));
    }
    [[nodiscard]] node_type extract(key_type const& __key) {
        return tree_.template nodeHandleExtract<node_type>(__key);
    }
    [[nodiscard]] node_type extract(const_iterator __it) {
        return tree_.template nodeHandleExtract<node_type>(__it.i_);
    }
    template <class CompareT2>
    void merge(multimap<key_type, mapped_type, CompareT2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        return tree_.nodeHandleMergeMulti(source.tree_);
    }
    template <class CompareT2>
    void merge(multimap<key_type, mapped_type, CompareT2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        return tree_.nodeHandleMergeMulti(source.tree_);
    }
    template <class CompareT2>
    void merge(map<key_type, mapped_type, CompareT2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        return tree_.nodeHandleMergeMulti(source.tree_);
    }
    template <class CompareT2>
    void merge(map<key_type, mapped_type, CompareT2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        return tree_.nodeHandleMergeMulti(source.tree_);
    }

    void clear() noexcept { tree_.clear(); }

    void swap(multimap& other) noexcept(std::is_nothrow_swappable_v<Tree_>) {
        tree_.swap(other.tree_);
    }

    template <class Self, class TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] SelfIterator<Self> find(this Self& self, const TransparentKey& key) {
        return self.tree_.find(key);
    }

    template <class TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] size_type count(const TransparentKey& key) const {
        return tree_.countMulti(key);
    }

    template <class TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] bool contains(const TransparentKey& key) const {
        return find(key) != end();
    }

    template <class Self, class TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] SelfIterator<Self> lower_bound(this Self& self, const TransparentKey& key) {
        return self.tree_.lowerBoundMulti(key);
    }

    template <class Self, class TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] iterator upper_bound(this Self& self, const TransparentKey& key) {
        return self.tree_.upperBoundMulti(key);
    }

    template <class Self, class TransparentKey> requires OrdersWithAtLeastWeakly<CompareT, key_type, TransparentKey>
    [[nodiscard]] SelfSubrange<Self> equal_range(this Self& self, const TransparentKey& key) {
        return self.tree_.equalRangeMulti(key);
    }
};

template <
    std::input_iterator IteratorT
  , OrdersAtLeastWeakly<__iter_to_alloc_type<IteratorT>> CompareT = CompareThreeWay
  , Allocator AllocatorT = std::allocator<__iter_to_alloc_type<IteratorT>>
>
multimap(IteratorT, IteratorT, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multimap<
       __iter_key_type<IteratorT>
     , __iter_mapped_type<IteratorT>
     , CompareT
     , AllocatorT
   >;

template <
    std::ranges::input_range RangeT
  , OrdersAtLeastWeakly<__range_to_alloc_type<RangeT>> CompareT = CompareThreeWay
  , Allocator AllocatorT = std::allocator<__range_to_alloc_type<RangeT>>
>
multimap(std::from_range_t, RangeT&&, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multimap<
       __range_key_type<RangeT>
     , __range_mapped_type<RangeT>
     , CompareT
     , AllocatorT
   >;

template <
    class KeyT
  , class ValueT
  , OrdersAtLeastWeakly<KeyT> CompareT = CompareThreeWay
  , Allocator AllocatorT = std::allocator<MapValue<KeyT, ValueT>>
>
multimap(std::initializer_list<std::pair<KeyT, ValueT>>, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multimap<std::remove_const_t<KeyT>, ValueT, CompareT, AllocatorT>;

template <std::input_iterator IteratorT, Allocator AllocatorT>
multimap(IteratorT, IteratorT, AllocatorT)
-> multimap<
       __iter_key_type<IteratorT>
     , __iter_mapped_type<IteratorT>
     , CompareThreeWay
     , AllocatorT
   >;

template <std::ranges::input_range RangeT, Allocator AllocatorT>
multimap(std::from_range_t, RangeT&&, AllocatorT)
-> multimap<
       __range_key_type<RangeT>
     , __range_mapped_type<RangeT>
     , CompareThreeWay
     , AllocatorT
   >;

template <class KeyT, class ValueT, Allocator AllocatorT>
multimap(std::initializer_list<std::pair<KeyT, ValueT>>, AllocatorT)
-> multimap<
       std::remove_const_t<KeyT>
     , ValueT
     , CompareThreeWay
     , AllocatorT
   >;

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
inline bool
operator==(const multimap<KeyT, ValueT, CompareT, AllocatorT>& x, const multimap<KeyT, ValueT, CompareT, AllocatorT>& y) {
    return std::ranges::equal(x, y);
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
auto
operator<=>(const multimap<KeyT, ValueT, CompareT, AllocatorT>& x,
            const multimap<KeyT, ValueT, CompareT, AllocatorT>& y) {
    using Map = multimap<KeyT, ValueT, CompareT, AllocatorT>;

    return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(), y.end(), [](const Map::value_type& val1, const Map::value_type& val2) {
        const auto key_cmp = CompareT{}(val1.key(), val2.key());
        using CmpRes = decltype(key_cmp);
        if (key_cmp == 0) {
            if (val1.value() < val2.value()) {
                return CmpRes::less;
            } else if (val2.value() < val1.value()) {
                return CmpRes::greater;
            } else {
                return CmpRes::equivalent;
            }
        }
        return key_cmp;
    });
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
inline void
swap(multimap<KeyT, ValueT, CompareT, AllocatorT>& x, multimap<KeyT, ValueT, CompareT, AllocatorT>& y)
noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT, class PredicateT>
inline typename multimap<KeyT, ValueT, CompareT, AllocatorT>::size_type
erase_if(multimap<KeyT, ValueT, CompareT, AllocatorT>& container, PredicateT pred) {
    return mstd::erase_if_container(container, pred);
}

} // namespace mstd

namespace mstd {
namespace pmr {
    template <class KeyT, class ValueT, class _CompareT = CompareThreeWay>
    using map = mstd::map<KeyT, ValueT, _CompareT, std::pmr::polymorphic_allocator<MapValue<KeyT, ValueT>>>;
    template <class KeyT, class ValueT, class _CompareT = CompareThreeWay>
    using multimap = mstd::multimap<KeyT, ValueT, _CompareT, std::pmr::polymorphic_allocator<MapValue<KeyT, ValueT>>>;
} // namespace pmr
} // namespace mstd


#endif // MSTD_MAP
