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
#include <functional>
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
#include <detail/type_traits/is_allocator.hpp>
#include <utility>
#include <detail/ranges/container_compatible_range.hpp>
#include <detail/utility/compare_three_way.hpp>
#include <stdexcept>
#include <tuple>
#include <version>
#include <compare>
#include <initializer_list>

namespace mstd {

template <class KeyT, class _CP, class CompareT>
class MapValueCompare {
    CompareT comp_;

public:
    MapValueCompare() noexcept(std::is_nothrow_default_constructible<CompareT>::value)
    : comp_() {}
    MapValueCompare(CompareT c) noexcept(std::is_nothrow_copy_constructible<CompareT>::value)
    : comp_(c) {}
    const CompareT& key_comp() const noexcept { return comp_; }

    auto operator()(const _CP& x, const _CP& y) const { return comp_(x.first, y.first); }
    void swap(MapValueCompare& y) noexcept(std::is_nothrow_swappable_v<CompareT>) { std::swap(comp_, y.comp_); }

    template <typename TransparentKey>
    auto operator()(const TransparentKey& x, const _CP& y) const { return comp_(x, y.first); }

    template <typename TransparentKey>
    auto operator()(const _CP& x, const TransparentKey& y) const { return comp_(x.first, y); }
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
    using AllocTraits_   = std::allocator_traits<AllocatorType_>;

public:
    using pointer = AllocTraits_::pointer;

private:
    AllocatorType_& na_;

public:
    bool first_constructed;
    bool second_constructed;

    explicit MapNodeDestructor(AllocatorType_& na) noexcept
    : na_{na}
    , first_constructed{false}
    , second_constructed{false} {}

    MapNodeDestructor(TreeNodeDestructor<AllocatorType_>&& x) noexcept
    : na_(x.na_)
    , first_constructed(x.value_constructed)
    , second_constructed(x.value_constructed) {
        x.value_constructed = false;
    }

    MapNodeDestructor& operator=(const MapNodeDestructor&) = delete;

    void operator()(pointer ptr) noexcept {
        if (second_constructed) {
            AllocTraits_::destroy(na_, std::addressof(ptr->get_value().second));
        }
        if (first_constructed) {
            AllocTraits_::destroy(na_, std::addressof(ptr->get_value().first));
        }
        if (ptr) {
            AllocTraits_::deallocate(na_, ptr, 1);
        }
    }
};

template <class KeyT, class ValueT>
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
};

template <class TreeIteratorT>
class MapConstIterator {
    TreeIteratorT i_;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = typename TreeIteratorT::value_type;
    using difference_type   = typename TreeIteratorT::difference_type;
    using reference         = const value_type&;
    using pointer           = typename TreeIteratorT::pointer;

    MapConstIterator() noexcept {}

    MapConstIterator(TreeIteratorT i) noexcept : i_(i) {}
    MapConstIterator(MapIterator< typename TreeIteratorT::non_const_iterator> i) noexcept : i_(i.i_) {}

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
    friend class TreeConstIterator;
};

template <class KeyT, class ValueT, class CompareT = CompareThreeWay, class AllocatorT = std::allocator<std::pair<const KeyT, ValueT> > >
class multimap;

template <class KeyT, class ValueT, class CompareT = CompareThreeWay, class AllocatorT = std::allocator<std::pair<const KeyT, ValueT> > >
class map {
public:
    // types:
    using key_type = KeyT;
    using mapped_type = ValueT;
    using value_type = std::pair<const key_type, mapped_type>;
    using key_compare = std::type_identity_t<CompareT>;
    using allocator_type = std::type_identity_t<AllocatorT>;
    using reference = value_type&;
    using const_reference = const value_type&;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

    class value_compare {
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
    using AllocTraits_  = std::allocator_traits<allocator_type>;

    static_assert(__check_valid_allocator<allocator_type>::value, "");

    Tree_ tree_;

public:
    using pointer = AllocTraits_::pointer;
    using const_pointer = AllocTraits_::const_pointer;
    using size_type = AllocTraits_::size_type;
    using difference_type = AllocTraits_::difference_type;
    using iterator               = MapIterator<typename Tree_::iterator>;
    using const_iterator         = MapConstIterator<typename Tree_::const_iterator>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    template <class Self>
    using SelfIterator = std::conditional_t<std::is_const_v<Self>, const_iterator, iterator>;
    template <class Self>
    using SelfSubrange = std::pair<SelfIterator<Self>, SelfIterator<Self>>;

    using node_type = MapNodeHandle<typename Tree_::node, allocator_type>;
    using insert_return_type = __insert_return_type<iterator, node_type>;

    template <class _Key2, class _Value2, class Comp2T, class _Alloc2>
    friend class map;
    template <class _Key2, class _Value2, class Comp2T, class _Alloc2>
    friend class multimap;

    map() noexcept(
       std::is_nothrow_default_constructible<allocator_type>::value
    && std::is_nothrow_default_constructible<key_compare>::value
    && std::is_nothrow_copy_constructible<key_compare>::value
    )
    : tree_(ValueCompare_(key_compare())) {}

    explicit map(const key_compare& comp) noexcept(
       std::is_nothrow_default_constructible<allocator_type>::value 
    && std::is_nothrow_copy_constructible<key_compare>::value
    )
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

    mapped_type& operator[](const key_type& key);
    mapped_type& operator[](key_type&& key);

    template <class ArgT>
    //requires __is_transparently_comparable_v<CompareT, key_type, std::remove_cvref_t<ArgT>>
    [[nodiscard]] mapped_type& at(ArgT&& arg) {
        auto [_, child] = tree_.find_equal(arg);
        if (child == nullptr) {
            std::__throw_out_of_range("map::at:  key not found");
        }
        return static_cast<__node_pointer>(child)->get_value().second;
    }

    template <class ArgT>
    //requires __is_transparently_comparable_v<CompareT, key_type, std::remove_cvref_t<ArgT>>
    [[nodiscard]] const mapped_type& at(ArgT&& arg) const {
        auto [_, child] = tree_.find_equal(arg);
        if (child == nullptr) {
            std::__throw_out_of_range("map::at:  key not found");
        }
        return static_cast<__node_pointer>(child)->get_value().second;
    }

    [[nodiscard]] mapped_type& at(const key_type& key);
    [[nodiscard]] const mapped_type& at(const key_type& key) const;

    [[nodiscard]] allocator_type get_allocator() const noexcept {
        return allocator_type(tree_.alloc());
    }
    [[nodiscard]] key_compare key_comp() const { return tree_.value_comp().key_comp(); }
    [[nodiscard]] value_compare value_comp() const {
        return value_compare(tree_.value_comp().key_comp());
    }

    template <class... ArgsT>
    std::pair<iterator, bool> emplace(ArgsT&&... args) {
        return tree_.emplaceUnique(std::forward<ArgsT>(args)...);
    }

    template <class... ArgsT>
    iterator emplace_hint(const_iterator pos, ArgsT&&... args) {
        return tree_
            .emplaceHintUnique(pos.i_, std::forward<ArgsT>(args)...)
            .first;
    }

    template <class _Pp>
    requires std::is_constructible_v<value_type, _Pp>
    std::pair<iterator, bool> insert(_Pp&& value) {
        return tree_.emplaceUnique(std::forward<_Pp>(value));
    }

    template <class _Pp>
    requires std::is_constructible_v<value_type, _Pp>
    iterator insert(const_iterator pos, _Pp&& value) {
        return tree_
            .emplaceHintUnique(pos.i_, std::forward<_Pp>(value))
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
        return tree_.emplaceHintUnique(pos.i_, std::move(value)).first;
    }

    void insert(std::initializer_list<value_type> init_list) {
        insert(init_list.begin(), init_list.end());
    }

    template <class InputIteratorT>
    void insert(InputIteratorT first, InputIteratorT last) {
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
            hint.i_,
            std::piecewise_construct,
            std::forward_as_tuple(k),
            std::forward_as_tuple(std::forward<ArgsT>(args)...)
        ).first;
    }

    template <class... ArgsT>
    iterator try_emplace(const_iterator hint, key_type&& k, ArgsT&&... args) {
        return tree_.emplaceHintUnique(
            hint.i_,
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
            iter->second = std::forward<_Vp>(v);
        }
        return result;
    }

    template <class _Vp>
    std::pair<iterator, bool> insert_or_assign(key_type&& k, _Vp&& v) {
        auto result = tree_.emplaceUnique(std::move(k), std::forward<_Vp>(v));
        auto& [iter, inserted] = result;
        if (!inserted) {
            iter->second = std::forward<_Vp>(v);
        }
        return result;
    }

    template <class _Vp>
    iterator insert_or_assign(const_iterator hint, const key_type& k, _Vp&& v) {
        auto [r, inserted] = tree_.emplaceHintUnique(hint.i_, k, std::forward<_Vp>(v));
        if (!inserted) {
            r->second = std::forward<_Vp>(v);
        }
        return r;
    }

    template <class _Vp>
    iterator insert_or_assign(const_iterator hint, key_type&& k, _Vp&& v) {
        auto [r, inserted] = tree_.emplaceHintUnique(hint.i_, std::move(k), std::forward<_Vp>(v));
        if (!inserted) {
            r->second = std::forward<_Vp>(v);
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

    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfIterator<Self> find(this Self& self, const TransparentKey& key) {
        return self.tree_.find(key);
    }

    template <typename TransparentKey>
    [[nodiscard]] size_type count(const TransparentKey& key) const {
        return tree_.countMulti(key);
    }

    template <typename TransparentKey>
    [[nodiscard]] bool contains(const TransparentKey& k) const {
        return find(k) != end();
    }

    template <class Self>
    [[nodiscard]] SelfIterator<Self> lower_bound(this Self& self, const key_type& k) {
        return self.tree_.lowerBoundUnique(k);
    }

    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfIterator<Self> lower_bound(this Self& self, const TransparentKey& k) {
        return self.tree_.lowerBoundMulti(k);
    }

    template <class Self>
    [[nodiscard]] SelfIterator<Self> upper_bound(this Self& self, const key_type& k) {
        return self.tree_.upperBoundUnique(k);
    }

    template <class Self, typename TransparentKey>
    [[nodiscard]] SelfIterator<Self> upper_bound(this Self& self, const TransparentKey& k) {
        return self.tree_.upperBoundMulti(k);
    }

    template <class Self>
    [[nodiscard]] SelfSubrange<Self> equal_range(this Self& self, const key_type& k) {
        return self.tree_.equalRangeUnique(k);
    }

    template <class Self, typename TransparentKey>
    [[nodiscard]] std::pair<iterator, iterator> equal_range(this Self& self, const TransparentKey& k) {
        return self.tree_.equalRangeMulti(k);
    }

private:
    using node = Tree_::node;
    using node_allocator = Tree_::node_allocator;
    using __node_pointer = Tree_::node_pointer;
    using node_base_pointer = Tree_::node_base_pointer;

    using _Dp = MapNodeDestructor<node_allocator>;
    using __node_holder = std::unique_ptr<node, _Dp>;
};

template <
    class InputIteratorT
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<__iter_to_alloc_type<InputIteratorT>>
>
requires __has_input_iterator_category<InputIteratorT>
      && (!__is_allocator_v<CompareT>)
      && __is_allocator_v<AllocatorT>
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
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<__range_to_alloc_type<RangeT>>
>
requires (!__is_allocator_v<CompareT>) && __is_allocator_v<AllocatorT>
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
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<std::pair<const KeyT, ValueT>>
>
requires (!__is_allocator_v<CompareT>) && __is_allocator_v<AllocatorT>
map(
    std::initializer_list<std::pair<KeyT, ValueT>>,
    CompareT = CompareT(),
    AllocatorT = AllocatorT()
) -> map<
         std::remove_const_t<KeyT>
       , ValueT
       , CompareT
       , AllocatorT
     >;

template <class InputIteratorT, class AllocatorT>
requires __has_input_iterator_category<InputIteratorT> && __is_allocator_v<AllocatorT>
map(InputIteratorT, InputIteratorT, AllocatorT)
-> map<
    __iter_key_type<InputIteratorT>
  , __iter_mapped_type<InputIteratorT>
  , CompareThreeWay
  , AllocatorT
>;

template<std::ranges::input_range RangeT, class AllocatorT>
requires __is_allocator_v<AllocatorT>
map(std::from_range_t, RangeT&&, AllocatorT)
-> map<
       __range_key_type<RangeT>
     , __range_mapped_type<RangeT>
     , CompareThreeWay
     , AllocatorT
   >;

template<class KeyT, class ValueT, class AllocatorT>
requires __is_allocator_v<AllocatorT>
map(std::initializer_list<std::pair<KeyT, ValueT>>, AllocatorT)
-> map<
       std::remove_const_t<KeyT>
     , ValueT
     , CompareThreeWay
     , AllocatorT
   >;

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
ValueT& map<KeyT, ValueT, CompareT, AllocatorT>::operator[](const key_type& key) {
    return tree_
        .emplaceUnique(
            std::piecewise_construct,
            std::forward_as_tuple(key),
            std::forward_as_tuple()
        )
        .first
        ->second;
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
ValueT& map<KeyT, ValueT, CompareT, AllocatorT>::operator[](key_type&& key) {
    return tree_
        .emplaceUnique(
            std::piecewise_construct,
            std::forward_as_tuple(std::move(key)),
            std::forward_as_tuple()
        )
        .first
        ->second;
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
ValueT& map<KeyT, ValueT, CompareT, AllocatorT>::at(const key_type& key) {
    auto [_, child] = tree_.find_equal(key);
    if (child == nullptr)
        std::__throw_out_of_range("map::at:  key not found");
    return static_cast<__node_pointer>(child)->get_value().second;
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
const ValueT& map<KeyT, ValueT, CompareT, AllocatorT>::at(const key_type& key) const {
    auto [_, child] = tree_.find_equal(key);
    if (child == nullptr)
        std::__throw_out_of_range("map::at:  key not found");
    return static_cast<__node_pointer>(child)->get_value().second;
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
inline bool
operator==(const map<KeyT, ValueT, CompareT, AllocatorT>& x, const map<KeyT, ValueT, CompareT, AllocatorT>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
auto
operator<=>(const map<KeyT, ValueT, CompareT, AllocatorT>& x, const map<KeyT, ValueT, CompareT, AllocatorT>& y) {
    return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(), y.end(),
    [cmp = CompareThreeWay{}](const auto& val1, const auto& val2){ return cmp(val1.first, val2.first); });
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
    // types:
    using key_type        = KeyT;
    using mapped_type     = ValueT;
    using value_type      = std::pair<const key_type, mapped_type>;
    using key_compare     = std::type_identity_t<CompareT>;
    using allocator_type  = std::type_identity_t<AllocatorT>;
    using reference       = value_type& ;
    using const_reference = const value_type&;

    static_assert(__check_valid_allocator<allocator_type>::value, "");
    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

    class value_compare {
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
    using AllocTraits_  = std::allocator_traits<allocator_type>;

    Tree_ tree_;

public:
    using pointer = AllocTraits_::pointer;
    using const_pointer = AllocTraits_::const_pointer;
    using size_type = AllocTraits_::size_type;
    using difference_type = AllocTraits_::difference_type;
    using iterator = MapIterator<typename Tree_::iterator>;
    using const_iterator = MapConstIterator<typename Tree_::const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    template <class Self>
    using SelfIterator = std::conditional_t<std::is_const_v<Self>, const_iterator, iterator>;
    template <class Self>
    using SelfSubrange = std::pair<SelfIterator<Self>, SelfIterator<Self>>;

    using node_type = MapNodeHandle<typename Tree_::node, allocator_type>;

    template <class /*Key*/, class /*Value*/, class /*Comp*/, class /*Alloc*/>
    friend class map;
    template <class /*Key*/, class /*Value*/, class /*Comp*/, class /*Alloc*/>
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
    
    multimap(const multimap& other) = default;

    multimap& operator=(const multimap& other) = default;

    multimap(multimap&& other) = default;

    multimap(multimap&& other, const allocator_type& alloc)
    : tree_(std::move(other.tree_), alloc) {}

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

    multimap(const multimap& other, const allocator_type& alloc)
    : tree_(other.tree_, alloc) {}

    ~multimap() {
        static_assert(sizeof(mstd::__diagnose_non_const_comparator<KeyT, CompareT>()), "");
    }

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
    [[nodiscard]] key_compare key_comp() const { return tree_.value_comp().key_comp(); }
    [[nodiscard]] value_compare value_comp() const {
        return value_compare(tree_.value_comp().key_comp());
    }

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

    template <class InputIteratorT>
    void insert(InputIteratorT begin, InputIteratorT end) {
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

    template <class Self, class TransparentKey>
    [[nodiscard]] SelfIterator<Self> find(this Self& self, const TransparentKey& key) {
        return self.tree_.find(key);
    }

    template <class TransparentKey>
    [[nodiscard]] size_type count(const TransparentKey& key) const {
        return tree_.countMulti(key);
    }

    template <class TransparentKey>
    [[nodiscard]] bool contains(const TransparentKey& key) const {
        return find(key) != end();
    }

    template <class Self, class TransparentKey>
    [[nodiscard]] SelfIterator<Self> lower_bound(this Self& self, const TransparentKey& key) {
        return self.tree_.lowerBoundMulti(key);
    }

    template <class Self, class TransparentKey>
    [[nodiscard]] iterator upper_bound(this Self& self, const TransparentKey& key) {
        return self.tree_.upperBoundMulti(key);
    }

    template <class Self, class TransparentKey>
    [[nodiscard]] SelfSubrange<Self> equal_range(this Self& self, const TransparentKey& key) {
        return self.tree_.equalRangeMulti(key);
    }

private:
    using node = Tree_::node;
    using node_allocator = Tree_::node_allocator;
    using __node_pointer = Tree_::node_pointer;

    using _Dp = MapNodeDestructor<node_allocator>;
    using __node_holder = std::unique_ptr<node, _Dp>;
};

template <
    class InputIteratorT
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<__iter_to_alloc_type<InputIteratorT>>
>
requires __has_input_iterator_category<InputIteratorT>
      && (!__is_allocator_v<CompareT>)
      && __is_allocator_v<AllocatorT>
multimap(InputIteratorT, InputIteratorT, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multimap<
       __iter_key_type<InputIteratorT>
     , __iter_mapped_type<InputIteratorT>
     , CompareT
     , AllocatorT
   >;

template <
    std::ranges::input_range RangeT
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<__range_to_alloc_type<RangeT>>
>
requires (!__is_allocator_v<CompareT>) && __is_allocator_v<AllocatorT>
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
  , class CompareT   = CompareThreeWay
  , class AllocatorT = std::allocator<std::pair<const KeyT, ValueT>>
>
requires (!__is_allocator_v<CompareT>) && __is_allocator_v<AllocatorT>
multimap(std::initializer_list<std::pair<KeyT, ValueT>>, CompareT = CompareT(), AllocatorT = AllocatorT())
-> multimap<std::remove_const_t<KeyT>, ValueT, CompareT, AllocatorT>;

template <class InputIteratorT, class AllocatorT>
requires __has_input_iterator_category<InputIteratorT> && __is_allocator_v<AllocatorT>
multimap(InputIteratorT, InputIteratorT, AllocatorT)
-> multimap<
       __iter_key_type<InputIteratorT>
     , __iter_mapped_type<InputIteratorT>
     , CompareThreeWay
     , AllocatorT
   >;

template <std::ranges::input_range RangeT, class AllocatorT>
requires __is_allocator_v<AllocatorT>
multimap(std::from_range_t, RangeT&&, AllocatorT)
-> multimap<
       __range_key_type<RangeT>
     , __range_mapped_type<RangeT>
     , CompareThreeWay
     , AllocatorT
   >;

template <class KeyT, class ValueT, class AllocatorT>
requires __is_allocator_v<AllocatorT>
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
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <class KeyT, class ValueT, class CompareT, class AllocatorT>
auto
operator<=>(const multimap<KeyT, ValueT, CompareT, AllocatorT>& x,
            const multimap<KeyT, ValueT, CompareT, AllocatorT>& y) {
    return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(), y.end(), CompareThreeWay{});
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
    template <class _KeyT, class ValueT, class _CompareT = CompareThreeWay>
    using map = mstd::map<_KeyT, ValueT, _CompareT, std::pmr::polymorphic_allocator<std::pair<const _KeyT, ValueT>>>;
    template <class _KeyT, class ValueT, class _CompareT = CompareThreeWay>
    using multimap = mstd::multimap<_KeyT, ValueT, _CompareT, std::pmr::polymorphic_allocator<std::pair<const _KeyT, ValueT>>>;
} // namespace pmr
} // namespace mstd


#endif // MSTD_MAP
