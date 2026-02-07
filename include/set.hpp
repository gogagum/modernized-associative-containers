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

/*
 *
 s et synopsis     *

 namespace std
 {

 template <class Key, class Compare = less<Key>,
class Allocator = allocator<Key>>
class set
{
public:
    // types:
    typedef Key                                      key_type;
    typedef key_type                                 value_type;
    typedef Compare                                  key_compare;
    typedef key_compare                              value_compare;
    typedef Allocator                                allocator_type;
    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;

    typedef implementation-defined                   iterator;
    typedef implementation-defined                   const_iterator;
    typedef std::reverse_iterator<iterator>          reverse_iterator;
    typedef std::reverse_iterator<const_iterator>    const_reverse_iterator;
    typedef unspecified                              node_type;               // C++17
    typedef INSERT_RETURN_TYPE<iterator, node_type>  insert_return_type;      // C++17

    // construct/copy/destroy:
    set()
    noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value &&
        std::is_nothrow_default_constructible<key_compare>::value &&
        std::is_nothrow_copy_constructible<key_compare>::value);
        explicit set(const value_compare& comp);
        set(const value_compare& comp, const allocator_type& a);
        template <class InputIterator>
        set(InputIterator first, InputIterator last,
        const value_compare& comp = value_compare());
        template <class InputIterator>
        set(InputIterator first, InputIterator last, const value_compare& comp,
        const allocator_type& a);
        template<container-compatible-range<value_type> R>
        set (std::from_range_t, R&& rg, const Compare& comp = Compare(), const Allocator& = Allocator()); // C++23
        set(const set& s);
        set(set&& s)
        noexcept(
            std::is_nothrow_move_constructible<allocator_type>::value &&
            std::is_nothrow_move_constructible<key_compare>::value);
            explicit set(const allocator_type& a);
            set(const set& s, const allocator_type& a);
            set(set&& s, const allocator_type& a);
            set(initializer_list<value_type> il, const value_compare& comp = value_compare());
            set(initializer_list<value_type> il, const value_compare& comp,
            const allocator_type& a);
            template <class InputIterator>
            set(InputIterator first, InputIterator last, const allocator_type& a)
            : set(first, last, Compare(), a) {}  // C++14
            template<container-compatible-range<value_type> R>
            set (std::from_range_t, R&& rg, const Allocator& a))
            : set (std::from_range, std::forward<R>(rg), Compare(), a) { } // C++23
            set(initializer_list<value_type> il, const allocator_type& a)
            : set(il, Compare(), a) {}  // C++14
            ~set();

            set& operator=(const set& s);
            set& operator=(set&& s)
            noexcept(
                allocator_type::propagate_on_container_move_assignment::value &&
                std::is_nothrow_move_assignable<allocator_type>::value &&
                std::is_nothrow_move_assignable<key_compare>::value);
                set& operator=(initializer_list<value_type> il);

                // iterators:
                iterator begin() noexcept;
                const_iterator begin() const noexcept;
                iterator end() noexcept;
                const_iterator end()   const noexcept;

                reverse_iterator rbegin() noexcept;
                const_reverse_iterator rbegin() const noexcept;
                reverse_iterator rend() noexcept;
                const_reverse_iterator rend()   const noexcept;

                const_iterator         cbegin()  const noexcept;
                const_iterator         cend()    const noexcept;
                const_reverse_iterator crbegin() const noexcept;
                const_reverse_iterator crend()   const noexcept;

                // capacity:
                bool      empty()    const noexcept;
                size_type size()     const noexcept;
                size_type max_size() const noexcept;

                // modifiers:
                template <class... Args>
                std::pair<iterator, bool> emplace(Args&&... args);
                template <class... Args>
                iterator emplace_hint(const_iterator position, Args&&... args);
                std::pair<iterator,bool> insert(const value_type& v);
                std::pair<iterator,bool> insert(value_type&& v);
                iterator insert(const_iterator position, const value_type& v);
                iterator insert(const_iterator position, value_type&& v);
                template <class InputIterator>
                void insert(InputIterator first, InputIterator last);
                template<container-compatible-range<value_type> R>
                void insert_range(R&& rg);                                                      // C++23
                void insert(initializer_list<value_type> il);

                node_type extract(const_iterator position);                                       // C++17
                node_type extract(const key_type& x);                                             // C++17
                insert_return_type insert(node_type&& nh);                                        // C++17
                iterator insert(const_iterator hint, node_type&& nh);                             // C++17

                iterator  erase(const_iterator position);
                iterator  erase(iterator position);  // C++14
                size_type erase(const key_type& k);
                iterator  erase(const_iterator first, const_iterator last);
                void clear() noexcept;

                template<class C2>
                void merge(set<Key, C2, Allocator>& source);         // C++17
                template<class C2>
                void merge(set<Key, C2, Allocator>&& source);        // C++17
                template<class C2>
                void merge(multiset<Key, C2, Allocator>& source);    // C++17
                template<class C2>
                void merge(multiset<Key, C2, Allocator>&& source);   // C++17

                void swap(set& s)
                noexcept(
                    __is_nothrow_swappable<key_compare>::value &&
                    (!allocator_type::propagate_on_container_swap::value ||
                    __is_nothrow_swappable<allocator_type>::value));

                    // observers:
                    allocator_type get_allocator() const noexcept;
                    key_compare    key_comp()      const;
                    value_compare  value_comp()    const;

                    // set operations:
                    iterator find(const key_type& k);
                    const_iterator find(const key_type& k) const;
                    template<typename K>
                    iterator find(const K& x);
                    template<typename K>
                    const_iterator find(const K& x) const;  // C++14

                    template<typename K>
                    size_type count(const K& x) const;        // C++14
                    size_type      count(const key_type& k) const;

                    bool           contains(const key_type& x) const;  // C++20
                    template<class K> bool contains(const K& x) const; // C++20

                    iterator lower_bound(const key_type& k);
                    const_iterator lower_bound(const key_type& k) const;
                    template<typename K>
                    iterator lower_bound(const K& x);              // C++14
                    template<typename K>
                    const_iterator lower_bound(const K& x) const;  // C++14

                    iterator upper_bound(const key_type& k);
                    const_iterator upper_bound(const key_type& k) const;
                    template<typename K>
                    iterator upper_bound(const K& x);              // C++14
                    template<typename K>
                    const_iterator upper_bound(const K& x) const;  // C++14
                    std::pair<iterator,iterator>             equal_range(const key_type& k);
                    std::pair<const_iterator,const_iterator> equal_range(const key_type& k) const;
                    template<typename K>
                    std::pair<iterator,iterator>             equal_range(const K& x);        // C++14
                    template<typename K>
                    std::pair<const_iterator,const_iterator> equal_range(const K& x) const;  // C++14
                    };

                    template <class InputIterator,
class Compare = less<typename iterator_traits<InputIterator>::value_type>,
class Allocator = allocator<typename iterator_traits<InputIterator>::value_type>>
set(InputIterator, InputIterator,
Compare = Compare(), Allocator = Allocator())
-> set<typename iterator_traits<InputIterator>::value_type, Compare, Allocator>; // C++17

template<ranges::input_range R, class Compare = less<ranges::range_value_t<R>>,
class Allocator = allocator<ranges::range_value_t<R>>>
set (std::from_range_t, R&&, Compare = Compare(), Allocator = Allocator())
-> set<ranges::range_value_t<R>, Compare, Allocator>; // C++23

template<class Key, class Compare = less<Key>, class Allocator = allocator<Key>>
set(initializer_list<Key>, Compare = Compare(), Allocator = Allocator())
-> set<Key, Compare, Allocator>; // C++17

template<class InputIterator, class Allocator>
set(InputIterator, InputIterator, Allocator)
-> set<typename iterator_traits<InputIterator>::value_type,
less<typename iterator_traits<InputIterator>::value_type>, Allocator>; // C++17

template<ranges::input_range R, class Allocator>
set (std::from_range_t, R&&, Allocator)
-> set<ranges::range_value_t<R>, less<ranges::range_value_t<R>>, Allocator>; // C++23

template<class Key, class Allocator>
set(initializer_list<Key>, Allocator) -> set<Key, less<Key>, Allocator>; // C++17

template <class Key, class Compare, class Allocator>
bool
operator==(const set<Key, Compare, Allocator>& x,
const set<Key, Compare, Allocator>& y);

template <class Key, class Compare, class Allocator>
bool
operator< (const set<Key, Compare, Allocator>& x,
const set<Key, Compare, Allocator>& y);                                // removed in C++20

template <class Key, class Compare, class Allocator>
bool
operator!=(const set<Key, Compare, Allocator>& x,
const set<Key, Compare, Allocator>& y);                                // removed in C++20

template <class Key, class Compare, class Allocator>
bool
operator> (const set<Key, Compare, Allocator>& x,
const set<Key, Compare, Allocator>& y);                                // removed in C++20

template <class Key, class Compare, class Allocator>
bool
operator>=(const set<Key, Compare, Allocator>& x,
const set<Key, Compare, Allocator>& y);                                // removed in C++20

template <class Key, class Compare, class Allocator>
bool
operator<=(const set<Key, Compare, Allocator>& x,
const set<Key, Compare, Allocator>& y);                                // removed in C++20

template<class Key, class Compare, class Allocator>
synth-three-way-result<Key> operator<=>(const set<Key, Compare, Allocator>& x,
const set<Key, Compare, Allocator>& y); // since C++20

// specialized algorithms:
template <class Key, class Compare, class Allocator>
void
swap(set<Key, Compare, Allocator>& x, set<Key, Compare, Allocator>& y)
noexcept(noexcept(x.swap(y)));

template <class Key, class Compare, class Allocator, class Predicate>
typename set<Key, Compare, Allocator>::size_type
erase_if(set<Key, Compare, Allocator>& c, Predicate pred);  // C++20

template <class Key, class Compare = less<Key>,
class Allocator = allocator<Key>>
class multiset
{
public:
    // types:
    typedef Key                                      key_type;
    typedef key_type                                 value_type;
    typedef Compare                                  key_compare;
    typedef key_compare                              value_compare;
    typedef Allocator                                allocator_type;
    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;

    typedef implementation-defined                   iterator;
    typedef implementation-defined                   const_iterator;
    typedef std::reverse_iterator<iterator>          reverse_iterator;
    typedef std::reverse_iterator<const_iterator>    const_reverse_iterator;
    typedef unspecified                              node_type;               // C++17

    // construct/copy/destroy:
    multiset()
    noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value &&
        std::is_nothrow_default_constructible<key_compare>::value &&
        std::is_nothrow_copy_constructible<key_compare>::value);
        explicit multiset(const value_compare& comp);
        multiset(const value_compare& comp, const allocator_type& a);
        template <class InputIterator>
        multiset(InputIterator first, InputIterator last,
        const value_compare& comp = value_compare());
        template <class InputIterator>
        multiset(InputIterator first, InputIterator last,
        const value_compare& comp, const allocator_type& a);
        template<container-compatible-range<value_type> R>
        multiset (std::from_range_t, R&& rg,
        const Compare& comp = Compare(), const Allocator& = Allocator()); // C++23
        multiset(const multiset& s);
        multiset(multiset&& s)
        noexcept(
            std::is_nothrow_move_constructible<allocator_type>::value &&
            std::is_nothrow_move_constructible<key_compare>::value);
            explicit multiset(const allocator_type& a);
            multiset(const multiset& s, const allocator_type& a);
            multiset(multiset&& s, const allocator_type& a);
            multiset(initializer_list<value_type> il, const value_compare& comp = value_compare());
            multiset(initializer_list<value_type> il, const value_compare& comp,
            const allocator_type& a);
            template <class InputIterator>
            multiset(InputIterator first, InputIterator last, const allocator_type& a)
            : set(first, last, Compare(), a) {}  // C++14
            template<container-compatible-range<value_type> R>
            multiset (std::from_range_t, R&& rg, const Allocator& a))
            : multiset (std::from_range, std::forward<R>(rg), Compare(), a) { } // C++23
            multiset(initializer_list<value_type> il, const allocator_type& a)
            : set(il, Compare(), a) {}  // C++14
            ~multiset();

            multiset& operator=(const multiset& s);
            multiset& operator=(multiset&& s)
            noexcept(
                allocator_type::propagate_on_container_move_assignment::value &&
                std::is_nothrow_move_assignable<allocator_type>::value &&
                std::is_nothrow_move_assignable<key_compare>::value);
                multiset& operator=(initializer_list<value_type> il);

                // iterators:
                iterator begin() noexcept;
                const_iterator begin() const noexcept;
                iterator end() noexcept;
                const_iterator end()   const noexcept;

                reverse_iterator rbegin() noexcept;
                const_reverse_iterator rbegin() const noexcept;
                reverse_iterator rend() noexcept;
                const_reverse_iterator rend()   const noexcept;

                const_iterator         cbegin()  const noexcept;
                const_iterator         cend()    const noexcept;
                const_reverse_iterator crbegin() const noexcept;
                const_reverse_iterator crend()   const noexcept;

                // capacity:
                bool      empty()    const noexcept;
                size_type size()     const noexcept;
                size_type max_size() const noexcept;

                // modifiers:
                template <class... Args>
                iterator emplace(Args&&... args);
                template <class... Args>
                iterator emplace_hint(const_iterator position, Args&&... args);
                iterator insert(const value_type& v);
                iterator insert(value_type&& v);
                iterator insert(const_iterator position, const value_type& v);
                iterator insert(const_iterator position, value_type&& v);
                template <class InputIterator>
                void insert(InputIterator first, InputIterator last);
                template<container-compatible-range<value_type> R>
                void insert_range(R&& rg);                                                      // C++23
                void insert(initializer_list<value_type> il);

                node_type extract(const_iterator position);                                       // C++17
                node_type extract(const key_type& x);                                             // C++17
                iterator insert(node_type&& nh);                                                  // C++17
                iterator insert(const_iterator hint, node_type&& nh);                             // C++17

                iterator  erase(const_iterator position);
                iterator  erase(iterator position);  // C++14
                size_type erase(const key_type& k);
                iterator  erase(const_iterator first, const_iterator last);
                void clear() noexcept;

                template<class C2>
                void merge(multiset<Key, C2, Allocator>& source);    // C++17
                template<class C2>
                void merge(multiset<Key, C2, Allocator>&& source);   // C++17
                template<class C2>
                void merge(set<Key, C2, Allocator>& source);         // C++17
                template<class C2>
                void merge(set<Key, C2, Allocator>&& source);        // C++17

                void swap(multiset& s)
                noexcept(
                    __is_nothrow_swappable<key_compare>::value &&
                    (!allocator_type::propagate_on_container_swap::value ||
                    __is_nothrow_swappable<allocator_type>::value));

                    // observers:
                    allocator_type get_allocator() const noexcept;
                    key_compare    key_comp()      const;
                    value_compare  value_comp()    const;

                    // set operations:
                    iterator find(const key_type& k);
                    const_iterator find(const key_type& k) const;
                    template<typename K>
                    iterator find(const K& x);
                    template<typename K>
                    const_iterator find(const K& x) const;  // C++14

                    template<typename K>
                    size_type count(const K& x) const;      // C++14
                    size_type      count(const key_type& k) const;

                    bool           contains(const key_type& x) const;  // C++20
                    template<class K> bool contains(const K& x) const; // C++20

                    iterator lower_bound(const key_type& k);
                    const_iterator lower_bound(const key_type& k) const;
                    template<typename K>
                    iterator lower_bound(const K& x);              // C++14
                    template<typename K>
                    const_iterator lower_bound(const K& x) const;  // C++14

                    iterator upper_bound(const key_type& k);
                    const_iterator upper_bound(const key_type& k) const;
                    template<typename K>
                    iterator upper_bound(const K& x);              // C++14
                    template<typename K>
                    const_iterator upper_bound(const K& x) const;  // C++14

                    std::pair<iterator,iterator>             equal_range(const key_type& k);
                    std::pair<const_iterator,const_iterator> equal_range(const key_type& k) const;
                    template<typename K>
                    std::pair<iterator,iterator>             equal_range(const K& x);        // C++14
                    template<typename K>
                    std::pair<const_iterator,const_iterator> equal_range(const K& x) const;  // C++14
                    };

                    template <class InputIterator,
class Compare = less<typename iterator_traits<InputIterator>::value_type>,
class Allocator = allocator<typename iterator_traits<InputIterator>::value_type>>
multiset(InputIterator, InputIterator,
Compare = Compare(), Allocator = Allocator())
-> multiset<typename iterator_traits<InputIterator>::value_type, Compare, Allocator>; // C++17

template<ranges::input_range R, class Compare = less<ranges::range_value_t<R>>,
class Allocator = allocator<ranges::range_value_t<R>>>
multiset (std::from_range_t, R&&, Compare = Compare(), Allocator = Allocator())
-> multiset<ranges::range_value_t<R>, Compare, Allocator>;

template<class Key, class Compare = less<Key>, class Allocator = allocator<Key>>
multiset(initializer_list<Key>, Compare = Compare(), Allocator = Allocator())
-> multiset<Key, Compare, Allocator>; // C++17

template<class InputIterator, class Allocator>
multiset(InputIterator, InputIterator, Allocator)
-> multiset<typename iterator_traits<InputIterator>::value_type,
less<typename iterator_traits<InputIterator>::value_type>, Allocator>; // C++17

template<ranges::input_range R, class Allocator>
multiset (std::from_range_t, R&&, Allocator)
-> multiset<ranges::range_value_t<R>, less<ranges::range_value_t<R>>, Allocator>;

template<class Key, class Allocator>
multiset(initializer_list<Key>, Allocator) -> multiset<Key, less<Key>, Allocator>; // C++17

template <class Key, class Compare, class Allocator>
bool
operator==(const multiset<Key, Compare, Allocator>& x,
const multiset<Key, Compare, Allocator>& y);

template <class Key, class Compare, class Allocator>
bool
operator< (const multiset<Key, Compare, Allocator>& x,
const multiset<Key, Compare, Allocator>& y);                                // removed in C++20

template <class Key, class Compare, class Allocator>
bool
operator!=(const multiset<Key, Compare, Allocator>& x,
const multiset<Key, Compare, Allocator>& y);                                // removed in C++20

template <class Key, class Compare, class Allocator>
bool
operator> (const multiset<Key, Compare, Allocator>& x,
const multiset<Key, Compare, Allocator>& y);                                // removed in C++20

template <class Key, class Compare, class Allocator>
bool
operator>=(const multiset<Key, Compare, Allocator>& x,
const multiset<Key, Compare, Allocator>& y);                                // removed in C++20

template <class Key, class Compare, class Allocator>
bool
operator<=(const multiset<Key, Compare, Allocator>& x,
const multiset<Key, Compare, Allocator>& y);                                // removed in C++20

template<class Key, class Compare, class Allocator>
synth-three-way-result<Key> operator<=>(const multiset<Key, Compare, Allocator>& x,
const multiset<Key, Compare, Allocator>& y); // since C++20

// specialized algorithms:
template <class Key, class Compare, class Allocator>
void
swap(multiset<Key, Compare, Allocator>& x, multiset<Key, Compare, Allocator>& y)
noexcept(noexcept(x.swap(y)));

template <class Key, class Compare, class Allocator, class Predicate>
typename multiset<Key, Compare, Allocator>::size_type
erase_if(multiset<Key, Compare, Allocator>& c, Predicate pred);  // C++20

}  // std

*/

#include <algorithm>
#include <detail/algorithm/specialized_algorithms.hpp>
#include <detail/functional/is_transparent.hpp>
#include <cassert>
#include <detail/config.hpp>
#include <functional>
#include <iterator>
#include <memory>
#include <detail/node_handle.hpp>
#include <detail/iterator/iterator_traits.hpp>
#include <detail/compare/synth_three_way.hpp>
#include <ranges>
#include <detail/tree.hpp>
#include <type_traits>
#include <detail/iterator/erase_if_container.hpp>
#include <detail/type_traits/is_allocator.hpp>
#include <detail/type_traits/container_traits.hpp>
#include <detail/ranges/container_compatible_range.hpp>
#include <memory_resource>
#  include <utility>
#  include <version>

// standard-mandated includes

// [associative.set.syn]
#  include <compare>
#  include <initializer_list>

namespace mstd {

template <class KeyT, class _Compare = std::less<KeyT>, class _Allocator = std::allocator<KeyT> >
class multiset;

template <class KeyT, class _Compare = std::less<KeyT>, class _Allocator = std::allocator<KeyT> >
class set {
public:
    // types:
    typedef KeyT key_type;
    typedef key_type value_type;
    typedef std::type_identity_t<_Compare> key_compare;
    typedef key_compare value_compare;
    typedef std::type_identity_t<_Allocator> allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    static_assert(std::is_same<typename allocator_type::value_type, value_type>::value,
                  "Allocator::value_type must be same type as value_type");

private:
    typedef Tree<value_type, value_compare, allocator_type> __base;
    typedef std::allocator_traits<allocator_type> __alloc_traits;

    static_assert(__check_valid_allocator<allocator_type>::value, "");

    __base tree_;

public:
    typedef typename __base::pointer pointer;
    typedef typename __base::const_pointer const_pointer;
    typedef typename __base::size_type size_type;
    typedef typename __base::difference_type difference_type;
    typedef typename __base::const_iterator iterator;
    typedef typename __base::const_iterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef __set_node_handle<typename __base::node, allocator_type> node_type;
    typedef __insert_return_type<iterator, node_type> insert_return_type;
    
    template <class _Key2, class _Compare2, class _Alloc2>
    friend class set;
    template <class _Key2, class _Compare2, class _Alloc2>
    friend class multiset;

    set() noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value && std::is_nothrow_default_constructible<key_compare>::value&&
        std::is_nothrow_copy_constructible<key_compare>::value)
    : tree_(value_compare()) {}

    explicit set(const value_compare& __comp) noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value && std::is_nothrow_copy_constructible<key_compare>::value)
    : tree_(__comp) {}

    explicit set(const value_compare& __comp, const allocator_type& __a) : tree_(__comp, __a) {}
    template <class InputIteratorT>
    set(InputIteratorT __f, InputIteratorT __l, const value_compare& __comp = value_compare())
    : tree_(__comp) {
        insert(__f, __l);
    }

    template <class InputIteratorT>
    set(InputIteratorT __f, InputIteratorT __l, const value_compare& __comp, const allocator_type& __a)
    : tree_(__comp, __a) {
        insert(__f, __l);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    set(std::from_range_t,
        RangeT&& __range,
        const key_compare& __comp = key_compare(),
        const allocator_type& __a = allocator_type())
    : tree_(__comp, __a) {
        insert_range(std::forward<RangeT>(__range));
    }
    
    template <class InputIteratorT>
    set(InputIteratorT __f, InputIteratorT __l, const allocator_type& __a)
    : set(__f, __l, key_compare(), __a) {}
    
    template <_ContainerCompatibleRange<value_type> RangeT>
    set(std::from_range_t, RangeT&& __range, const allocator_type& __a)
    : set (std::from_range, std::forward<RangeT>(__range), key_compare(), __a) {}
    
    set(const set& __s) = default;

    set& operator=(const set& __s) = default;

    set(set&& __s) = default;

    explicit set(const allocator_type& __a) : tree_(__a) {}

    set(const set& __s, const allocator_type& __alloc) : tree_(__s.tree_, __alloc) {}

    set(set&& __s, const allocator_type& __alloc) : tree_(std::move(__s.tree_), __alloc) {}

    set(std::initializer_list<value_type> __il, const value_compare& __comp = value_compare())
    : tree_(__comp) {
        insert(__il.begin(), __il.end());
    }

    set(std::initializer_list<value_type> __il, const value_compare& __comp, const allocator_type& __a)
    : tree_(__comp, __a) {
        insert(__il.begin(), __il.end());
    }

    set(std::initializer_list<value_type> __il, const allocator_type& __a)
    : set(__il, key_compare(), __a) {}

    set& operator=(std::initializer_list<value_type> __il) {
        clear();
        insert(__il.begin(), __il.end());
        return *this;
    }

    set& operator=(set&& __s) = default;

    ~set() { static_assert(sizeof(mstd::__diagnose_non_const_comparator<KeyT, _Compare>()), ""); }

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

    // modifiers:
    template <class... _Args>
    std::pair<iterator, bool> emplace(_Args&&... __args) {
        return tree_.emplaceUnique(std::forward<_Args>(__args)...);
    }
    template <class... _Args>
    iterator emplace_hint(const_iterator __p, _Args&&... __args) {
        return tree_.emplaceHintUnique(__p, std::forward<_Args>(__args)...).first;
    }

    std::pair<iterator, bool> insert(const value_type& __v) { return tree_.emplaceUnique(__v); }
    iterator insert(const_iterator __p, const value_type& __v) {
        return tree_.emplaceHintUnique(__p, __v).first;
    }

    template <class InputIteratorT>
    void insert(InputIteratorT __first, InputIteratorT __last) {
        tree_.insertRangeUnique(__first, __last);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    void insert_range(RangeT&& __range) {
        tree_.insertRangeUnique(std::ranges::begin(__range), std::ranges::end(__range));
    }
    
    std::pair<iterator, bool> insert(value_type&& __v) {
        return tree_.emplaceUnique(std::move(__v));
    }

    iterator insert(const_iterator __p, value_type&& __v) {
        return tree_.emplaceHintUnique(__p, std::move(__v)).first;
    }

    void insert(std::initializer_list<value_type> __il) { insert(__il.begin(), __il.end()); }
    
    iterator erase(const_iterator __p) { return tree_.erase(__p); }
    size_type erase(const key_type& __k) { return tree_.eraseUnique(__k); }
    iterator erase(const_iterator __f, const_iterator __l) { return tree_.erase(__f, __l); }
    void clear() noexcept { tree_.clear(); }

    insert_return_type insert(node_type&& __nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(__nh.empty() || __nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to set::insert()");
        return tree_.template nodeHandleInsertUnique< node_type, insert_return_type>(std::move(__nh));
    }
    iterator insert(const_iterator __hint, node_type&& __nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(__nh.empty() || __nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to set::insert()");
        return tree_.template nodeHandleInsertUnique<node_type>(__hint, std::move(__nh));
    }
    [[nodiscard]] node_type extract(key_type const& __key) {
        return tree_.template nodeHandleExtract<node_type>(__key);
    }
    [[nodiscard]] node_type extract(const_iterator __it) {
        return tree_.template nodeHandleExtract<node_type>(__it);
    }
    template <class _Compare2>
    void merge(set<key_type, _Compare2, allocator_type>& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeUnique(__source.tree_);
    }
    template <class _Compare2>
    void merge(set<key_type, _Compare2, allocator_type>&& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeUnique(__source.tree_);
    }
    template <class _Compare2>
    void merge(multiset<key_type, _Compare2, allocator_type>& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeUnique(__source.tree_);
    }
    template <class _Compare2>
    void merge(multiset<key_type, _Compare2, allocator_type>&& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeUnique(__source.tree_);
    }
    
    void swap(set& __s) noexcept(std::is_nothrow_swappable_v<__base>) { tree_.swap(__s.tree_); }

    [[nodiscard]] allocator_type get_allocator() const noexcept { return tree_.alloc(); }
    [[nodiscard]] key_compare key_comp() const { return tree_.value_comp(); }
    [[nodiscard]] value_compare value_comp() const { return tree_.value_comp(); }

    // set operations:
    [[nodiscard]] iterator find(const key_type& __k) { return tree_.find(__k); }
    [[nodiscard]] const_iterator find(const key_type& __k) const { return tree_.find(__k); }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] iterator find(const _K2& __k) {
        return tree_.find(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] const_iterator find(const _K2& __k) const {
        return tree_.find(__k);
    }
    
    [[nodiscard]] size_type count(const key_type& __k) const {
        return tree_.countUnique(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] size_type count(const _K2& __k) const {
        return tree_.countMulti(__k);
    }

    [[nodiscard]] bool contains(const key_type& __k) const { return find(__k) != end(); }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] bool contains(const _K2& __k) const {
        return find(__k) != end();
    }

    [[nodiscard]] iterator lower_bound(const key_type& __k) {
        return tree_.lowerBoundUnique(__k);
    }

    [[nodiscard]] const_iterator lower_bound(const key_type& __k) const {
        return tree_.lowerBoundUnique(__k);
    }

    // The transparent versions of the lookup functions use the _multi version, since a non-element key is allowed to
    // match multiple elements.
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] iterator lower_bound(const _K2& __k) {
        return tree_.lowerBoundMulti(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] const_iterator lower_bound(const _K2& __k) const {
        return tree_.lowerBoundMulti(__k);
    }
    
    [[nodiscard]] iterator upper_bound(const key_type& __k) {
        return tree_.upperBoundUnique(__k);
    }

    [[nodiscard]] const_iterator upper_bound(const key_type& __k) const {
        return tree_.upperBoundUnique(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] iterator upper_bound(const _K2& __k) {
        return tree_.upperBoundMulti(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] const_iterator upper_bound(const _K2& __k) const {
        return tree_.upperBoundMulti(__k);
    }
    
    [[nodiscard]] std::pair<iterator, iterator> equal_range(const key_type& __k) {
        return tree_.equalRangeUnique(__k);
    }
    [[nodiscard]] std::pair<const_iterator, const_iterator> equal_range(const key_type& __k) const {
        return tree_.equalRangeUnique(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] std::pair<iterator, iterator> equal_range(const _K2& __k) {
        return tree_.equalRangeMulti(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] std::pair<const_iterator, const_iterator> equal_range(const _K2& __k) const {
        return tree_.equalRangeMulti(__k);
    }
    
    template <class, class...>
    friend struct __specialized_algorithm;
};

template <class InputIteratorT,
class _Compare   = std::less<__iterator_value_type<InputIteratorT>>,
class _Allocator = std::allocator<__iterator_value_type<InputIteratorT>>,
class            = std::enable_if_t<__has_input_iterator_category<InputIteratorT>::value, void>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>>
set(InputIteratorT, InputIteratorT, _Compare = _Compare(), _Allocator = _Allocator())
-> set<__iterator_value_type<InputIteratorT>, _Compare, _Allocator>;

template <std::ranges::input_range RangeT,
class _Compare   = std::less<std::ranges::range_value_t<RangeT>>,
class _Allocator = std::allocator<std::ranges::range_value_t<RangeT>>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>>
set(std::from_range_t, RangeT&&, _Compare = _Compare(), _Allocator = _Allocator())
-> set<std::ranges::range_value_t<RangeT>, _Compare, _Allocator>;

template <class KeyT,
class _Compare   = std::less<KeyT>,
class _Allocator = std::allocator<KeyT>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>>
set(std::initializer_list<KeyT>, _Compare = _Compare(), _Allocator = _Allocator()) -> set<KeyT, _Compare, _Allocator>;

template <class InputIteratorT,
class _Allocator,
class = std::enable_if_t<__has_input_iterator_category<InputIteratorT>::value>,
class = std::enable_if_t<__is_allocator_v<_Allocator>>>
set(InputIteratorT, InputIteratorT, _Allocator)
-> set<__iterator_value_type<InputIteratorT>, std::less<__iterator_value_type<InputIteratorT>>, _Allocator>;

template <std::ranges::input_range RangeT, class _Allocator, class = std::enable_if_t<__is_allocator_v<_Allocator>>>
set(std::from_range_t, RangeT&&, _Allocator)
-> set<std::ranges::range_value_t<RangeT>, std::less<std::ranges::range_value_t<RangeT>>, _Allocator>;

template <class KeyT, class _Allocator, class = std::enable_if_t<__is_allocator_v<_Allocator>>>
set(std::initializer_list<KeyT>, _Allocator) -> set<KeyT, std::less<KeyT>, _Allocator>;

template <class _Alg, class KeyT, class _Compare, class _Allocator>
struct __specialized_algorithm<_Alg, __single_range<set<KeyT, _Compare, _Allocator>>> {
    using __set = set<KeyT, _Compare, _Allocator>;

    static const bool __has_algorithm =
    __specialized_algorithm<_Alg, __single_range<typename __set::__base>>::__has_algorithm;

    // set's begin() and end() are identical with and without const qualification
    template <class... _Args>
    static auto operator()(const __set& __set, _Args&&... __args) {
        return __specialized_algorithm<_Alg, __single_range<typename __set::__base>>()(
            __set.tree_, std::forward<_Args>(__args)...);
    }
};

template <class KeyT, class _Compare, class _Allocator>
inline bool
operator==(const set<KeyT, _Compare, _Allocator>& __x, const set<KeyT, _Compare, _Allocator>& __y) {
    return __x.size() == __y.size() && std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class KeyT, class _Compare, class _Allocator>
__synth_three_way_result<KeyT>
operator<=>(const set<KeyT, _Compare, _Allocator>& __x, const set<KeyT, _Compare, _Allocator>& __y) {
    return std::lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), mstd::__synth_three_way);
}

// specialized algorithms:
template <class KeyT, class _Compare, class _Allocator>
inline void swap(set<KeyT, _Compare, _Allocator>& __x, set<KeyT, _Compare, _Allocator>& __y)
noexcept(noexcept(__x.swap(__y))) {
    __x.swap(__y);
}

template <class KeyT, class _Compare, class _Allocator, class _Predicate>
inline typename set<KeyT, _Compare, _Allocator>::size_type
erase_if(set<KeyT, _Compare, _Allocator>& __c, _Predicate __pred) {
    return mstd::_MSTD_erase_if_container(__c, __pred);
}

template <class KeyT, class _Compare, class _Allocator>
struct __container_traits<set<KeyT, _Compare, _Allocator> > {
    // http://eel.is/c++draft/associative.reqmts.except#2
    // For associative containers, if an exception is thrown by any operation from within
    // an insert or emplace function inserting a single element, the insertion has no effect.
    static constexpr bool __emplacement_has_strong_exception_safety_guarantee = true;

    static constexpr bool __reservable = false;
};

template <class KeyT, class _Compare, class _Allocator>
class multiset {
public:
    // types:
    typedef KeyT key_type;
    typedef key_type value_type;
    typedef std::type_identity_t<_Compare> key_compare;
    typedef key_compare value_compare;
    typedef std::type_identity_t<_Allocator> allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

private:
    typedef Tree<value_type, value_compare, allocator_type> __base;
    typedef allocator_traits<allocator_type> __alloc_traits;

    static_assert(__check_valid_allocator<allocator_type>::value, "");

    __base tree_;

public:
    typedef typename __base::pointer pointer;
    typedef typename __base::const_pointer const_pointer;
    typedef typename __base::size_type size_type;
    typedef typename __base::difference_type difference_type;
    typedef typename __base::const_iterator iterator;
    typedef typename __base::const_iterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef __set_node_handle<typename __base::node, allocator_type> node_type;

    template <class _Key2, class _Compare2, class _Alloc2>
    friend class set;
    template <class _Key2, class _Compare2, class _Alloc2>
    friend class multiset;

    // construct/copy/destroy:
    multiset() noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value && std::is_nothrow_default_constructible<key_compare>::value&&
        std::is_nothrow_copy_constructible<key_compare>::value)
    : tree_(value_compare()) {}

    explicit multiset(const value_compare& __comp) noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value && std::is_nothrow_copy_constructible<key_compare>::value)
    : tree_(__comp) {}

    explicit multiset(const value_compare& __comp, const allocator_type& __a)
    : tree_(__comp, __a) {}
    template <class InputIteratorT>
    multiset(InputIteratorT __f, InputIteratorT __l, const value_compare& __comp = value_compare())
    : tree_(__comp) {
        insert(__f, __l);
    }

    template <class InputIteratorT>
    multiset(InputIteratorT __f, InputIteratorT __l, const allocator_type& __a)
    : multiset(__f, __l, key_compare(), __a) {}

    template <class InputIteratorT>
    multiset(InputIteratorT __f, InputIteratorT __l, const value_compare& __comp, const allocator_type& __a)
    : tree_(__comp, __a) {
        insert(__f, __l);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    multiset(std::from_range_t,
             RangeT&& __range,
             const key_compare& __comp = key_compare(),
             const allocator_type& __a = allocator_type())
    : tree_(__comp, __a) {
        insert_range(std::forward<RangeT>(__range));
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    multiset(std::from_range_t, RangeT&& __range, const allocator_type& __a)
    : multiset (std::from_range, std::forward<RangeT>(__range), key_compare(), __a) {}
    
    multiset(const multiset& __s) = default;

    multiset& operator=(const multiset& __s) = default;

    multiset(multiset&& __s) = default;

    multiset(multiset&& __s, const allocator_type& __a) : tree_(std::move(__s.tree_), __a) {}
    explicit multiset(const allocator_type& __a) : tree_(__a) {}
    multiset(const multiset& __s, const allocator_type& __a) : tree_(__s.tree_, __a) {}

    multiset(std::initializer_list<value_type> __il, const value_compare& __comp = value_compare())
    : tree_(__comp) {
        insert(__il.begin(), __il.end());
    }

    multiset(std::initializer_list<value_type> __il, const value_compare& __comp, const allocator_type& __a)
    : tree_(__comp, __a) {
        insert(__il.begin(), __il.end());
    }

    multiset(std::initializer_list<value_type> __il, const allocator_type& __a)
    : multiset(__il, key_compare(), __a) {}

    multiset& operator=(std::initializer_list<value_type> __il) {
        clear();
        insert(__il.begin(), __il.end());
        return *this;
    }

    multiset& operator=(multiset&& __s) = default;

    ~multiset() {
        static_assert(sizeof(mstd::__diagnose_non_const_comparator<KeyT, _Compare>()), "");
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

    // modifiers:
    template <class... _Args>
    iterator emplace(_Args&&... __args) {
        return tree_.emplaceMulti(std::forward<_Args>(__args)...);
    }
    template <class... _Args>
    iterator emplace_hint(const_iterator __p, _Args&&... __args) {
        return tree_.emplaceHintMulti(__p, std::forward<_Args>(__args)...);
    }

    iterator insert(const value_type& __v) { return tree_.emplaceMulti(__v); }
    iterator insert(const_iterator __p, const value_type& __v) {
        return tree_.emplaceHintMulti(__p, __v);
    }

    template <class InputIteratorT>
    void insert(InputIteratorT __first, InputIteratorT __last) {
        tree_.insertRangeMulti(__first, __last);
    }

    template <_ContainerCompatibleRange<value_type> RangeT>
    void insert_range(RangeT&& __range) {
        tree_.insertRangeMulti(std::ranges::begin(__range), std::ranges::end(__range));
    }
    
    iterator insert(value_type&& __v) { return tree_.emplaceMulti(std::move(__v)); }

    iterator insert(const_iterator __p, value_type&& __v) {
        return tree_.emplaceHintMulti(__p, std::move(__v));
    }

    void insert(std::initializer_list<value_type> __il) { insert(__il.begin(), __il.end()); }

    iterator erase(const_iterator __p) { return tree_.erase(__p); }
    size_type erase(const key_type& __k) { return tree_.eraseMulti(__k); }
    iterator erase(const_iterator __f, const_iterator __l) { return tree_.erase(__f, __l); }
    void clear() noexcept { tree_.clear(); }

    iterator insert(node_type&& __nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(__nh.empty() || __nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to multiset::insert()");
        return tree_.template nodeHandleInsertMulti<node_type>(std::move(__nh));
    }
    iterator insert(const_iterator __hint, node_type&& __nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(__nh.empty() || __nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to multiset::insert()");
        return tree_.template nodeHandleInsertMulti<node_type>(__hint, std::move(__nh));
    }
    [[nodiscard]] node_type extract(key_type const& __key) {
        return tree_.template nodeHandleExtract<node_type>(__key);
    }
    [[nodiscard]] node_type extract(const_iterator __it) {
        return tree_.template nodeHandleExtract<node_type>(__it);
    }
    template <class _Compare2>
    void merge(multiset<key_type, _Compare2, allocator_type>& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeMulti(__source.tree_);
    }
    template <class _Compare2>
    void merge(multiset<key_type, _Compare2, allocator_type>&& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeMulti(__source.tree_);
    }
    template <class _Compare2>
    void merge(set<key_type, _Compare2, allocator_type>& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeMulti(__source.tree_);
    }
    template <class _Compare2>
    void merge(set<key_type, _Compare2, allocator_type>&& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        tree_.nodeHandleMergeMulti(__source.tree_);
    }
    
    void swap(multiset& __s) noexcept(std::is_nothrow_swappable_v<__base>) {
        tree_.swap(__s.tree_);
    }

    [[nodiscard]] allocator_type get_allocator() const noexcept { return tree_.alloc(); }
    [[nodiscard]] key_compare key_comp() const { return tree_.value_comp(); }
    [[nodiscard]] value_compare value_comp() const { return tree_.value_comp(); }

    // set operations:
    [[nodiscard]] iterator find(const key_type& __k) { return tree_.find(__k); }
    [[nodiscard]] const_iterator find(const key_type& __k) const { return tree_.find(__k); }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] iterator find(const _K2& __k) {
        return tree_.find(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] const_iterator find(const _K2& __k) const {
        return tree_.find(__k);
    }
    
    [[nodiscard]] size_type count(const key_type& __k) const {
        return tree_.countMulti(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] size_type count(const _K2& __k) const {
        return tree_.countMulti(__k);
    }
    
    [[nodiscard]] bool contains(const key_type& __k) const { return find(__k) != end(); }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] bool contains(const _K2& __k) const {
        return find(__k) != end();
    }

    [[nodiscard]] iterator lower_bound(const key_type& __k) {
        return tree_.lowerBoundMulti(__k);
    }

    [[nodiscard]] const_iterator lower_bound(const key_type& __k) const {
        return tree_.lowerBoundMulti(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] iterator lower_bound(const _K2& __k) {
        return tree_.lowerBoundMulti(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] const_iterator lower_bound(const _K2& __k) const {
        return tree_.lowerBoundMulti(__k);
    }
    
    [[nodiscard]] iterator upper_bound(const key_type& __k) {
        return tree_.upperBoundMulti(__k);
    }

    [[nodiscard]] const_iterator upper_bound(const key_type& __k) const {
        return tree_.upperBoundMulti(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] iterator upper_bound(const _K2& __k) {
        return tree_.upperBoundMulti(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] const_iterator upper_bound(const _K2& __k) const {
        return tree_.upperBoundMulti(__k);
    }
    
    [[nodiscard]] std::pair<iterator, iterator> equal_range(const key_type& __k) {
        return tree_.equalRangeMulti(__k);
    }
    [[nodiscard]] std::pair<const_iterator, const_iterator> equal_range(const key_type& __k) const {
        return tree_.equalRangeMulti(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] std::pair<iterator, iterator> equal_range(const _K2& __k) {
        return tree_.equalRangeMulti(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] std::pair<const_iterator, const_iterator> equal_range(const _K2& __k) const {
        return tree_.equalRangeMulti(__k);
    }
    
    template <class, class...>
    friend struct __specialized_algorithm;
};

template <class InputIteratorT,
class _Compare   = std::less<__iterator_value_type<InputIteratorT>>,
class _Allocator = std::allocator<__iterator_value_type<InputIteratorT>>,
class            = std::enable_if_t<__has_input_iterator_category<InputIteratorT>::value, void>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>>
multiset(InputIteratorT, InputIteratorT, _Compare = _Compare(), _Allocator = _Allocator())
-> multiset<__iterator_value_type<InputIteratorT>, _Compare, _Allocator>;

template <std::ranges::input_range RangeT,
class _Compare   = std::less<std::ranges::range_value_t<RangeT>>,
class _Allocator = std::allocator<std::ranges::range_value_t<RangeT>>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>>
multiset(std::from_range_t, RangeT&&, _Compare = _Compare(), _Allocator = _Allocator())
-> multiset<std::ranges::range_value_t<RangeT>, _Compare, _Allocator>;

template <class KeyT,
class _Compare   = std::less<KeyT>,
class _Allocator = std::allocator<KeyT>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>>
multiset(std::initializer_list<KeyT>, _Compare = _Compare(), _Allocator = _Allocator())
-> multiset<KeyT, _Compare, _Allocator>;

template <class InputIteratorT,
class _Allocator,
class = std::enable_if_t<__has_input_iterator_category<InputIteratorT>::value, void>,
class = std::enable_if_t<__is_allocator_v<_Allocator>>>
multiset(InputIteratorT, InputIteratorT, _Allocator)
-> multiset<__iterator_value_type<InputIteratorT>, std::less<__iterator_value_type<InputIteratorT>>, _Allocator>;

template <std::ranges::input_range RangeT, class _Allocator, class = std::enable_if_t<__is_allocator_v<_Allocator>>>
multiset(std::from_range_t, RangeT&&, _Allocator)
-> multiset<std::ranges::range_value_t<RangeT>, std::less<std::ranges::range_value_t<RangeT>>, _Allocator>;

template <class KeyT, class _Allocator, class = std::enable_if_t<__is_allocator_v<_Allocator>>>
multiset(std::initializer_list<KeyT>, _Allocator) -> multiset<KeyT, std::less<KeyT>, _Allocator>;

template <class _Alg, class KeyT, class _Compare, class _Allocator>
struct __specialized_algorithm<_Alg, __single_range<multiset<KeyT, _Compare, _Allocator>>> {
    using __set = multiset<KeyT, _Compare, _Allocator>;

    static const bool __has_algorithm =
    __specialized_algorithm<_Alg, __single_range<typename __set::__base>>::__has_algorithm;

    // set's begin() and end() are identical with and without const qualification
    template <class... _Args>
    static auto operator()(const __set& __set, _Args&&... __args) {
        return __specialized_algorithm<_Alg, __single_range<typename __set::__base>>()(
            __set.tree_, std::forward<_Args>(__args)...);
    }
};

template <class KeyT, class _Compare, class _Allocator>
inline bool
operator==(const multiset<KeyT, _Compare, _Allocator>& __x, const multiset<KeyT, _Compare, _Allocator>& __y) {
    return __x.size() == __y.size() && std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class KeyT, class _Compare, class _Allocator>
__synth_three_way_result<KeyT>
operator<=>(const multiset<KeyT, _Compare, _Allocator>& __x, const multiset<KeyT, _Compare, _Allocator>& __y) {
    return std::lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), __synth_three_way);
}

template <class KeyT, class _Compare, class _Allocator>
inline void
swap(multiset<KeyT, _Compare, _Allocator>& __x, multiset<KeyT, _Compare, _Allocator>& __y)
noexcept(noexcept(__x.swap(__y))) {
    __x.swap(__y);
}

template <class KeyT, class _Compare, class _Allocator, class _Predicate>
inline typename multiset<KeyT, _Compare, _Allocator>::size_type
erase_if(multiset<KeyT, _Compare, _Allocator>& __c, _Predicate __pred) {
    return mstd::_MSTD_erase_if_container(__c, __pred);
}

template <class KeyT, class _Compare, class _Allocator>
struct __container_traits<multiset<KeyT, _Compare, _Allocator> > {
    // http://eel.is/c++draft/associative.reqmts.except#2
    // For associative containers, if an exception is thrown by any operation from within
    // an insert or emplace function inserting a single element, the insertion has no effect.
    static constexpr const bool __emplacement_has_strong_exception_safety_guarantee = true;

    static constexpr const bool __reservable = false;
};

} // namespace mstd

namespace mstd {
namespace pmr {
    template <class _KeyT, class _CompareT = std::less<_KeyT>>
    using set = mstd::set<_KeyT, _CompareT, std::pmr::polymorphic_allocator<_KeyT>>;

    template <class _KeyT, class _CompareT = std::less<_KeyT>>
    using multiset = mstd::multiset<_KeyT, _CompareT, std::pmr::polymorphic_allocator<_KeyT>>;
} // namespace pmr
} // namespace mstd


#endif // MSTD_SET
