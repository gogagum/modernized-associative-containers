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
#include <detail/config.hpp>
#include <functional>
#include <detail/functional/is_transparent.hpp>
#include <detail/iterator/erase_if_container.hpp>
#include <iterator>
#include <memory>
#include <detail/memory/compressed_pair.hpp>
#include <detail/iterator/iterator_traits.hpp>
#include <detail/iterator/range_iterator_traits.hpp>
#include <detail/iterator/erase_if_container.hpp>
#include <detail/compare/synth_three_way.hpp>
#include <memory_resource>
#include <detail/node_handle.hpp>
#include <ranges>
#include <detail/tree.hpp>
#include <detail/type_traits/container_traits.hpp>
#include <detail/type_traits/is_allocator.hpp>
#include <utility>
#include <detail/utility/lazy_synth_three_way_comparator.hpp>
#include <detail/ranges/container_compatible_range.hpp>
#include <stdexcept>
#include <tuple>
#include <version>

// standard-mandated includes

// [associative.map.syn]
#  include <compare>
#  include <initializer_list>

namespace mstd {

template <class KeyT, class _CP, class CompareT>
class MapValueCompare {
    MSTD_COMPRESSED_ELEMENT(CompareT, comp_);

public:
    MapValueCompare() noexcept(std::is_nothrow_default_constructible<CompareT>::value)
    : comp_() {}
    MapValueCompare(CompareT c) noexcept(std::is_nothrow_copy_constructible<CompareT>::value)
    : comp_(c) {}
    const CompareT& key_comp() const noexcept { return comp_; }

    bool operator()(const _CP& x, const _CP& y) const { return comp_(x.first, y.first); }
    bool operator()(const _CP& x, const KeyT& y) const { return comp_(x.first, y); }
    bool operator()(const KeyT& x, const _CP& y) const { return comp_(x, y.first); }
    void swap(MapValueCompare& y) noexcept(std::is_nothrow_swappable_v<CompareT>) { std::swap(comp_, y.comp_); }

    template <typename _K2>
    bool operator()(const _K2& x, const _CP& y) const { return comp_(x, y.first); }

    template <typename _K2>
    bool operator()(const _CP& x, const _K2& y) const { return comp_(x.first, y); }
};

template <class MapValueT, class KeyT, class CompareT>
struct LazySynthThreeWayComparator<MapValueCompare<KeyT, MapValueT, CompareT>, MapValueT, MapValueT> {
    LazySynthThreeWayComparator<CompareT, KeyT, KeyT> comp_;

    LazySynthThreeWayComparator(const MapValueCompare<KeyT, MapValueT, CompareT>& comp)
    : comp_(comp.key_comp()) {}

    auto operator()(const MapValueT& lhs, const MapValueT& rhs) const {
        return comp_(lhs.first, rhs.first);
    }
};

template <class MapValueT, class KeyT, class TransparentKeyT, class CompareT>
struct LazySynthThreeWayComparator<MapValueCompare<KeyT, MapValueT, CompareT>, TransparentKeyT, MapValueT> {
    LazySynthThreeWayComparator<CompareT, TransparentKeyT, KeyT> comp_;

    LazySynthThreeWayComparator(const MapValueCompare<KeyT, MapValueT, CompareT>& comp)
    : comp_(comp.key_comp()) {}

    auto operator()(const TransparentKeyT& lhs, const MapValueT& rhs) const {
        return comp_(lhs, rhs.first);
    }
};

template <class MapValueT, class KeyT, class TransparentKeyT, class CompareT>
struct LazySynthThreeWayComparator<MapValueCompare<KeyT, MapValueT, CompareT>, MapValueT, TransparentKeyT> {
    LazySynthThreeWayComparator<CompareT, KeyT, TransparentKeyT> comp_;

    LazySynthThreeWayComparator(const MapValueCompare<KeyT, MapValueT, CompareT>& comp)
    : comp_(comp.key_comp()) {}

    auto operator()(const MapValueT& lhs, const TransparentKeyT& rhs) const {
        return comp_(lhs.first, rhs);
    }
};

template <class KeyT, class _CP, class CompareT>
inline void
swap(MapValueCompare<KeyT, _CP, CompareT>& x, MapValueCompare<KeyT, _CP, CompareT>& y)
noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

template <class AllocatorT>
class MapNodeDestructor {
    using AllocatorType_ = AllocatorT;
    using AllocTraits_ = allocator_traits<AllocatorType_>;

public:
    typedef typename AllocTraits_::pointer pointer;

private:
    AllocatorType_& na_;

public:
    bool first_constructed;
    bool second_constructed;

    explicit MapNodeDestructor(AllocatorType_& na) noexcept
    : na_{na}
    , first_constructed{false}
    , second_constructed{false} {}

    MapNodeDestructor(__tree_node_destructor<AllocatorType_>&& x) noexcept
    : na_(x.na_)
    , first_constructed(x.__value_constructed)
    , second_constructed(x.__value_constructed) {
        x.__value_constructed = false;
    }

    MapNodeDestructor& operator=(const MapNodeDestructor&) = delete;

    void operator()(pointer ptr) noexcept {
        if (second_constructed) {
            AllocTraits_::destroy(na_, std::addressof(ptr->__get_value().second));
        }
        if (first_constructed) {
            AllocTraits_::destroy(na_, std::addressof(ptr->__get_value().first));
        }
        if (ptr) {
            AllocTraits_::deallocate(na_, ptr, 1);
        }
    }
};

template <class KeyT, class _Tp>
struct ValueType;

template <class TreeIteratorT>
class MapIterator {
    TreeIteratorT i_;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = typename TreeIteratorT::value_type;
    using difference_type   = typename TreeIteratorT::difference_type;
    using reference         = value_type&;
    using pointer           = typename TreeIteratorT::pointer;

    MapIterator() noexcept {}

    MapIterator(TreeIteratorT __i) noexcept : i_(__i) {}

    reference operator*() const { return *i_; }
    pointer operator->() const { return std::pointer_traits<pointer>::pointer_to(*i_); }

    MapIterator& operator++() {
        ++i_;
        return *this;
    }
    MapIterator operator++(int) {
        MapIterator t(*this);
        ++(*this);
        return t;
    }

    MapIterator& operator--() {
        --i_;
        return *this;
    }
    MapIterator operator--(int) {
        MapIterator __t(*this);
        --(*this);
        return __t;
    }

    friend bool operator==(const MapIterator& x, const MapIterator& y) {
        return x.i_ == y.i_;
    }
    friend bool operator!=(const MapIterator& x, const MapIterator& y) {
        return x.i_ != y.i_;
    }

    template <class, class, class, class>
    friend class map;
    template <class, class, class, class>
    friend class multimap;
    template <class>
    friend class MapConstIterator;

    template <class, class...>
    friend struct __specialized_algorithm;
};

template <class _Alg, class TreeIteratorT>
struct __specialized_algorithm<_Alg, __iterator_pair<MapIterator<TreeIteratorT>, MapIterator<TreeIteratorT>>> {
    using Tree_ = __specialized_algorithm<_Alg, __iterator_pair<TreeIteratorT, TreeIteratorT>>;

    static const bool __has_algorithm = Tree_::__has_algorithm;

    using Iterator_ = MapIterator<TreeIteratorT>;

    template <class... ArgsT>
    static void operator()(Iterator_ first, Iterator_ last, ArgsT&&... args) {
        Tree_()(first.i_, last.i_, std::forward<ArgsT>(args)...);
    }
};

template <class TreeIteratorT>
class MapConstIterator {
    TreeIteratorT i_;

public:
    using iterator_category = bidirectional_iterator_tag;
    using value_type        = typename TreeIteratorT::value_type;
    using difference_type   = typename TreeIteratorT::difference_type;
    using reference         = const value_type&;
    using pointer           = typename TreeIteratorT::pointer;

    MapConstIterator() noexcept {}

    MapConstIterator(TreeIteratorT i) noexcept : i_(i) {}
    MapConstIterator(MapIterator< typename TreeIteratorT::__non_const_iterator> i) noexcept : i_(i.i_) {}

    reference operator*() const { return *i_; }
    pointer operator->() const { return std::pointer_traits<pointer>::pointer_to(*i_); }

    MapConstIterator& operator++() {
        ++i_;
        return *this;
    }
    MapConstIterator operator++(int) {
        MapConstIterator t(*this);
        ++(*this);
        return t;
    }

    MapConstIterator& operator--() {
        --i_;
        return *this;
    }
    MapConstIterator operator--(int) {
        MapConstIterator t(*this);
        --(*this);
        return t;
    }

    friend bool operator==(const MapConstIterator& x, const MapConstIterator& y) {
        return x.i_ == y.i_;
    }
    friend bool operator!=(const MapConstIterator& x, const MapConstIterator& y) {
        return x.i_ != y.i_;
    }

    template <class, class, class, class>
    friend class map;
    template <class, class, class, class>
    friend class multimap;
    template <class, class, class>
    friend class __tree_const_iterator;

    template <class, class...>
    friend struct __specialized_algorithm;
};

template <class _Alg, class TreeIteratorT>
struct __specialized_algorithm<_Alg, __iterator_pair<MapConstIterator<TreeIteratorT>, MapConstIterator<TreeIteratorT>>> {
    using Tree_ = __specialized_algorithm<_Alg, __iterator_pair<TreeIteratorT, TreeIteratorT>>;

    static const bool __has_algorithm = Tree_::__has_algorithm;

    using Iterator_ = MapConstIterator<TreeIteratorT>;

    template <class... ArgsT>
    static void operator()(Iterator_ first, Iterator_ last, ArgsT&&... args) {
        Tree_()(first.i_, last.i_, std::forward<ArgsT>(args)...);
    }
};

template <class KeyT, class _Tp, class CompareT = std::less<KeyT>, class AllocatorT = std::allocator<std::pair<const KeyT, _Tp> > >
class multimap;

template <class KeyT, class _Tp, class CompareT = std::less<KeyT>, class AllocatorT = std::allocator<std::pair<const KeyT, _Tp> > >
class map {
public:
    // types:
    typedef KeyT key_type;
    typedef _Tp mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef std::type_identity_t<CompareT> key_compare;
    typedef std::type_identity_t<AllocatorT> allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

    class value_compare : public std::binary_function<value_type, value_type, bool> {
        friend class map;

    protected:
        key_compare comp;

        value_compare(key_compare c) : comp(c) {}

    public:
        bool operator()(const value_type& x, const value_type& y) const {
            return comp(x.first, y.first);
        }
    };

private:
    using ValueType_    = ValueType<key_type, mapped_type>;
    using ValueCompare_ = MapValueCompare<key_type, value_type, key_compare>;
    using Tree_         = Tree<ValueType_, ValueCompare_, allocator_type>;
    using AllocTraits_ = allocator_traits<allocator_type>;

    static_assert(__check_valid_allocator<allocator_type>::value, "");

    Tree_ tree_;

public:
    typedef typename AllocTraits_::pointer pointer;
    typedef typename AllocTraits_::const_pointer const_pointer;
    typedef typename AllocTraits_::size_type size_type;
    typedef typename AllocTraits_::difference_type difference_type;
    using iterator               = MapIterator<typename Tree_::iterator>;
    using const_iterator         = MapConstIterator<typename Tree_::const_iterator>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using node_type = MapNodeHandle<typename Tree_::__node, allocator_type>;
    typedef __insert_return_type<iterator, node_type> insert_return_type;

    template <class _Key2, class _Value2, class _Comp2, class _Alloc2>
    friend class map;
    template <class _Key2, class _Value2, class _Comp2, class _Alloc2>
    friend class multimap;

    map() noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value && std::is_nothrow_default_constructible<key_compare>::value&&
        std::is_nothrow_copy_constructible<key_compare>::value)
    : tree_(ValueCompare_(key_compare())) {}

    explicit map(const key_compare& comp) noexcept(
           std::is_nothrow_default_constructible<allocator_type>::value 
        && std::is_nothrow_copy_constructible<key_compare>::value)
    : tree_(ValueCompare_(comp)) {}

    explicit map(const key_compare& comp, const allocator_type& alloc)
    : tree_(ValueCompare_(comp), typename Tree_::allocator_type(alloc)) {}

    template <class InputIteratorT>
    map(InputIteratorT begin, InputIteratorT end, const key_compare& comp = key_compare())
    : tree_(ValueCompare_(comp)) {
        insert(begin, end);
    }

    template <class InputIteratorT>
    map(InputIteratorT begin, InputIteratorT end, const key_compare& comp, const allocator_type& alloc)
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
    
    template <class InputIteratorT>
    map(InputIteratorT begin, InputIteratorT end, const allocator_type& alloc)
    : map(begin, end, key_compare(), alloc) {}

    template <_ContainerCompatibleRange<value_type> RangeT>
    map(std::from_range_t, RangeT&& range, const allocator_type& alloc)
    : map(std::from_range, std::forward<RangeT>(range), key_compare(), alloc) {}

    map(const map& __m) = default;

    map& operator=(const map& __m) = default;

    map(map&& m) = default;

    map(map&& m, const allocator_type& alloc) : tree_(std::move(m.tree_), alloc) {}

    map& operator=(map&& __m) = default;

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

    map(const map& __m, const allocator_type& alloc) : tree_(__m.tree_, alloc) {}

    ~map() { static_assert(sizeof(mstd::__diagnose_non_const_comparator<KeyT, CompareT>()), ""); }

    [[nodiscard]] iterator begin() noexcept { return tree_.begin(); }
    [[nodiscard]] const_iterator begin() const noexcept { return tree_.begin(); }
    [[nodiscard]] iterator end() noexcept { return tree_.end(); }
    [[nodiscard]] const_iterator end() const noexcept { return tree_.end(); }

    [[nodiscard]] reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    [[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
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

    mapped_type& operator[](const key_type& key);
    mapped_type& operator[](key_type&& key);

    template <
        class _Arg
      , std::enable_if_t<
            __is_transparently_comparable_v<
                CompareT
              , key_type
              , std::remove_cvref_t<_Arg>
            >
          , int
        > = 0
    >
    [[nodiscard]] mapped_type& at(_Arg&& arg) {
        auto [_, child] = tree_.__find_equal(arg);
        if (child == nullptr) {
            std::__throw_out_of_range("map::at:  key not found");
        }
        return static_cast<__node_pointer>(child)->__get_value().second;
    }

    template <
        class _Arg
      , std::enable_if_t<
            __is_transparently_comparable_v<
                CompareT
              , key_type
              , std::remove_cvref_t<_Arg>
            >
          , int
        > = 0
    >
    [[nodiscard]] const mapped_type& at(_Arg&& arg) const {
        auto [_, child] = tree_.__find_equal(arg);
        if (child == nullptr) {
            std::__throw_out_of_range("map::at:  key not found");
        }
        return static_cast<__node_pointer>(child)->__get_value().second;
    }

    [[nodiscard]] mapped_type& at(const key_type& key);
    [[nodiscard]] const mapped_type& at(const key_type& key) const;

    [[nodiscard]] allocator_type get_allocator() const noexcept {
        return allocator_type(tree_.__alloc());
    }
    [[nodiscard]] key_compare key_comp() const { return tree_.value_comp().key_comp(); }
    [[nodiscard]] value_compare value_comp() const {
        return value_compare(tree_.value_comp().key_comp());
    }

    template <class... ArgsT>
    std::pair<iterator, bool> emplace(ArgsT&&... args) {
        return tree_.__emplace_unique(std::forward<ArgsT>(args)...);
    }

    template <class... ArgsT>
    iterator emplace_hint(const_iterator pos, ArgsT&&... args) {
        return tree_
            .__emplace_hint_unique(pos.i_, std::forward<ArgsT>(args)...)
            .first;
    }

    template <
        class _Pp
      , std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0
    >
    std::pair<iterator, bool> insert(_Pp&& pos) {
        return tree_.__emplace_unique(std::forward<_Pp>(pos));
    }

    template <
        class _Pp
      , std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0
    >
    iterator insert(const_iterator __pos, _Pp&& pos) {
        return tree_
            .__emplace_hint_unique(__pos.i_, std::forward<_Pp>(pos))
            .first;
    }

    std::pair<iterator, bool> insert(const value_type& __v) {
        return tree_.__emplace_unique(__v);
    }

    iterator insert(const_iterator pos, const value_type& __v) {
        return tree_.__emplace_hint_unique(pos.i_, __v).first;
    }

    std::pair<iterator, bool> insert(value_type&& __v) {
        return tree_.__emplace_unique(std::move(__v));
    }

    iterator insert(const_iterator pos, value_type&& __v) {
        return tree_.__emplace_hint_unique(pos.i_, std::move(__v)).first;
    }

    void insert(std::initializer_list<value_type> init_list) {
        insert(init_list.begin(), init_list.end());
    }

    template <class InputIteratorT>
    void insert(InputIteratorT first, InputIteratorT last) {
        tree_.__insert_range_unique(first, last);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    void insert_range(RangeT&& range) {
        tree_.__insert_range_unique(
            std::ranges::begin(range),
            std::ranges::end(range)
        );
    }

    template <class... ArgsT>
    std::pair<iterator, bool> try_emplace(const key_type& k, ArgsT&&... args) {
        return tree_.__emplace_unique(
            std::piecewise_construct,
            std::forward_as_tuple(k),
            std::forward_as_tuple(std::forward<ArgsT>(args)...)
        );
    }

    template <class... ArgsT>
    std::pair<iterator, bool> try_emplace(key_type&& k, ArgsT&&... args) {
        return tree_.__emplace_unique(
            std::piecewise_construct,
            std::forward_as_tuple(std::move(k)),
            std::forward_as_tuple(std::forward<ArgsT>(args)...)
        );
    }

    template <class... ArgsT>
    iterator try_emplace(const_iterator hint, const key_type& k, ArgsT&&... args) {
        return tree_.__emplace_hint_unique(
            hint.i_,
            std::piecewise_construct,
            std::forward_as_tuple(k),
            std::forward_as_tuple(std::forward<ArgsT>(args)...)
        ).first;
    }

    template <class... ArgsT>
    iterator try_emplace(const_iterator hint, key_type&& k, ArgsT&&... args) {
        return tree_.__emplace_hint_unique(
            hint.i_,
            std::piecewise_construct,
            std::forward_as_tuple(std::move(k)),
            std::forward_as_tuple(std::forward<ArgsT>(args)...)
        ).first;
    }

    template <class _Vp>
    std::pair<iterator, bool> insert_or_assign(const key_type& k, _Vp&& v) {
        auto result = tree_.__emplace_unique(k, std::forward<_Vp>(v));
        auto& [iter, inserted] = result;
        if (!inserted) {
            iter->second = std::forward<_Vp>(v);
        }
        return result;
    }

    template <class _Vp>
    std::pair<iterator, bool> insert_or_assign(key_type&& k, _Vp&& v) {
        auto result = tree_.__emplace_unique(std::move(k), std::forward<_Vp>(v));
        auto& [iter, inserted] = result;
        if (!inserted) {
            iter->second = std::forward<_Vp>(v);
        }
        return result;
    }

    template <class _Vp>
    iterator insert_or_assign(const_iterator hint, const key_type& k, _Vp&& v) {
        auto [r, inserted] = tree_.__emplace_hint_unique(hint.i_, k, std::forward<_Vp>(v));
        if (!inserted) {
            r->second = std::forward<_Vp>(v);
        }
        return r;
    }

    template <class _Vp>
    iterator insert_or_assign(const_iterator hint, key_type&& k, _Vp&& v) {
        auto [r, inserted] = tree_.__emplace_hint_unique(hint.i_, std::move(k), std::forward<_Vp>(v));
        if (!inserted) {
            r->second = std::forward<_Vp>(v);
        }
        return r;
    }

    iterator erase(const_iterator p) { return tree_.erase(p.i_); }
    iterator erase(iterator p) { return tree_.erase(p.i_); }
    size_type erase(const key_type& k) { return tree_.__erase_unique(k); }
    iterator erase(const_iterator f, const_iterator l) { return tree_.erase(f.i_, l.i_); }
    
    void clear() noexcept { tree_.clear(); }

    insert_return_type insert(node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(nh.empty() || nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to map::insert()");
        return tree_.template __node_handle_insert_unique< node_type, insert_return_type>(std::move(nh));
    }
    iterator insert(const_iterator __hint, node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(nh.empty() || nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to map::insert()");
        return tree_.template __node_handle_insert_unique<node_type>(__hint.i_, std::move(nh));
    }
    [[nodiscard]] node_type extract(key_type const& __key) {
        return tree_.template __node_handle_extract<node_type>(__key);
    }
    [[nodiscard]] node_type extract(const_iterator __it) {
        return tree_.template __node_handle_extract<node_type>(__it.i_);
    }
    template <class _Compare2>
    void merge(map<key_type, mapped_type, _Compare2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.__node_handle_merge_unique(source.tree_);
    }
    template <class _Compare2>
    void merge(map<key_type, mapped_type, _Compare2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.__node_handle_merge_unique(source.tree_);
    }
    template <class _Compare2>
    void merge(multimap<key_type, mapped_type, _Compare2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.__node_handle_merge_unique(source.tree_);
    }
    template <class _Compare2>
    void merge(multimap<key_type, mapped_type, _Compare2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.__node_handle_merge_unique(source.tree_);
    }

    void swap(map& __m) noexcept(std::is_nothrow_swappable_v<Tree_>) { tree_.swap(__m.tree_); }

    [[nodiscard]] iterator find(const key_type& key) { return tree_.find(key); }
    [[nodiscard]] const_iterator find(const key_type& key) const { return tree_.find(key); }

    template <
        typename _K2
      , std::enable_if_t<
            __is_transparent_v<CompareT, _K2> || __is_transparently_comparable_v<CompareT, key_type, _K2>
          , int
        > = 0
    >
    [[nodiscard]] iterator find(const _K2& key) {
        return tree_.find(key);
    }

    template <
        typename _K2
      , std::enable_if_t<
            __is_transparent_v<CompareT, _K2> || __is_transparently_comparable_v<CompareT, key_type, _K2>
          , int
        > = 0
    >
    [[nodiscard]] const_iterator find(const _K2& k) const {
        return tree_.find(k);
    }

    [[nodiscard]] size_type count(const key_type& k) const {
        return tree_.__count_unique(k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0>
    [[nodiscard]] size_type count(const _K2& key) const {
        return tree_.__count_multi(key);
    }

    [[nodiscard]] bool contains(const key_type& key) const { return find(key) != end(); }

    template <
        typename _K2
      , std::enable_if_t<
            __is_transparent_v<CompareT, _K2> || __is_transparently_comparable_v<CompareT, key_type, _K2>
          , int
        > = 0
    >
    [[nodiscard]] bool contains(const _K2& k) const {
        return find(k) != end();
    }

    [[nodiscard]] iterator lower_bound(const key_type& k) {
        return tree_.__lower_bound_unique(k);
    }

    [[nodiscard]] const_iterator lower_bound(const key_type& k) const {
        return tree_.__lower_bound_unique(k);
    }

    // The transparent versions of the lookup functions use the _multi version, since a non-element key is allowed to
    // match multiple elements.
    template <
        typename _K2
      , std::enable_if_t<
            __is_transparent_v<CompareT, _K2>
         || __is_transparently_comparable_v<CompareT, key_type, _K2>
         ,  int
        > = 0
    >
    [[nodiscard]] iterator lower_bound(const _K2& k) {
        return tree_.__lower_bound_multi(k);
    }

    template <
        typename _K2
      , std::enable_if_t<
            __is_transparent_v<CompareT, _K2>
         || __is_transparently_comparable_v<CompareT, key_type, _K2>
         , int
        > = 0
    >
    [[nodiscard]] const_iterator lower_bound(const _K2& k) const {
        return tree_.__lower_bound_multi(k);
    }

    [[nodiscard]] iterator upper_bound(const key_type& k) {
        return tree_.__upper_bound_unique(k);
    }

    [[nodiscard]] const_iterator upper_bound(const key_type& k) const {
        return tree_.__upper_bound_unique(k);
    }

    template <
        typename _K2
      , std::enable_if_t<
            __is_transparent_v<CompareT, _K2>
         || __is_transparently_comparable_v<CompareT, key_type, _K2>
         ,  int
        > = 0
    >
    [[nodiscard]] iterator upper_bound(const _K2& k) {
        return tree_.__upper_bound_multi(k);
    }
    template <
        typename _K2
      , std::enable_if_t<
            __is_transparent_v<CompareT, _K2>
         || __is_transparently_comparable_v<CompareT, key_type, _K2>
         ,  int
        > = 0
    >
    [[nodiscard]] const_iterator upper_bound(const _K2& k) const {
        return tree_.__upper_bound_multi(k);
    }
    [[nodiscard]] std::pair<iterator, iterator> equal_range(const key_type& k) {
        return tree_.__equal_range_unique(k);
    }
    [[nodiscard]] std::pair<const_iterator, const_iterator> equal_range(const key_type& k) const {
        return tree_.__equal_range_unique(k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0>
    [[nodiscard]] std::pair<iterator, iterator> equal_range(const _K2& k) {
        return tree_.__equal_range_multi(k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0>
    [[nodiscard]] std::pair<const_iterator, const_iterator> equal_range(const _K2& k) const {
        return tree_.__equal_range_multi(k);
    }

private:
    typedef typename Tree_::__node __node;
    typedef typename Tree_::__node_allocator __node_allocator;
    typedef typename Tree_::__node_pointer __node_pointer;
    typedef typename Tree_::__node_base_pointer __node_base_pointer;

    typedef MapNodeDestructor<__node_allocator> _Dp;
    typedef std::unique_ptr<__node, _Dp> __node_holder;

    friend struct __specialized_algorithm<_Algorithm::__for_each, __single_range<map> >;
};

template <
    class InputIteratorT
  , class CompareT   = std::less<__iter_key_type<InputIteratorT>>
  , class AllocatorT = std::allocator<__iter_to_alloc_type<InputIteratorT>>
  , class            = std::enable_if_t<__has_input_iterator_category<InputIteratorT>::value, void>
  , class            = std::enable_if_t<!__is_allocator_v<CompareT>>
  , class            = std::enable_if_t<__is_allocator_v<AllocatorT>>
>
map(
    InputIteratorT,
    InputIteratorT,
    CompareT = CompareT(),
    AllocatorT = AllocatorT()
) -> map<
        __iter_key_type<InputIteratorT>
      , __iter_mapped_type<InputIteratorT>
      , CompareT, AllocatorT
>;

template <
    std::ranges::input_range RangeT
  , class CompareT   = std::less<__range_key_type<RangeT>>
  , class AllocatorT = std::allocator<__range_to_alloc_type<RangeT>>
  , class            = std::enable_if_t<!__is_allocator_v<CompareT>>
  , class            = std::enable_if_t<__is_allocator_v<AllocatorT>>
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
  , class _Tp
  , class CompareT   = std::less<std::remove_const_t<KeyT>>
  , class AllocatorT = std::allocator<std::pair<const KeyT, _Tp>>
  , class            = std::enable_if_t<!__is_allocator_v<CompareT>>
  , class            = std::enable_if_t<__is_allocator_v<AllocatorT>>
>
map(
    std::initializer_list<std::pair<KeyT, _Tp>>,
    CompareT = CompareT(),
    AllocatorT = AllocatorT()
) -> map<
         std::remove_const_t<KeyT>
       , _Tp
       , CompareT
       , AllocatorT
     >;

template <
    class InputIteratorT
  , class AllocatorT
  , class = std::enable_if_t<__has_input_iterator_category<InputIteratorT>::value, void>
  , class = std::enable_if_t<__is_allocator_v<AllocatorT>>
>
map(InputIteratorT, InputIteratorT, AllocatorT)
-> map<
    __iter_key_type<InputIteratorT>
  , __iter_mapped_type<InputIteratorT>
  , std::less<__iter_key_type<InputIteratorT>>
  , AllocatorT
>;

template<
    std::ranges::input_range RangeT
  , class AllocatorT
  , class = std::enable_if_t<__is_allocator_v<AllocatorT>>
>
map(std::from_range_t, RangeT&&, AllocatorT)
-> map<
       __range_key_type<RangeT>
     , __range_mapped_type<RangeT>
     , std::less<__range_key_type<RangeT>>
     , AllocatorT
   >;

template<
    class KeyT,
    class _Tp,
    class AllocatorT,
    class = std::enable_if_t<__is_allocator_v<AllocatorT>>
>
map(std::initializer_list<std::pair<KeyT, _Tp>>, AllocatorT)
-> map<
       std::remove_const_t<KeyT>
     , _Tp
     , std::less<std::remove_const_t<KeyT>>
     , AllocatorT
   >;

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
struct __specialized_algorithm<_Algorithm::__for_each, __single_range<map<KeyT, _Tp, CompareT, AllocatorT>>> {
    using __map = map<KeyT, _Tp, CompareT, AllocatorT>;

    static const bool __has_algorithm = true;

    template <class _Map, class _Func, class _Proj>
    static auto operator()(_Map&& __map, _Func __func, _Proj __proj) {
        auto [_, __func2] = __specialized_algorithm<_Algorithm::__for_each, __single_range<typename __map::Tree_>>()(
            __map.tree_, std::move(__func), std::move(__proj));
        return std::make_pair(__map.end(), std::move(__func2));
    }
};

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
_Tp& map<KeyT, _Tp, CompareT, AllocatorT>::operator[](const key_type& key) {
    return tree_
        .__emplace_unique(
            std::piecewise_construct,
            std::forward_as_tuple(key),
            std::forward_as_tuple()
        )
        .first
        ->second;
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
_Tp& map<KeyT, _Tp, CompareT, AllocatorT>::operator[](key_type&& key) {
    return tree_
        .__emplace_unique(
            std::piecewise_construct,
            std::forward_as_tuple(std::move(key)),
            std::forward_as_tuple()
        )
        .first
        ->second;
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
_Tp& map<KeyT, _Tp, CompareT, AllocatorT>::at(const key_type& key) {
    auto [_, child] = tree_.__find_equal(key);
    if (child == nullptr)
        std::__throw_out_of_range("map::at:  key not found");
    return static_cast<__node_pointer>(child)->__get_value().second;
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
const _Tp& map<KeyT, _Tp, CompareT, AllocatorT>::at(const key_type& key) const {
    auto [_, child] = tree_.__find_equal(key);
    if (child == nullptr)
        std::__throw_out_of_range("map::at:  key not found");
    return static_cast<__node_pointer>(child)->__get_value().second;
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
inline bool
operator==(const map<KeyT, _Tp, CompareT, AllocatorT>& x, const map<KeyT, _Tp, CompareT, AllocatorT>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
__synth_three_way_result<std::pair<const KeyT, _Tp>>
operator<=>(const map<KeyT, _Tp, CompareT, AllocatorT>& x, const map<KeyT, _Tp, CompareT, AllocatorT>& y) {
    return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(), y.end(), mstd::__synth_three_way);
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
inline void
swap(map<KeyT, _Tp, CompareT, AllocatorT>& x, map<KeyT, _Tp, CompareT, AllocatorT>& y)
noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT, class _Predicate>
inline typename map<KeyT, _Tp, CompareT, AllocatorT>::size_type
erase_if(map<KeyT, _Tp, CompareT, AllocatorT>& __c, _Predicate pred) {
    return mstd::_MSTD_erase_if_container(__c, pred);
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
struct __container_traits<map<KeyT, _Tp, CompareT, AllocatorT> > {
    // http://eel.is/c++draft/associative.reqmts.except#2
    // For associative containers, if an exception is thrown by any operation from within
    // an insert or emplace function inserting a single element, the insertion has no effect.
    static constexpr const bool __emplacement_has_strong_exception_safety_guarantee = true;

    static constexpr const bool __reservable = false;
};

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
class multimap {
public:
    // types:
    typedef KeyT key_type;
    typedef _Tp mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef std::type_identity_t<CompareT> key_compare;
    typedef std::type_identity_t<AllocatorT> allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    static_assert(__check_valid_allocator<allocator_type>::value, "");
    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

    class value_compare : public std::binary_function<value_type, value_type, bool> {
        friend class multimap;

    protected:
        key_compare comp;

        value_compare(key_compare __c) : comp(__c) {}

    public:
        bool operator()(const value_type& x, const value_type& y) const {
            return comp(x.first, y.first);
        }
    };

private:
    using ValueType_    = ValueType<key_type, mapped_type>;
    using ValueCompare_ = MapValueCompare<key_type, value_type, key_compare>;
    using Tree_         = Tree<ValueType_, ValueCompare_, allocator_type>;
    using AllocTraits_  = allocator_traits<allocator_type>;

    Tree_ tree_;

public:
    typedef typename AllocTraits_::pointer pointer;
    typedef typename AllocTraits_::const_pointer const_pointer;
    typedef typename AllocTraits_::size_type size_type;
    typedef typename AllocTraits_::difference_type difference_type;
    typedef MapIterator<typename Tree_::iterator> iterator;
    typedef MapConstIterator<typename Tree_::const_iterator> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef MapNodeHandle<typename Tree_::__node, allocator_type> node_type;

    template <class _Key2, class _Value2, class _Comp2, class _Alloc2>
    friend class map;
    template <class _Key2, class _Value2, class _Comp2, class _Alloc2>
    friend class multimap;

    multimap() noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value
     && std::is_nothrow_default_constructible<key_compare>::value
     && std::is_nothrow_copy_constructible<key_compare>::value
    )
    : tree_(ValueCompare_(key_compare())) {}

    explicit multimap(const key_compare& comp) noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value
     && std::is_nothrow_copy_constructible<key_compare>::value
    )
    : tree_(ValueCompare_(comp)) {}

    explicit multimap(const key_compare& comp, const allocator_type& alloc)
    : tree_(ValueCompare_(comp), typename Tree_::allocator_type(alloc)) {}

    template <class InputIteratorT>
    multimap(InputIteratorT begin, InputIteratorT end, const key_compare& comp = key_compare())
    : tree_(ValueCompare_(comp)) {
        insert(begin, end);
    }

    template <class InputIteratorT>
    multimap(InputIteratorT begin, InputIteratorT end, const key_compare& comp, const allocator_type& alloc)
    : tree_(ValueCompare_(comp), typename Tree_::allocator_type(alloc)) {
        insert(begin, end);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    multimap(std::from_range_t,
             RangeT&& range,
             const key_compare& comp = key_compare(),
             const allocator_type& alloc = allocator_type())
    : tree_(ValueCompare_(comp), typename Tree_::allocator_type(alloc)) {
        insert_range(std::forward<RangeT>(range));
    }

    template <class InputIteratorT>
    multimap(InputIteratorT begin, InputIteratorT end, const allocator_type& alloc)
    : multimap(begin, end, key_compare(), alloc) {}
    
    template <_ContainerCompatibleRange<value_type> RangeT>
    multimap(std::from_range_t, RangeT&& range, const allocator_type& alloc)
    : multimap(std::from_range, std::forward<RangeT>(range), key_compare(), alloc) {}
    
    multimap(const multimap& __m) = default;

    multimap& operator=(const multimap& __m) = default;

    multimap(multimap&& __m) = default;

    multimap(multimap&& __m, const allocator_type& alloc)
    : tree_(std::move(__m.tree_), alloc) {}

    multimap& operator=(multimap&& other) = default;

    multimap(std::initializer_list<value_type> init_list, const key_compare& comp = key_compare())
    : tree_(ValueCompare_(comp)) {
        insert(init_list.begin(), init_list.end());
    }

    multimap(std::initializer_list<value_type> init_list, const key_compare& comp, const allocator_type& alloc)
    : tree_(ValueCompare_(comp), typename Tree_::allocator_type(alloc)) {
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

    multimap(const multimap& __m, const allocator_type& alloc)
    : tree_(__m.tree_, alloc) {}

    ~multimap() {
        static_assert(sizeof(mstd::__diagnose_non_const_comparator<KeyT, CompareT>()), "");
    }

    [[nodiscard]] iterator begin() noexcept { return tree_.begin(); }
    [[nodiscard]] const_iterator begin() const noexcept { return tree_.begin(); }
    [[nodiscard]] iterator end() noexcept { return tree_.end(); }
    [[nodiscard]] const_iterator end() const noexcept { return tree_.end(); }

    [[nodiscard]] reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    [[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
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
        return allocator_type(tree_.__alloc());
    }
    [[nodiscard]] key_compare key_comp() const { return tree_.value_comp().key_comp(); }
    [[nodiscard]] value_compare value_comp() const {
        return value_compare(tree_.value_comp().key_comp());
    }

    template <class... ArgsT>
    iterator emplace(ArgsT&&... args) {
        return tree_.__emplace_multi(std::forward<ArgsT>(args)...);
    }

    template <class... ArgsT>
    iterator emplace_hint(const_iterator pos, ArgsT&&... args) {
        return tree_.__emplace_hint_multi(pos.i_, std::forward<ArgsT>(args)...);
    }

    template <
        class _Pp
      , std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0
    >
    iterator insert(_Pp&& pos) {
        return tree_.__emplace_multi(std::forward<_Pp>(pos));
    }

    template <
        class _Pp
      , std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0
    >
    iterator insert(const_iterator pos, _Pp&& val) {
        return tree_.__emplace_hint_multi(pos.i_, std::forward<_Pp>(val));
    }

    iterator insert(value_type&& value) {
        return tree_.__emplace_multi(std::move(value));
    }

    iterator insert(const_iterator pos, value_type&& __v) {
        return tree_.__emplace_hint_multi(pos.i_, std::move(__v));
    }

    void insert(std::initializer_list<value_type> init_list) {
        insert(init_list.begin(), init_list.end());
    }

    iterator insert(const value_type& __v) {
        return tree_.__emplace_multi(__v);
    }

    iterator insert(const_iterator pos, const value_type& __v) {
        return tree_.__emplace_hint_multi(pos.i_, __v);
    }

    template <class InputIteratorT>
    void insert(InputIteratorT begin, InputIteratorT end) {
        tree_.__insert_range_multi(begin, end);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    void insert_range(RangeT&& range) {
        tree_.__insert_range_multi(std::ranges::begin(range),  std::ranges::end(range));
    }

    iterator erase(const_iterator pos) {
        return tree_.erase(pos.i_);
    }

    iterator erase(iterator pos) {
        return tree_.erase(pos.i_);
    }

    size_type erase(const key_type& k) {
        return tree_.__erase_multi(k);
    }

    iterator erase(const_iterator begin, const_iterator end) {
        return tree_.erase(begin.i_, end.i_);
    }

    iterator insert(node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            nh.empty() || nh.get_allocator() == get_allocator(),
            "node_type with incompatible allocator passed to multimap::insert()"
        );
        return tree_.template __node_handle_insert_multi<node_type>(std::move(nh));
    }
    iterator insert(const_iterator __hint, node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            nh.empty() || nh.get_allocator() == get_allocator(),
            "node_type with incompatible allocator passed to multimap::insert()"
        );
        return tree_.template __node_handle_insert_multi<node_type>(__hint.i_, std::move(nh));
    }
    [[nodiscard]] node_type extract(key_type const& __key) {
        return tree_.template __node_handle_extract<node_type>(__key);
    }
    [[nodiscard]] node_type extract(const_iterator __it) {
        return tree_.template __node_handle_extract<node_type>(__it.i_);
    }
    template <class _Compare2>
    void merge(multimap<key_type, mapped_type, _Compare2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        return tree_.__node_handle_merge_multi(source.tree_);
    }
    template <class _Compare2>
    void merge(multimap<key_type, mapped_type, _Compare2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        return tree_.__node_handle_merge_multi(source.tree_);
    }
    template <class _Compare2>
    void merge(map<key_type, mapped_type, _Compare2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        return tree_.__node_handle_merge_multi(source.tree_);
    }
    template <class _Compare2>
    void merge(map<key_type, mapped_type, _Compare2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(),
            "merging container with incompatible allocator"
        );
        return tree_.__node_handle_merge_multi(source.tree_);
    }

    void clear() noexcept { tree_.clear(); }

    void swap(multimap& other) noexcept(std::is_nothrow_swappable_v<Tree_>) {
        tree_.swap(other.tree_);
    }

    [[nodiscard]] iterator find(const key_type& key) {
        return tree_.find(key);
    }

    [[nodiscard]] const_iterator find(const key_type& key) const {
        return tree_.find(key);
    }
    
    template <
        typename _K2
      , std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0
    >
    [[nodiscard]] iterator find(const _K2& key) {
        return tree_.find(key);
    }
    template <
        typename _K2
      , std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0
    >
    [[nodiscard]] const_iterator find(const _K2& key) const {
        return tree_.find(key);
    }

    [[nodiscard]] size_type count(const key_type& key) const {
        return tree_.__count_multi(key);
    }
    template <
        typename _K2
      , std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0
    >
    [[nodiscard]] size_type count(const _K2& key) const {
        return tree_.__count_multi(key);
    }

    [[nodiscard]] bool contains(const key_type& key) const {
        return find(key) != end();
    }

    template <
        typename _K2
      , std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0
    >
    [[nodiscard]] bool contains(const _K2& key) const {
        return find(key) != end();
    }

    [[nodiscard]] iterator lower_bound(const key_type& key) {
        return tree_.__lower_bound_multi(key);
    }

    [[nodiscard]] const_iterator lower_bound(const key_type& key) const {
        return tree_.__lower_bound_multi(key);
    }

    template <
        typename _K2
      , std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0
    >
    [[nodiscard]] iterator lower_bound(const _K2& key) {
        return tree_.__lower_bound_multi(key);
    }

    template <
        typename _K2
      , std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0
    >
    [[nodiscard]] const_iterator lower_bound(const _K2& key) const {
        return tree_.__lower_bound_multi(key);
    }

    [[nodiscard]] iterator upper_bound(const key_type& key) {
        return tree_.__upper_bound_multi(key);
    }

    [[nodiscard]] const_iterator upper_bound(const key_type& key) const {
        return tree_.__upper_bound_multi(key);
    }

    template <
        typename _K2
      , std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0
    >
    [[nodiscard]] iterator upper_bound(const _K2& key) {
        return tree_.__upper_bound_multi(key);
    }
    template <
        typename _K2
      , std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0
    >
    [[nodiscard]] const_iterator upper_bound(const _K2& key) const {
        return tree_.__upper_bound_multi(key);
    }

    [[nodiscard]] std::pair<iterator, iterator>
    equal_range(const key_type& key) {
        return tree_.__equal_range_multi(key);
    }
    [[nodiscard]] std::pair<const_iterator, const_iterator>
    equal_range(const key_type& key) const {
        return tree_.__equal_range_multi(key);
    }
    template <
        typename _K2
      , std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0
    >
    [[nodiscard]] std::pair<iterator, iterator> equal_range(const _K2& key) {
        return tree_.__equal_range_multi(key);
    }

    template <
        typename _K2
      , std::enable_if_t<__is_transparent_v<CompareT, _K2>, int> = 0
    >
    [[nodiscard]] std::pair<const_iterator, const_iterator>
    equal_range(const _K2& key) const {
        return tree_.__equal_range_multi(key);
    }

private:
    typedef typename Tree_::__node __node;
    typedef typename Tree_::__node_allocator __node_allocator;
    typedef typename Tree_::__node_pointer __node_pointer;

    typedef MapNodeDestructor<__node_allocator> _Dp;
    typedef std::unique_ptr<__node, _Dp> __node_holder;

    friend struct __specialized_algorithm<_Algorithm::__for_each, __single_range<multimap> >;
};

template <
    class InputIteratorT
  , class CompareT   = std::less<__iter_key_type<InputIteratorT>>
  , class AllocatorT = std::allocator<__iter_to_alloc_type<InputIteratorT>>
  , class            = std::enable_if_t<__has_input_iterator_category<InputIteratorT>::value, void>
  , class            = std::enable_if_t<!__is_allocator_v<CompareT>>
  , class            = std::enable_if_t<__is_allocator_v<AllocatorT>>
>
multimap(InputIteratorT, InputIteratorT, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multimap<
       __iter_key_type<InputIteratorT>
     , __iter_mapped_type<InputIteratorT>
     , CompareT
     , AllocatorT
   >;

template <
    std::ranges::input_range RangeT
  , class CompareT   = std::less<__range_key_type<RangeT>>
  , class AllocatorT = std::allocator<__range_to_alloc_type<RangeT>>
  , class            = std::enable_if_t<!__is_allocator_v<CompareT>>
  , class            = std::enable_if_t<__is_allocator_v<AllocatorT>>
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
  , class _Tp
  , class CompareT   = std::less<std::remove_const_t<KeyT>>
  , class AllocatorT = std::allocator<std::pair<const KeyT, _Tp>>
  , class            = std::enable_if_t<!__is_allocator_v<CompareT>>
  , class            = std::enable_if_t<__is_allocator_v<AllocatorT>>
>
multimap(std::initializer_list<std::pair<KeyT, _Tp>>, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multimap<std::remove_const_t<KeyT>, _Tp, CompareT, AllocatorT>;

template <
    class InputIteratorT
  , class AllocatorT
  , class = std::enable_if_t<__has_input_iterator_category<InputIteratorT>::value, void>
  , class = std::enable_if_t<__is_allocator_v<AllocatorT>>
>
multimap(InputIteratorT, InputIteratorT, AllocatorT)
-> multimap<
       __iter_key_type<InputIteratorT>
     , __iter_mapped_type<InputIteratorT>
     , std::less<__iter_key_type<InputIteratorT>>
     , AllocatorT
   >;

template <
    std::ranges::input_range RangeT
  , class AllocatorT
  , class = std::enable_if_t<__is_allocator_v<AllocatorT>>
>
multimap(std::from_range_t, RangeT&&, AllocatorT)
-> multimap<
       __range_key_type<RangeT>
     , __range_mapped_type<RangeT>
     , std::less<__range_key_type<RangeT>>
     , AllocatorT
   >;

template <
    class KeyT
  , class _Tp
  , class AllocatorT
  , class = std::enable_if_t<__is_allocator_v<AllocatorT>>
>
multimap(std::initializer_list<std::pair<KeyT, _Tp>>, AllocatorT)
-> multimap<
       std::remove_const_t<KeyT>
     , _Tp
     , std::less<std::remove_const_t<KeyT>>
     , AllocatorT
   >;

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
struct __specialized_algorithm<_Algorithm::__for_each, __single_range<multimap<KeyT, _Tp, CompareT, AllocatorT>>> {
    using __map = multimap<KeyT, _Tp, CompareT, AllocatorT>;

    static const bool __has_algorithm = true;

    template <class _Map, class _Func, class _Proj>
    static auto operator()(_Map&& __map, _Func __func, _Proj __proj) {
        auto [_, __func2] = __specialized_algorithm<_Algorithm::__for_each, __single_range<typename __map::Tree_>>()(
            __map.tree_, std::move(__func), std::move(__proj));
        return std::make_pair(__map.end(), std::move(__func2));
    }
};

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
inline bool
operator==(const multimap<KeyT, _Tp, CompareT, AllocatorT>& x, const multimap<KeyT, _Tp, CompareT, AllocatorT>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
__synth_three_way_result<std::pair<const KeyT, _Tp>>
operator<=>(const multimap<KeyT, _Tp, CompareT, AllocatorT>& x,
            const multimap<KeyT, _Tp, CompareT, AllocatorT>& y) {
    return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(), y.end(), __synth_three_way);
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
inline void
swap(multimap<KeyT, _Tp, CompareT, AllocatorT>& x, multimap<KeyT, _Tp, CompareT, AllocatorT>& y)
noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT, class _Predicate>
inline typename multimap<KeyT, _Tp, CompareT, AllocatorT>::size_type
erase_if(multimap<KeyT, _Tp, CompareT, AllocatorT>& __c, _Predicate pred) {
    return mstd::_MSTD_erase_if_container(__c, pred);
}

template <class KeyT, class _Tp, class CompareT, class AllocatorT>
struct __container_traits<multimap<KeyT, _Tp, CompareT, AllocatorT> > {
    // http://eel.is/c++draft/associative.reqmts.except#2
    // For associative containers, if an exception is thrown by any operation from within
    // an insert or emplace function inserting a single element, the insertion has no effect.
    static constexpr const bool __emplacement_has_strong_exception_safety_guarantee = true;
    static constexpr const bool __reservable = false;
};

} // namespace mstd

namespace mstd {
namespace pmr {
    template <class _KeyT, class _ValueT, class _CompareT = std::less<_KeyT>>
    using map = mstd::map<_KeyT, _ValueT, _CompareT, std::pmr::polymorphic_allocator<std::pair<const _KeyT, _ValueT>>>;
    template <class _KeyT, class _ValueT, class _CompareT = std::less<_KeyT>>
    using multimap = mstd::multimap<_KeyT, _ValueT, _CompareT, std::pmr::polymorphic_allocator<std::pair<const _KeyT, _ValueT>>>;
} // namespace pmr
} // namespace mstd


#endif // MSTD_MAP
