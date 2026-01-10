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

/*
 *
 m ap synopsis     *

 namespace std
 {

 template <class Key, class T, class Compare = less<Key>,
class Allocator = allocator<pair<const Key, T>>>
class map
{
public:
    // types:
    typedef Key                                      key_type;
    typedef T                                        mapped_type;
    typedef std::pair<const key_type, mapped_type>        value_type;
    typedef Compare                                  key_compare;
    typedef Allocator                                allocator_type;
    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;

    typedef implementation-defined                   iterator;
    typedef implementation-defined                   const_iterator;
    typedef std::reverse_iterator<iterator>          reverse_iterator;
    typedef std::reverse_iterator<const_iterator>    const_reverse_iterator;
    typedef unspecified                              node_type;              // C++17
    typedef INSERT_RETURN_TYPE<iterator, node_type>  insert_return_type;     // C++17

    class value_compare
    {
    friend class map;
    protected:
        key_compare comp;

        value_compare(key_compare c);
        public:
            typedef bool result_type;  // deprecated in C++17, removed in C++20
            typedef value_type first_argument_type;  // deprecated in C++17, removed in C++20
            typedef value_type second_argument_type;  // deprecated in C++17, removed in C++20
            bool operator()(const value_type& x, const value_type& y) const;
            };

            // construct/copy/destroy:
            map()
            noexcept(
                is_nothrow_default_constructible<allocator_type>::value &&
                is_nothrow_default_constructible<key_compare>::value &&
                is_nothrow_copy_constructible<key_compare>::value);
                explicit map(const key_compare& comp);
                map(const key_compare& comp, const allocator_type& a);
                template <class InputIterator>
                map(InputIterator first, InputIterator last,
                const key_compare& comp = key_compare());
                template <class InputIterator>
                map(InputIterator first, InputIterator last,
                const key_compare& comp, const allocator_type& a);
                template<container-compatible-range<value_type> R>
                map(std::from_range_t, R&& rg, const Compare& comp = Compare(), const Allocator& = Allocator()); // C++23
                map(const map& m);
                map(map&& m)
                noexcept(
                    is_nothrow_move_constructible<allocator_type>::value &&
                    is_nothrow_move_constructible<key_compare>::value);
                    explicit map(const allocator_type& a);
                    map(const map& m, const allocator_type& a);
                    map(map&& m, const allocator_type& a);
                    map(std::initializer_list<value_type> il, const key_compare& comp = key_compare());
                    map(std::initializer_list<value_type> il, const key_compare& comp, const allocator_type& a);
                    template <class InputIterator>
                    map(InputIterator first, InputIterator last, const allocator_type& a)
                    : map(first, last, Compare(), a) {}  // C++14
                    template<container-compatible-range<value_type> R>
                    map(std::from_range_t, R&& rg, const Allocator& a))
                    : map(from_range, std::forward<R>(rg), Compare(), a) { } // C++23
                    map(std::initializer_list<value_type> il, const allocator_type& a)
                    : map(il, Compare(), a) {}  // C++14
                    ~map();

                    map& operator=(const map& m);
                    map& operator=(map&& m)
                    noexcept(
                        allocator_type::propagate_on_container_move_assignment::value &&
                        is_nothrow_move_assignable<allocator_type>::value &&
                        is_nothrow_move_assignable<key_compare>::value);
                        map& operator=(std::initializer_list<value_type> il);

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

                        // element access:
                        mapped_type& operator[](const key_type& k);
                        mapped_type& operator[](key_type&& k);

                        mapped_type& at(const key_type& k);
                        const mapped_type& at(const key_type& k) const;

                        // modifiers:
                        template <class... Args>
                        std::pair<iterator, bool> emplace(Args&&... args);
                        template <class... Args>
                        iterator emplace_hint(const_iterator position, Args&&... args);
                        std::pair<iterator, bool> insert(const value_type& v);
                        std::pair<iterator, bool> insert(      value_type&& v);                                // C++17
                        template <class P>
                        std::pair<iterator, bool> insert(P&& p);
                        iterator insert(const_iterator position, const value_type& v);
                        iterator insert(const_iterator position,       value_type&& v);                   // C++17
                        template <class P>
                        iterator insert(const_iterator position, P&& p);
                        template <class InputIterator>
                        void insert(InputIterator first, InputIterator last);
                        template<container-compatible-range<value_type> R>
                        void insert_range(R&& rg);                                                      // C++23
                        void insert(std::initializer_list<value_type> il);

                        node_type extract(const_iterator position);                                       // C++17
                        node_type extract(const key_type& x);                                             // C++17
                        insert_return_type insert(node_type&& nh);                                        // C++17
                        iterator insert(const_iterator hint, node_type&& nh);                             // C++17

                        template <class... Args>
                        std::pair<iterator, bool> try_emplace(const key_type& k, Args&&... args);          // C++17
                        template <class... Args>
                        std::pair<iterator, bool> try_emplace(key_type&& k, Args&&... args);               // C++17
                        template <class... Args>
                        iterator try_emplace(const_iterator hint, const key_type& k, Args&&... args); // C++17
                        template <class... Args>
                        iterator try_emplace(const_iterator hint, key_type&& k, Args&&... args);      // C++17
                        template <class M>
                        std::pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj);            // C++17
                        template <class M>
                        std::pair<iterator, bool> insert_or_assign(key_type&& k, M&& obj);                 // C++17
                        template <class M>
                        iterator insert_or_assign(const_iterator hint, const key_type& k, M&& obj);   // C++17
                        template <class M>
                        iterator insert_or_assign(const_iterator hint, key_type&& k, M&& obj);        // C++17

                        iterator  erase(const_iterator position);
                        iterator  erase(iterator position); // C++14
                        size_type erase(const key_type& k);
                        iterator  erase(const_iterator first, const_iterator last);
                        void clear() noexcept;

                        template<class C2>
                        void merge(map<Key, T, C2, Allocator>& source);         // C++17
                        template<class C2>
                        void merge(map<Key, T, C2, Allocator>&& source);        // C++17
                        template<class C2>
                        void merge(multimap<Key, T, C2, Allocator>& source);    // C++17
                        template<class C2>
                        void merge(multimap<Key, T, C2, Allocator>&& source);   // C++17

                        void swap(map& m)
                        noexcept(allocator_traits<allocator_type>::is_always_equal::value &&
                        is_nothrow_swappable<key_compare>::value); // C++17

                        // observers:
                        allocator_type get_allocator() const noexcept;
                        key_compare    key_comp()      const;
                        value_compare  value_comp()    const;

                        // map operations:
                        iterator find(const key_type& k);
                        const_iterator find(const key_type& k) const;
                        template<typename K>
                        iterator find(const K& x);              // C++14
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
class Compare = less<iter_key_t<InputIterator>>,
class Allocator = allocator<iter_to_alloc_t<InputIterator>>>
map(InputIterator, InputIterator, Compare = Compare(), Allocator = Allocator())
-> map<iter_key_t<InputIterator>, iter_val_t<InputIterator>, Compare, Allocator>; // C++17

template< std::ranges::input_range R, class Compare = less<range-key-type<R>,
class Allocator = allocator<range-to-alloc-type<R>>>
map(std::from_range_t, R&&, Compare = Compare(), Allocator = Allocator())
-> map<range-key-type<R>, range-mapped-type<R>, Compare, Allocator>; // C++23

template<class Key, class T, class Compare = less<Key>,
class Allocator = allocator<pair<const Key, T>>>
map(std::initializer_list<pair<const Key, T>>, Compare = Compare(), Allocator = Allocator())
-> map<Key, T, Compare, Allocator>; // C++17

template <class InputIterator, class Allocator>
map(InputIterator, InputIterator, Allocator)
-> map<iter_key_t<InputIterator>, iter_val_t<InputIterator>, less<iter_key_t<InputIterator>>,
Allocator>; // C++17

template< std::ranges::input_range R, class Allocator>
map(std::from_range_t, R&&, Allocator)
-> map<range-key-type<R>, range-mapped-type<R>, less<range-key-type<R>>, Allocator>; // C++23

template<class Key, class T, class Allocator>
map(std::initializer_list<pair<const Key, T>>, Allocator) -> map<Key, T, less<Key>, Allocator>; // C++17

template <class Key, class T, class Compare, class Allocator>
bool
operator==(const map<Key, T, Compare, Allocator>& x,
const map<Key, T, Compare, Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool
operator< (const map<Key, T, Compare, Allocator>& x,
const map<Key, T, Compare, Allocator>& y);      // removed in C++20

template <class Key, class T, class Compare, class Allocator>
bool
operator!=(const map<Key, T, Compare, Allocator>& x,
const map<Key, T, Compare, Allocator>& y);      // removed in C++20

template <class Key, class T, class Compare, class Allocator>
bool
operator> (const map<Key, T, Compare, Allocator>& x,
const map<Key, T, Compare, Allocator>& y);      // removed in C++20

template <class Key, class T, class Compare, class Allocator>
bool
operator>=(const map<Key, T, Compare, Allocator>& x,
const map<Key, T, Compare, Allocator>& y);      // removed in C++20

template <class Key, class T, class Compare, class Allocator>
bool
operator<=(const map<Key, T, Compare, Allocator>& x,
const map<Key, T, Compare, Allocator>& y);      // removed in C++20

template<class Key, class T, class Compare, class Allocator>
synth-three-way-result<pair<const Key, T>>
operator<=>(const map<Key, T, Compare, Allocator>& x,
const map<Key, T, Compare, Allocator>& y); // since C++20

// specialized algorithms:
template <class Key, class T, class Compare, class Allocator>
void
swap(map<Key, T, Compare, Allocator>& x, map<Key, T, Compare, Allocator>& y)
noexcept(noexcept(x.swap(y)));

template <class Key, class T, class Compare, class Allocator, class Predicate>
typename map<Key, T, Compare, Allocator>::size_type
erase_if(map<Key, T, Compare, Allocator>& c, Predicate pred);  // C++20


template <class Key, class T, class Compare = less<Key>,
class Allocator = allocator<pair<const Key, T>>>
class multimap
{
public:
    // types:
    typedef Key                                      key_type;
    typedef T                                        mapped_type;
    typedef std::pair<const key_type,mapped_type>         value_type;
    typedef Compare                                  key_compare;
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
    typedef unspecified                              node_type;              // C++17

    class value_compare
    {
    friend class multimap;
    protected:
        key_compare comp;
        value_compare(key_compare c);
        public:
            typedef bool result_type;  // deprecated in C++17, removed in C++20
            typedef value_type first_argument_type;  // deprecated in C++17, removed in C++20
            typedef value_type second_argument_type;  // deprecated in C++17, removed in C++20
            bool operator()(const value_type& x, const value_type& y) const;
            };

            // construct/copy/destroy:
            multimap()
            noexcept(
                is_nothrow_default_constructible<allocator_type>::value &&
                is_nothrow_default_constructible<key_compare>::value &&
                is_nothrow_copy_constructible<key_compare>::value);
                explicit multimap(const key_compare& comp);
                multimap(const key_compare& comp, const allocator_type& a);
                template <class InputIterator>
                multimap(InputIterator first, InputIterator last, const key_compare& comp);
                template <class InputIterator>
                multimap(InputIterator first, InputIterator last, const key_compare& comp,
                const allocator_type& a);
                template<container-compatible-range<value_type> R>
                multimap(std::from_range_t, R&& rg,
                const Compare& comp = Compare(), const Allocator& = Allocator()); // C++23
                multimap(const multimap& m);
                multimap(multimap&& m)
                noexcept(
                    is_nothrow_move_constructible<allocator_type>::value &&
                    is_nothrow_move_constructible<key_compare>::value);
                    explicit multimap(const allocator_type& a);
                    multimap(const multimap& m, const allocator_type& a);
                    multimap(multimap&& m, const allocator_type& a);
                    multimap(std::initializer_list<value_type> il, const key_compare& comp = key_compare());
                    multimap(std::initializer_list<value_type> il, const key_compare& comp,
                    const allocator_type& a);
                    template <class InputIterator>
                    multimap(InputIterator first, InputIterator last, const allocator_type& a)
                    : multimap(first, last, Compare(), a) {} // C++14
                    template<container-compatible-range<value_type> R>
                    multimap(std::from_range_t, R&& rg, const Allocator& a))
                    : multimap(from_range, std::forward<R>(rg), Compare(), a) { } // C++23
                    multimap(std::initializer_list<value_type> il, const allocator_type& a)
                    : multimap(il, Compare(), a) {} // C++14
                    ~multimap();

                    multimap& operator=(const multimap& m);
                    multimap& operator=(multimap&& m)
                    noexcept(
                        allocator_type::propagate_on_container_move_assignment::value &&
                        is_nothrow_move_assignable<allocator_type>::value &&
                        is_nothrow_move_assignable<key_compare>::value);
                        multimap& operator=(std::initializer_list<value_type> il);

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
                        iterator insert(      value_type&& v);                                            // C++17
                        template <class P>
                        iterator insert(P&& p);
                        iterator insert(const_iterator position, const value_type& v);
                        iterator insert(const_iterator position,       value_type&& v);                   // C++17
                        template <class P>
                        iterator insert(const_iterator position, P&& p);
                        template <class InputIterator>
                        void insert(InputIterator first, InputIterator last);
                        template<container-compatible-range<value_type> R>
                        void insert_range(R&& rg);                                                      // C++23
                        void insert(std::initializer_list<value_type> il);

                        node_type extract(const_iterator position);                                       // C++17
                        node_type extract(const key_type& x);                                             // C++17
                        iterator insert(node_type&& nh);                                                  // C++17
                        iterator insert(const_iterator hint, node_type&& nh);                             // C++17

                        iterator  erase(const_iterator position);
                        iterator  erase(iterator position); // C++14
                        size_type erase(const key_type& k);
                        iterator  erase(const_iterator first, const_iterator last);
                        void clear() noexcept;

                        template<class C2>
                        void merge(multimap<Key, T, C2, Allocator>& source);    // C++17
                        template<class C2>
                        void merge(multimap<Key, T, C2, Allocator>&& source);   // C++17
                        template<class C2>
                        void merge(map<Key, T, C2, Allocator>& source);         // C++17
                        template<class C2>
                        void merge(map<Key, T, C2, Allocator>&& source);        // C++17

                        void swap(multimap& m)
                        noexcept(allocator_traits<allocator_type>::is_always_equal::value &&
                        is_nothrow_swappable<key_compare>::value); // C++17

                        // observers:
                        allocator_type get_allocator() const noexcept;
                        key_compare    key_comp()      const;
                        value_compare  value_comp()    const;

                        // map operations:
                        iterator find(const key_type& k);
                        const_iterator find(const key_type& k) const;
                        template<typename K>
                        iterator find(const K& x);              // C++14
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
class Compare = less<iter_key_t<InputIterator>>,
class Allocator = allocator<iter_to_alloc_t<InputIterator>>>
multimap(InputIterator, InputIterator, Compare = Compare(), Allocator = Allocator())
-> multimap<iter_key_t<InputIterator>, iter_val_t<InputIterator>, Compare, Allocator>; // C++17

template< std::ranges::input_range R, class Compare = less<range-key-type<R>>,
class Allocator = allocator<range-to-alloc-type<R>>>
multimap(std::from_range_t, R&&, Compare = Compare(), Allocator = Allocator())
-> multimap<range-key-type<R>, range-mapped-type<R>, Compare, Allocator>; // C++23

template<class Key, class T, class Compare = less<Key>,
class Allocator = allocator<pair<const Key, T>>>
multimap(std::initializer_list<pair<const Key, T>>, Compare = Compare(), Allocator = Allocator())
-> multimap<Key, T, Compare, Allocator>; // C++17

template <class InputIterator, class Allocator>
multimap(InputIterator, InputIterator, Allocator)
-> multimap<iter_key_t<InputIterator>, iter_val_t<InputIterator>,
less<iter_key_t<InputIterator>>, Allocator>; // C++17

template< std::ranges::input_range R, class Allocator>
multimap(std::from_range_t, R&&, Allocator)
-> multimap<range-key-type<R>, range-mapped-type<R>, less<range-key-type<R>>, Allocator>; // C++23

template<class Key, class T, class Allocator>
multimap(std::initializer_list<pair<const Key, T>>, Allocator)
-> multimap<Key, T, less<Key>, Allocator>; // C++17

template <class Key, class T, class Compare, class Allocator>
bool
operator==(const multimap<Key, T, Compare, Allocator>& x,
const multimap<Key, T, Compare, Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool
operator< (const multimap<Key, T, Compare, Allocator>& x,
const multimap<Key, T, Compare, Allocator>& y);      // removed in C++20

template <class Key, class T, class Compare, class Allocator>
bool
operator!=(const multimap<Key, T, Compare, Allocator>& x,
const multimap<Key, T, Compare, Allocator>& y);      // removed in C++20

template <class Key, class T, class Compare, class Allocator>
bool
operator> (const multimap<Key, T, Compare, Allocator>& x,
const multimap<Key, T, Compare, Allocator>& y);      // removed in C++20

template <class Key, class T, class Compare, class Allocator>
bool
operator>=(const multimap<Key, T, Compare, Allocator>& x,
const multimap<Key, T, Compare, Allocator>& y);      // removed in C++20

template <class Key, class T, class Compare, class Allocator>
bool
operator<=(const multimap<Key, T, Compare, Allocator>& x,
const multimap<Key, T, Compare, Allocator>& y);      // removed in C++20

template<class Key, class T, class Compare, class Allocator>
synth-three-way-result<pair<const Key, T>>
operator<=>(const multimap<Key, T, Compare, Allocator>& x,
const multimap<Key, T, Compare, Allocator>& y); // since c++20

// specialized algorithms:
template <class Key, class T, class Compare, class Allocator>
void
swap(multimap<Key, T, Compare, Allocator>& x,
multimap<Key, T, Compare, Allocator>& y)
noexcept(noexcept(x.swap(y)));

template <class Key, class T, class Compare, class Allocator, class Predicate>
typename multimap<Key, T, Compare, Allocator>::size_type
erase_if(multimap<Key, T, Compare, Allocator>& c, Predicate pred);  // C++20

}  // std

*/

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

#  if !defined(MSTD_HAS_NO_PRAGMA_SYSTEM_HEADER)
#    pragma GCC system_header
#  endif


namespace mstd {

template <class _Key, class _CP, class _Compare>
class __map_value_compare {
    MSTD_COMPRESSED_ELEMENT(_Compare, __comp_);

public:
    MSTD_HIDE_FROM_ABI __map_value_compare() noexcept(std::is_nothrow_default_constructible<_Compare>::value)
    : __comp_() {}
    MSTD_HIDE_FROM_ABI __map_value_compare(_Compare __c) noexcept(std::is_nothrow_copy_constructible<_Compare>::value)
    : __comp_(__c) {}
    MSTD_HIDE_FROM_ABI const _Compare& key_comp() const noexcept { return __comp_; }

    MSTD_HIDE_FROM_ABI bool operator()(const _CP& __x, const _CP& __y) const { return __comp_(__x.first, __y.first); }
    MSTD_HIDE_FROM_ABI bool operator()(const _CP& __x, const _Key& __y) const { return __comp_(__x.first, __y); }
    MSTD_HIDE_FROM_ABI bool operator()(const _Key& __x, const _CP& __y) const { return __comp_(__x, __y.first); }
    MSTD_HIDE_FROM_ABI void swap(__map_value_compare& __y) noexcept(std::is_nothrow_swappable_v<_Compare>) {
        using std::swap;
        swap(__comp_, __y.__comp_);
    }

    template <typename _K2>
    MSTD_HIDE_FROM_ABI bool operator()(const _K2& __x, const _CP& __y) const {
        return __comp_(__x, __y.first);
    }

    template <typename _K2>
    MSTD_HIDE_FROM_ABI bool operator()(const _CP& __x, const _K2& __y) const {
        return __comp_(__x.first, __y);
    }
};

template <class _Key, class _MapValueT, class _Compare>
struct __make_transparent<__map_value_compare<_Key, _MapValueT, _Compare> > {
    using type MSTD_NODEBUG = __map_value_compare<_Key, _MapValueT, __make_transparent_t<_Compare> >;
};

template <class _MapValueT, class _Key, class _Compare>
struct __lazy_synth_three_way_comparator<__map_value_compare<_Key, _MapValueT, _Compare>, _MapValueT, _MapValueT> {
    __lazy_synth_three_way_comparator<_Compare, _Key, _Key> __comp_;

    __lazy_synth_three_way_comparator(
        MSTD_CTOR_LIFETIMEBOUND const __map_value_compare<_Key, _MapValueT, _Compare>& __comp)
    : __comp_(__comp.key_comp()) {}

    MSTD_HIDE_FROM_ABI auto
    operator()(MSTD_LIFETIMEBOUND const _MapValueT& __lhs, MSTD_LIFETIMEBOUND const _MapValueT& __rhs) const {
        return __comp_(__lhs.first, __rhs.first);
    }
};

template <class _MapValueT, class _Key, class _TransparentKey, class _Compare>
struct __lazy_synth_three_way_comparator<__map_value_compare<_Key, _MapValueT, _Compare>, _TransparentKey, _MapValueT> {
    __lazy_synth_three_way_comparator<_Compare, _TransparentKey, _Key> __comp_;

    __lazy_synth_three_way_comparator(
        MSTD_CTOR_LIFETIMEBOUND const __map_value_compare<_Key, _MapValueT, _Compare>& __comp)
    : __comp_(__comp.key_comp()) {}

    MSTD_HIDE_FROM_ABI auto
    operator()(MSTD_LIFETIMEBOUND const _TransparentKey& __lhs, MSTD_LIFETIMEBOUND const _MapValueT& __rhs) const {
        return __comp_(__lhs, __rhs.first);
    }
};

template <class _MapValueT, class _Key, class _TransparentKey, class _Compare>
struct __lazy_synth_three_way_comparator<__map_value_compare<_Key, _MapValueT, _Compare>, _MapValueT, _TransparentKey> {
    __lazy_synth_three_way_comparator<_Compare, _Key, _TransparentKey> __comp_;

    __lazy_synth_three_way_comparator(
        MSTD_CTOR_LIFETIMEBOUND const __map_value_compare<_Key, _MapValueT, _Compare>& __comp)
    : __comp_(__comp.key_comp()) {}

    MSTD_HIDE_FROM_ABI auto
    operator()(MSTD_LIFETIMEBOUND const _MapValueT& __lhs, MSTD_LIFETIMEBOUND const _TransparentKey& __rhs) const {
        return __comp_(__lhs.first, __rhs);
    }
};

template <class _Key, class _CP, class _Compare>
inline MSTD_HIDE_FROM_ABI void
swap(__map_value_compare<_Key, _CP, _Compare>& __x, __map_value_compare<_Key, _CP, _Compare>& __y)
noexcept(noexcept(__x.swap(__y))) {
    __x.swap(__y);
}

template <class _Allocator>
class __map_node_destructor {
    typedef _Allocator allocator_type;
    typedef allocator_traits<allocator_type> __alloc_traits;

public:
    typedef typename __alloc_traits::pointer pointer;

private:
    allocator_type& __na_;

public:
    bool __first_constructed;
    bool __second_constructed;

    MSTD_HIDE_FROM_ABI explicit __map_node_destructor(allocator_type& __na) noexcept
    : __na_(__na),
    __first_constructed(false),
    __second_constructed(false) {}

    MSTD_HIDE_FROM_ABI __map_node_destructor(__tree_node_destructor<allocator_type>&& __x) noexcept
    : __na_(__x.__na_),
    __first_constructed(__x.__value_constructed),
    __second_constructed(__x.__value_constructed) {
        __x.__value_constructed = false;
    }

    __map_node_destructor& operator=(const __map_node_destructor&) = delete;

    MSTD_HIDE_FROM_ABI void operator()(pointer __p) noexcept {
        if (__second_constructed)
            __alloc_traits::destroy(__na_, std::addressof(__p->__get_value().second));
        if (__first_constructed)
            __alloc_traits::destroy(__na_, std::addressof(__p->__get_value().first));
        if (__p)
            __alloc_traits::deallocate(__na_, __p, 1);
    }
};

template <class _Key, class _Tp>
struct __value_type;

template <class _TreeIterator>
class __map_iterator {
    _TreeIterator __i_;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = typename _TreeIterator::value_type;
    using difference_type   = typename _TreeIterator::difference_type;
    using reference         = value_type&;
    using pointer           = typename _TreeIterator::pointer;

    MSTD_HIDE_FROM_ABI __map_iterator() noexcept {}

    MSTD_HIDE_FROM_ABI __map_iterator(_TreeIterator __i) noexcept : __i_(__i) {}

    MSTD_HIDE_FROM_ABI reference operator*() const { return *__i_; }
    MSTD_HIDE_FROM_ABI pointer operator->() const { return std::pointer_traits<pointer>::pointer_to(*__i_); }

    MSTD_HIDE_FROM_ABI __map_iterator& operator++() {
        ++__i_;
        return *this;
    }
    MSTD_HIDE_FROM_ABI __map_iterator operator++(int) {
        __map_iterator __t(*this);
        ++(*this);
        return __t;
    }

    MSTD_HIDE_FROM_ABI __map_iterator& operator--() {
        --__i_;
        return *this;
    }
    MSTD_HIDE_FROM_ABI __map_iterator operator--(int) {
        __map_iterator __t(*this);
        --(*this);
        return __t;
    }

    friend MSTD_HIDE_FROM_ABI bool operator==(const __map_iterator& __x, const __map_iterator& __y) {
        return __x.__i_ == __y.__i_;
    }
    friend MSTD_HIDE_FROM_ABI bool operator!=(const __map_iterator& __x, const __map_iterator& __y) {
        return __x.__i_ != __y.__i_;
    }

    template <class, class, class, class>
    friend class map;
    template <class, class, class, class>
    friend class multimap;
    template <class>
    friend class __map_const_iterator;

    template <class, class...>
    friend struct __specialized_algorithm;
};

template <class _Alg, class _TreeIterator>
struct __specialized_algorithm<_Alg, __iterator_pair<__map_iterator<_TreeIterator>, __map_iterator<_TreeIterator>>> {
    using __base MSTD_NODEBUG = __specialized_algorithm<_Alg, __iterator_pair<_TreeIterator, _TreeIterator>>;

    static const bool __has_algorithm = __base::__has_algorithm;

    using __iterator MSTD_NODEBUG = __map_iterator<_TreeIterator>;

    template <class... _Args>
    MSTD_HIDE_FROM_ABI static void operator()(__iterator __first, __iterator __last, _Args&&... __args) {
        __base()(__first.__i_, __last.__i_, std::forward<_Args>(__args)...);
    }
};

template <class _TreeIterator>
class __map_const_iterator {
    _TreeIterator __i_;

public:
    using iterator_category = bidirectional_iterator_tag;
    using value_type        = typename _TreeIterator::value_type;
    using difference_type   = typename _TreeIterator::difference_type;
    using reference         = const value_type&;
    using pointer           = typename _TreeIterator::pointer;

    MSTD_HIDE_FROM_ABI __map_const_iterator() noexcept {}

    MSTD_HIDE_FROM_ABI __map_const_iterator(_TreeIterator __i) noexcept : __i_(__i) {}
    MSTD_HIDE_FROM_ABI
    __map_const_iterator(__map_iterator< typename _TreeIterator::__non_const_iterator> __i) noexcept : __i_(__i.__i_) {}

    MSTD_HIDE_FROM_ABI reference operator*() const { return *__i_; }
    MSTD_HIDE_FROM_ABI pointer operator->() const { return std::pointer_traits<pointer>::pointer_to(*__i_); }

    MSTD_HIDE_FROM_ABI __map_const_iterator& operator++() {
        ++__i_;
        return *this;
    }
    MSTD_HIDE_FROM_ABI __map_const_iterator operator++(int) {
        __map_const_iterator __t(*this);
        ++(*this);
        return __t;
    }

    MSTD_HIDE_FROM_ABI __map_const_iterator& operator--() {
        --__i_;
        return *this;
    }
    MSTD_HIDE_FROM_ABI __map_const_iterator operator--(int) {
        __map_const_iterator __t(*this);
        --(*this);
        return __t;
    }

    friend MSTD_HIDE_FROM_ABI bool operator==(const __map_const_iterator& __x, const __map_const_iterator& __y) {
        return __x.__i_ == __y.__i_;
    }
    friend MSTD_HIDE_FROM_ABI bool operator!=(const __map_const_iterator& __x, const __map_const_iterator& __y) {
        return __x.__i_ != __y.__i_;
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

template <class _Alg, class _TreeIterator>
struct __specialized_algorithm<
_Alg,
__iterator_pair<__map_const_iterator<_TreeIterator>, __map_const_iterator<_TreeIterator>>> {
    using __base MSTD_NODEBUG = __specialized_algorithm<_Alg, __iterator_pair<_TreeIterator, _TreeIterator>>;

    static const bool __has_algorithm = __base::__has_algorithm;

    using __iterator MSTD_NODEBUG = __map_const_iterator<_TreeIterator>;

    template <class... _Args>
    MSTD_HIDE_FROM_ABI static void operator()(__iterator __first, __iterator __last, _Args&&... __args) {
        __base()(__first.__i_, __last.__i_, std::forward<_Args>(__args)...);
    }
};

template <class _Key, class _Tp, class _Compare = std::less<_Key>, class _Allocator = std::allocator<std::pair<const _Key, _Tp> > >
class multimap;

template <class _Key, class _Tp, class _Compare = std::less<_Key>, class _Allocator = std::allocator<std::pair<const _Key, _Tp> > >
class map {
public:
    // types:
    typedef _Key key_type;
    typedef _Tp mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef std::type_identity_t<_Compare> key_compare;
    typedef std::type_identity_t<_Allocator> allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

    class value_compare : public std::binary_function<value_type, value_type, bool> {
        friend class map;

    protected:
        key_compare comp;

        MSTD_HIDE_FROM_ABI value_compare(key_compare __c) : comp(__c) {}

    public:
        MSTD_HIDE_FROM_ABI bool operator()(const value_type& __x, const value_type& __y) const {
            return comp(__x.first, __y.first);
        }
    };

private:
    typedef __value_type<key_type, mapped_type> __value_type;
    typedef __map_value_compare<key_type, value_type, key_compare> __vc;
    typedef __tree<__value_type, __vc, allocator_type> __base;
    typedef typename __base::__node_traits __node_traits;
    typedef allocator_traits<allocator_type> __alloc_traits;

    static_assert(__check_valid_allocator<allocator_type>::value, "");

    __base __tree_;

public:
    typedef typename __alloc_traits::pointer pointer;
    typedef typename __alloc_traits::const_pointer const_pointer;
    typedef typename __alloc_traits::size_type size_type;
    typedef typename __alloc_traits::difference_type difference_type;
    typedef __map_iterator<typename __base::iterator> iterator;
    typedef __map_const_iterator<typename __base::const_iterator> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef __map_node_handle<typename __base::__node, allocator_type> node_type;
    typedef __insert_return_type<iterator, node_type> insert_return_type;

    template <class _Key2, class _Value2, class _Comp2, class _Alloc2>
    friend class map;
    template <class _Key2, class _Value2, class _Comp2, class _Alloc2>
    friend class multimap;

    MSTD_HIDE_FROM_ABI map() noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value && std::is_nothrow_default_constructible<key_compare>::value&&
        std::is_nothrow_copy_constructible<key_compare>::value)
    : __tree_(__vc(key_compare())) {}

    MSTD_HIDE_FROM_ABI explicit map(const key_compare& __comp) noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value&& std::is_nothrow_copy_constructible<key_compare>::value)
    : __tree_(__vc(__comp)) {}

    MSTD_HIDE_FROM_ABI explicit map(const key_compare& __comp, const allocator_type& __a)
    : __tree_(__vc(__comp), typename __base::allocator_type(__a)) {}

    template <class _InputIterator>
    MSTD_HIDE_FROM_ABI map(_InputIterator __f, _InputIterator __l, const key_compare& __comp = key_compare())
    : __tree_(__vc(__comp)) {
        insert(__f, __l);
    }

    template <class _InputIterator>
    MSTD_HIDE_FROM_ABI
    map(_InputIterator __f, _InputIterator __l, const key_compare& __comp, const allocator_type& __a)
    : __tree_(__vc(__comp), typename __base::allocator_type(__a)) {
        insert(__f, __l);
    }

    template <_ContainerCompatibleRange<value_type> _Range>
    MSTD_HIDE_FROM_ABI
    map(std::from_range_t,
        _Range&& __range,
        const key_compare& __comp = key_compare(),
        const allocator_type& __a = allocator_type())
    : __tree_(__vc(__comp), typename __base::allocator_type(__a)) {
        insert_range(std::forward<_Range>(__range));
    }
    
    template <class _InputIterator>
    MSTD_HIDE_FROM_ABI map(_InputIterator __f, _InputIterator __l, const allocator_type& __a)
    : map(__f, __l, key_compare(), __a) {}

    template <_ContainerCompatibleRange<value_type> _Range>
    MSTD_HIDE_FROM_ABI map(std::from_range_t, _Range&& __range, const allocator_type& __a)
    : map(std::from_range, std::forward<_Range>(__range), key_compare(), __a) {}

    MSTD_HIDE_FROM_ABI map(const map& __m) = default;

    MSTD_HIDE_FROM_ABI map& operator=(const map& __m) = default;

    MSTD_HIDE_FROM_ABI map(map&& __m) = default;

    MSTD_HIDE_FROM_ABI map(map&& __m, const allocator_type& __a) : __tree_(std::move(__m.__tree_), __a) {}

    MSTD_HIDE_FROM_ABI map& operator=(map&& __m) = default;

    MSTD_HIDE_FROM_ABI map(std::initializer_list<value_type> __il, const key_compare& __comp = key_compare())
    : __tree_(__vc(__comp)) {
        insert(__il.begin(), __il.end());
    }

    MSTD_HIDE_FROM_ABI map(std::initializer_list<value_type> __il, const key_compare& __comp, const allocator_type& __a)
    : __tree_(__vc(__comp), typename __base::allocator_type(__a)) {
        insert(__il.begin(), __il.end());
    }

    MSTD_HIDE_FROM_ABI map(std::initializer_list<value_type> __il, const allocator_type& __a)
    : map(__il, key_compare(), __a) {}

    MSTD_HIDE_FROM_ABI map& operator=(std::initializer_list<value_type> __il) {
        clear();
        insert(__il.begin(), __il.end());
        return *this;
    }

    MSTD_HIDE_FROM_ABI explicit map(const allocator_type& __a) : __tree_(typename __base::allocator_type(__a)) {}

    MSTD_HIDE_FROM_ABI map(const map& __m, const allocator_type& __alloc) : __tree_(__m.__tree_, __alloc) {}

    MSTD_HIDE_FROM_ABI ~map() { static_assert(sizeof(mstd::__diagnose_non_const_comparator<_Key, _Compare>()), ""); }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator begin() noexcept { return __tree_.begin(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator begin() const noexcept { return __tree_.begin(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator end() noexcept { return __tree_.end(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator end() const noexcept { return __tree_.end(); }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator cbegin() const noexcept { return begin(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator cend() const noexcept { return end(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_reverse_iterator crend() const noexcept { return rend(); }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI bool empty() const noexcept { return __tree_.size() == 0; }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI size_type size() const noexcept { return __tree_.size(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI size_type max_size() const noexcept { return __tree_.max_size(); }

    MSTD_HIDE_FROM_ABI mapped_type& operator[](const key_type& __k);
    MSTD_HIDE_FROM_ABI mapped_type& operator[](key_type&& __k);

    template <class _Arg,
    std::enable_if_t<__is_transparently_comparable_v<_Compare, key_type, std::remove_cvref_t<_Arg> >, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI mapped_type& at(_Arg&& __arg) {
        auto [_, __child] = __tree_.__find_equal(__arg);
        if (__child == nullptr)
            std::__throw_out_of_range("map::at:  key not found");
        return static_cast<__node_pointer>(__child)->__get_value().second;
    }

    template <class _Arg,
    std::enable_if_t<__is_transparently_comparable_v<_Compare, key_type, std::remove_cvref_t<_Arg> >, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const mapped_type& at(_Arg&& __arg) const {
        auto [_, __child] = __tree_.__find_equal(__arg);
        if (__child == nullptr)
            std::__throw_out_of_range("map::at:  key not found");
        return static_cast<__node_pointer>(__child)->__get_value().second;
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI mapped_type& at(const key_type& __k);
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const mapped_type& at(const key_type& __k) const;

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI allocator_type get_allocator() const noexcept {
        return allocator_type(__tree_.__alloc());
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI key_compare key_comp() const { return __tree_.value_comp().key_comp(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI value_compare value_comp() const {
        return value_compare(__tree_.value_comp().key_comp());
    }

    template <class... _Args>
    MSTD_HIDE_FROM_ABI std::pair<iterator, bool> emplace(_Args&&... __args) {
        return __tree_.__emplace_unique(std::forward<_Args>(__args)...);
    }

    template <class... _Args>
    MSTD_HIDE_FROM_ABI iterator emplace_hint(const_iterator __p, _Args&&... __args) {
        return __tree_.__emplace_hint_unique(__p.__i_, std::forward<_Args>(__args)...).first;
    }

    template <class _Pp, std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0>
    MSTD_HIDE_FROM_ABI std::pair<iterator, bool> insert(_Pp&& __p) {
        return __tree_.__emplace_unique(std::forward<_Pp>(__p));
    }

    template <class _Pp, std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0>
    MSTD_HIDE_FROM_ABI iterator insert(const_iterator __pos, _Pp&& __p) {
        return __tree_.__emplace_hint_unique(__pos.__i_, std::forward<_Pp>(__p)).first;
    }

    MSTD_HIDE_FROM_ABI std::pair<iterator, bool> insert(const value_type& __v) { return __tree_.__emplace_unique(__v); }

    MSTD_HIDE_FROM_ABI iterator insert(const_iterator __p, const value_type& __v) {
        return __tree_.__emplace_hint_unique(__p.__i_, __v).first;
    }

    MSTD_HIDE_FROM_ABI std::pair<iterator, bool> insert(value_type&& __v) {
        return __tree_.__emplace_unique(std::move(__v));
    }

    MSTD_HIDE_FROM_ABI iterator insert(const_iterator __p, value_type&& __v) {
        return __tree_.__emplace_hint_unique(__p.__i_, std::move(__v)).first;
    }

    MSTD_HIDE_FROM_ABI void insert(std::initializer_list<value_type> __il) { insert(__il.begin(), __il.end()); }

    template <class _InputIterator>
    MSTD_HIDE_FROM_ABI void insert(_InputIterator __first, _InputIterator __last) {
        __tree_.__insert_range_unique(__first, __last);
    }

    template <_ContainerCompatibleRange<value_type> _Range>
    MSTD_HIDE_FROM_ABI void insert_range(_Range&& __range) {
        __tree_.__insert_range_unique( std::ranges::begin(__range),  std::ranges::end(__range));
    }

    template <class... _Args>
    MSTD_HIDE_FROM_ABI std::pair<iterator, bool> try_emplace(const key_type& __k, _Args&&... __args) {
        return __tree_.__emplace_unique(
            std::piecewise_construct, std::forward_as_tuple(__k), std::forward_as_tuple(std::forward<_Args>(__args)...));
    }

    template <class... _Args>
    MSTD_HIDE_FROM_ABI std::pair<iterator, bool> try_emplace(key_type&& __k, _Args&&... __args) {
        return __tree_.__emplace_unique(
            std::piecewise_construct,
            std::forward_as_tuple(std::move(__k)),
                                        std::forward_as_tuple(std::forward<_Args>(__args)...));
    }

    template <class... _Args>
    MSTD_HIDE_FROM_ABI iterator try_emplace(const_iterator __h, const key_type& __k, _Args&&... __args) {
        return __tree_
        .__emplace_hint_unique(
            __h.__i_,
            std::piecewise_construct,
            std::forward_as_tuple(__k),
                               std::forward_as_tuple(std::forward<_Args>(__args)...))
        .first;
    }

    template <class... _Args>
    MSTD_HIDE_FROM_ABI iterator try_emplace(const_iterator __h, key_type&& __k, _Args&&... __args) {
        return __tree_
        .__emplace_hint_unique(
            __h.__i_,
            std::piecewise_construct,
            std::forward_as_tuple(std::move(__k)),
                               std::forward_as_tuple(std::forward<_Args>(__args)...))
        .first;
    }

    template <class _Vp>
    MSTD_HIDE_FROM_ABI std::pair<iterator, bool> insert_or_assign(const key_type& __k, _Vp&& __v) {
        auto __result              = __tree_.__emplace_unique(__k, std::forward<_Vp>(__v));
        auto& [__iter, __inserted] = __result;
        if (!__inserted)
            __iter->second = std::forward<_Vp>(__v);
        return __result;
    }

    template <class _Vp>
    MSTD_HIDE_FROM_ABI std::pair<iterator, bool> insert_or_assign(key_type&& __k, _Vp&& __v) {
        auto __result              = __tree_.__emplace_unique(std::move(__k), std::forward<_Vp>(__v));
        auto& [__iter, __inserted] = __result;
        if (!__inserted)
            __iter->second = std::forward<_Vp>(__v);
        return __result;
    }

    template <class _Vp>
    MSTD_HIDE_FROM_ABI iterator insert_or_assign(const_iterator __h, const key_type& __k, _Vp&& __v) {
        auto [__r, __inserted] = __tree_.__emplace_hint_unique(__h.__i_, __k, std::forward<_Vp>(__v));

        if (!__inserted)
            __r->second = std::forward<_Vp>(__v);

        return __r;
    }

    template <class _Vp>
    MSTD_HIDE_FROM_ABI iterator insert_or_assign(const_iterator __h, key_type&& __k, _Vp&& __v) {
        auto [__r, __inserted] = __tree_.__emplace_hint_unique(__h.__i_, std::move(__k), std::forward<_Vp>(__v));

        if (!__inserted)
            __r->second = std::forward<_Vp>(__v);

        return __r;
    }

    MSTD_HIDE_FROM_ABI iterator erase(const_iterator __p) { return __tree_.erase(__p.__i_); }
    MSTD_HIDE_FROM_ABI iterator erase(iterator __p) { return __tree_.erase(__p.__i_); }
    MSTD_HIDE_FROM_ABI size_type erase(const key_type& __k) { return __tree_.__erase_unique(__k); }
    MSTD_HIDE_FROM_ABI iterator erase(const_iterator __f, const_iterator __l) {
        return __tree_.erase(__f.__i_, __l.__i_);
    }
    MSTD_HIDE_FROM_ABI void clear() noexcept { __tree_.clear(); }

    MSTD_HIDE_FROM_ABI insert_return_type insert(node_type&& __nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(__nh.empty() || __nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to map::insert()");
        return __tree_.template __node_handle_insert_unique< node_type, insert_return_type>(std::move(__nh));
    }
    MSTD_HIDE_FROM_ABI iterator insert(const_iterator __hint, node_type&& __nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(__nh.empty() || __nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to map::insert()");
        return __tree_.template __node_handle_insert_unique<node_type>(__hint.__i_, std::move(__nh));
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI node_type extract(key_type const& __key) {
        return __tree_.template __node_handle_extract<node_type>(__key);
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI node_type extract(const_iterator __it) {
        return __tree_.template __node_handle_extract<node_type>(__it.__i_);
    }
    template <class _Compare2>
    MSTD_HIDE_FROM_ABI void merge(map<key_type, mapped_type, _Compare2, allocator_type>& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        __tree_.__node_handle_merge_unique(__source.__tree_);
    }
    template <class _Compare2>
    MSTD_HIDE_FROM_ABI void merge(map<key_type, mapped_type, _Compare2, allocator_type>&& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        __tree_.__node_handle_merge_unique(__source.__tree_);
    }
    template <class _Compare2>
    MSTD_HIDE_FROM_ABI void merge(multimap<key_type, mapped_type, _Compare2, allocator_type>& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        __tree_.__node_handle_merge_unique(__source.__tree_);
    }
    template <class _Compare2>
    MSTD_HIDE_FROM_ABI void merge(multimap<key_type, mapped_type, _Compare2, allocator_type>&& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        __tree_.__node_handle_merge_unique(__source.__tree_);
    }

    MSTD_HIDE_FROM_ABI void swap(map& __m) noexcept(std::is_nothrow_swappable_v<__base>) { __tree_.swap(__m.__tree_); }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator find(const key_type& __k) { return __tree_.find(__k); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator find(const key_type& __k) const { return __tree_.find(__k); }
    template <typename _K2,
    std::enable_if_t<__is_transparent_v<_Compare, _K2> || __is_transparently_comparable_v<_Compare, key_type, _K2>,
    int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator find(const _K2& __k) {
        return __tree_.find(__k);
    }
    template <typename _K2,
    std::enable_if_t<__is_transparent_v<_Compare, _K2> || __is_transparently_comparable_v<_Compare, key_type, _K2>,
    int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator find(const _K2& __k) const {
        return __tree_.find(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI size_type count(const key_type& __k) const {
        return __tree_.__count_unique(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI size_type count(const _K2& __k) const {
        return __tree_.__count_multi(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI bool contains(const key_type& __k) const { return find(__k) != end(); }
    template <typename _K2,
    std::enable_if_t<__is_transparent_v<_Compare, _K2> || __is_transparently_comparable_v<_Compare, key_type, _K2>,
    int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI bool contains(const _K2& __k) const {
        return find(__k) != end();
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator lower_bound(const key_type& __k) {
        return __tree_.__lower_bound_unique(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator lower_bound(const key_type& __k) const {
        return __tree_.__lower_bound_unique(__k);
    }

    // The transparent versions of the lookup functions use the _multi version, since a non-element key is allowed to
    // match multiple elements.
    template <typename _K2,
    std::enable_if_t<__is_transparent_v<_Compare, _K2> || __is_transparently_comparable_v<_Compare, key_type, _K2>,
    int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator lower_bound(const _K2& __k) {
        return __tree_.__lower_bound_multi(__k);
    }

    template <typename _K2,
    std::enable_if_t<__is_transparent_v<_Compare, _K2> || __is_transparently_comparable_v<_Compare, key_type, _K2>,
    int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator lower_bound(const _K2& __k) const {
        return __tree_.__lower_bound_multi(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator upper_bound(const key_type& __k) {
        return __tree_.__upper_bound_unique(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator upper_bound(const key_type& __k) const {
        return __tree_.__upper_bound_unique(__k);
    }

    template <typename _K2,
    std::enable_if_t<__is_transparent_v<_Compare, _K2> || __is_transparently_comparable_v<_Compare, key_type, _K2>,
    int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator upper_bound(const _K2& __k) {
        return __tree_.__upper_bound_multi(__k);
    }
    template <typename _K2,
    std::enable_if_t<__is_transparent_v<_Compare, _K2> || __is_transparently_comparable_v<_Compare, key_type, _K2>,
    int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator upper_bound(const _K2& __k) const {
        return __tree_.__upper_bound_multi(__k);
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI std::pair<iterator, iterator> equal_range(const key_type& __k) {
        return __tree_.__equal_range_unique(__k);
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI std::pair<const_iterator, const_iterator> equal_range(const key_type& __k) const {
        return __tree_.__equal_range_unique(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI std::pair<iterator, iterator> equal_range(const _K2& __k) {
        return __tree_.__equal_range_multi(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI std::pair<const_iterator, const_iterator> equal_range(const _K2& __k) const {
        return __tree_.__equal_range_multi(__k);
    }

private:
    typedef typename __base::__node __node;
    typedef typename __base::__node_allocator __node_allocator;
    typedef typename __base::__node_pointer __node_pointer;
    typedef typename __base::__node_base_pointer __node_base_pointer;

    typedef __map_node_destructor<__node_allocator> _Dp;
    typedef std::unique_ptr<__node, _Dp> __node_holder;

    friend struct __specialized_algorithm<_Algorithm::__for_each, __single_range<map> >;
};

template <class _InputIterator,
class _Compare   = std::less<__iter_key_type<_InputIterator>>,
class _Allocator = std::allocator<__iter_to_alloc_type<_InputIterator>>,
class            = std::enable_if_t<__has_input_iterator_category<_InputIterator>::value, void>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>>
map(_InputIterator, _InputIterator, _Compare = _Compare(), _Allocator = _Allocator())
-> map<__iter_key_type<_InputIterator>, __iter_mapped_type<_InputIterator>, _Compare, _Allocator>;

template < std::ranges::input_range _Range,
class _Compare   = std::less<__range_key_type<_Range>>,
class _Allocator = std::allocator<__range_to_alloc_type<_Range>>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>>
map(std::from_range_t, _Range&&, _Compare = _Compare(), _Allocator = _Allocator())
-> map<__range_key_type<_Range>, __range_mapped_type<_Range>, _Compare, _Allocator>;

template <class _Key,
class _Tp,
class _Compare   = std::less<std::remove_const_t<_Key>>,
class _Allocator = std::allocator<std::pair<const _Key, _Tp>>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>>
map(std::initializer_list<std::pair<_Key, _Tp>>, _Compare = _Compare(), _Allocator = _Allocator())
-> map<std::remove_const_t<_Key>, _Tp, _Compare, _Allocator>;

template <class _InputIterator,
class _Allocator,
class = std::enable_if_t<__has_input_iterator_category<_InputIterator>::value, void>,
class = std::enable_if_t<__is_allocator_v<_Allocator>>>
map(_InputIterator, _InputIterator, _Allocator)
-> map<__iter_key_type<_InputIterator>,
__iter_mapped_type<_InputIterator>,
std::less<__iter_key_type<_InputIterator>>,
_Allocator>;

template < std::ranges::input_range _Range, class _Allocator, class = std::enable_if_t<__is_allocator_v<_Allocator>>>
map(std::from_range_t, _Range&&, _Allocator)
-> map<__range_key_type<_Range>, __range_mapped_type<_Range>, std::less<__range_key_type<_Range>>, _Allocator>;

template <class _Key, class _Tp, class _Allocator, class = std::enable_if_t<__is_allocator_v<_Allocator>>>
map(std::initializer_list<std::pair<_Key, _Tp>>, _Allocator)
-> map<std::remove_const_t<_Key>, _Tp, std::less<std::remove_const_t<_Key>>, _Allocator>;

template <class _Key, class _Tp, class _Compare, class _Allocator>
struct __specialized_algorithm<_Algorithm::__for_each, __single_range<map<_Key, _Tp, _Compare, _Allocator>>> {
    using __map MSTD_NODEBUG = map<_Key, _Tp, _Compare, _Allocator>;

    static const bool __has_algorithm = true;

    template <class _Map, class _Func, class _Proj>
    MSTD_HIDE_FROM_ABI static auto operator()(_Map&& __map, _Func __func, _Proj __proj) {
        auto [_, __func2] = __specialized_algorithm<_Algorithm::__for_each, __single_range<typename __map::__base>>()(
            __map.__tree_, std::move(__func), std::move(__proj));
        return std::make_pair(__map.end(), std::move(__func2));
    }
};

template <class _Key, class _Tp, class _Compare, class _Allocator>
_Tp& map<_Key, _Tp, _Compare, _Allocator>::operator[](const key_type& __k) {
    return __tree_.__emplace_unique(std::piecewise_construct, std::forward_as_tuple(__k), std::forward_as_tuple())
    .first->second;
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
_Tp& map<_Key, _Tp, _Compare, _Allocator>::operator[](key_type&& __k) {
    return __tree_
    .__emplace_unique(std::piecewise_construct, std::forward_as_tuple(std::move(__k)), std::forward_as_tuple())
    .first->second;
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
_Tp& map<_Key, _Tp, _Compare, _Allocator>::at(const key_type& __k) {
    auto [_, __child] = __tree_.__find_equal(__k);
    if (__child == nullptr)
        std::__throw_out_of_range("map::at:  key not found");
    return static_cast<__node_pointer>(__child)->__get_value().second;
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
const _Tp& map<_Key, _Tp, _Compare, _Allocator>::at(const key_type& __k) const {
    auto [_, __child] = __tree_.__find_equal(__k);
    if (__child == nullptr)
        std::__throw_out_of_range("map::at:  key not found");
    return static_cast<__node_pointer>(__child)->__get_value().second;
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
inline MSTD_HIDE_FROM_ABI bool
operator==(const map<_Key, _Tp, _Compare, _Allocator>& __x, const map<_Key, _Tp, _Compare, _Allocator>& __y) {
    return __x.size() == __y.size() && std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
MSTD_HIDE_FROM_ABI __synth_three_way_result<std::pair<const _Key, _Tp>>
operator<=>(const map<_Key, _Tp, _Compare, _Allocator>& __x, const map<_Key, _Tp, _Compare, _Allocator>& __y) {
    return std::lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), mstd::__synth_three_way);
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
inline MSTD_HIDE_FROM_ABI void
swap(map<_Key, _Tp, _Compare, _Allocator>& __x, map<_Key, _Tp, _Compare, _Allocator>& __y)
noexcept(noexcept(__x.swap(__y))) {
    __x.swap(__y);
}

template <class _Key, class _Tp, class _Compare, class _Allocator, class _Predicate>
inline MSTD_HIDE_FROM_ABI typename map<_Key, _Tp, _Compare, _Allocator>::size_type
erase_if(map<_Key, _Tp, _Compare, _Allocator>& __c, _Predicate __pred) {
    return mstd::_MSTD_erase_if_container(__c, __pred);
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
struct __container_traits<map<_Key, _Tp, _Compare, _Allocator> > {
    // http://eel.is/c++draft/associative.reqmts.except#2
    // For associative containers, if an exception is thrown by any operation from within
    // an insert or emplace function inserting a single element, the insertion has no effect.
    static constexpr const bool __emplacement_has_strong_exception_safety_guarantee = true;

    static constexpr const bool __reservable = false;
};

template <class _Key, class _Tp, class _Compare, class _Allocator>
class multimap {
public:
    // types:
    typedef _Key key_type;
    typedef _Tp mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef std::type_identity_t<_Compare> key_compare;
    typedef std::type_identity_t<_Allocator> allocator_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    static_assert(__check_valid_allocator<allocator_type>::value, "");
    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>,
                  "Allocator::value_type must be same type as value_type");

    class value_compare : public std::binary_function<value_type, value_type, bool> {
        friend class multimap;

    protected:
        key_compare comp;

        MSTD_HIDE_FROM_ABI value_compare(key_compare __c) : comp(__c) {}

    public:
        MSTD_HIDE_FROM_ABI bool operator()(const value_type& __x, const value_type& __y) const {
            return comp(__x.first, __y.first);
        }
    };

private:
    typedef __value_type<key_type, mapped_type> __value_type;
    typedef __map_value_compare<key_type, value_type, key_compare> __vc;
    typedef __tree<__value_type, __vc, allocator_type> __base;
    typedef typename __base::__node_traits __node_traits;
    typedef allocator_traits<allocator_type> __alloc_traits;

    __base __tree_;

public:
    typedef typename __alloc_traits::pointer pointer;
    typedef typename __alloc_traits::const_pointer const_pointer;
    typedef typename __alloc_traits::size_type size_type;
    typedef typename __alloc_traits::difference_type difference_type;
    typedef __map_iterator<typename __base::iterator> iterator;
    typedef __map_const_iterator<typename __base::const_iterator> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef __map_node_handle<typename __base::__node, allocator_type> node_type;

    template <class _Key2, class _Value2, class _Comp2, class _Alloc2>
    friend class map;
    template <class _Key2, class _Value2, class _Comp2, class _Alloc2>
    friend class multimap;

    MSTD_HIDE_FROM_ABI multimap() noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value && std::is_nothrow_default_constructible<key_compare>::value&&
        std::is_nothrow_copy_constructible<key_compare>::value)
    : __tree_(__vc(key_compare())) {}

    MSTD_HIDE_FROM_ABI explicit multimap(const key_compare& __comp) noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value && std::is_nothrow_copy_constructible<key_compare>::value)
    : __tree_(__vc(__comp)) {}

    MSTD_HIDE_FROM_ABI explicit multimap(const key_compare& __comp, const allocator_type& __a)
    : __tree_(__vc(__comp), typename __base::allocator_type(__a)) {}

    template <class _InputIterator>
    MSTD_HIDE_FROM_ABI multimap(_InputIterator __f, _InputIterator __l, const key_compare& __comp = key_compare())
    : __tree_(__vc(__comp)) {
        insert(__f, __l);
    }

    template <class _InputIterator>
    MSTD_HIDE_FROM_ABI
    multimap(_InputIterator __f, _InputIterator __l, const key_compare& __comp, const allocator_type& __a)
    : __tree_(__vc(__comp), typename __base::allocator_type(__a)) {
        insert(__f, __l);
    }

    template <_ContainerCompatibleRange<value_type> _Range>
    MSTD_HIDE_FROM_ABI
    multimap(std::from_range_t,
             _Range&& __range,
             const key_compare& __comp = key_compare(),
             const allocator_type& __a = allocator_type())
    : __tree_(__vc(__comp), typename __base::allocator_type(__a)) {
        insert_range(std::forward<_Range>(__range));
    }

    template <class _InputIterator>
    MSTD_HIDE_FROM_ABI multimap(_InputIterator __f, _InputIterator __l, const allocator_type& __a)
    : multimap(__f, __l, key_compare(), __a) {}
    
    template <_ContainerCompatibleRange<value_type> _Range>
    MSTD_HIDE_FROM_ABI multimap(std::from_range_t, _Range&& __range, const allocator_type& __a)
    : multimap(std::from_range, std::forward<_Range>(__range), key_compare(), __a) {}
    
    MSTD_HIDE_FROM_ABI multimap(const multimap& __m) = default;

    MSTD_HIDE_FROM_ABI multimap& operator=(const multimap& __m) = default;

    MSTD_HIDE_FROM_ABI multimap(multimap&& __m) = default;

    MSTD_HIDE_FROM_ABI multimap(multimap&& __m, const allocator_type& __a) : __tree_(std::move(__m.__tree_), __a) {}

    MSTD_HIDE_FROM_ABI multimap& operator=(multimap&& __m) = default;

    MSTD_HIDE_FROM_ABI multimap(std::initializer_list<value_type> __il, const key_compare& __comp = key_compare())
    : __tree_(__vc(__comp)) {
        insert(__il.begin(), __il.end());
    }

    MSTD_HIDE_FROM_ABI
    multimap(std::initializer_list<value_type> __il, const key_compare& __comp, const allocator_type& __a)
    : __tree_(__vc(__comp), typename __base::allocator_type(__a)) {
        insert(__il.begin(), __il.end());
    }

    MSTD_HIDE_FROM_ABI multimap(std::initializer_list<value_type> __il, const allocator_type& __a)
    : multimap(__il, key_compare(), __a) {}
    
    MSTD_HIDE_FROM_ABI multimap& operator=(std::initializer_list<value_type> __il) {
        clear();
        insert(__il.begin(), __il.end());
        return *this;
    }

    MSTD_HIDE_FROM_ABI explicit multimap(const allocator_type& __a) : __tree_(typename __base::allocator_type(__a)) {}

    MSTD_HIDE_FROM_ABI multimap(const multimap& __m, const allocator_type& __a) : __tree_(__m.__tree_, __a) {}

    MSTD_HIDE_FROM_ABI ~multimap() {
        static_assert(sizeof(mstd::__diagnose_non_const_comparator<_Key, _Compare>()), "");
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator begin() noexcept { return __tree_.begin(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator begin() const noexcept { return __tree_.begin(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator end() noexcept { return __tree_.end(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator end() const noexcept { return __tree_.end(); }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator cbegin() const noexcept { return begin(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator cend() const noexcept { return end(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_reverse_iterator crend() const noexcept { return rend(); }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI bool empty() const noexcept { return __tree_.size() == 0; }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI size_type size() const noexcept { return __tree_.size(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI size_type max_size() const noexcept { return __tree_.max_size(); }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI allocator_type get_allocator() const noexcept {
        return allocator_type(__tree_.__alloc());
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI key_compare key_comp() const { return __tree_.value_comp().key_comp(); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI value_compare value_comp() const {
        return value_compare(__tree_.value_comp().key_comp());
    }

    template <class... _Args>
    MSTD_HIDE_FROM_ABI iterator emplace(_Args&&... __args) {
        return __tree_.__emplace_multi(std::forward<_Args>(__args)...);
    }

    template <class... _Args>
    MSTD_HIDE_FROM_ABI iterator emplace_hint(const_iterator __p, _Args&&... __args) {
        return __tree_.__emplace_hint_multi(__p.__i_, std::forward<_Args>(__args)...);
    }

    template <class _Pp, std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0>
    MSTD_HIDE_FROM_ABI iterator insert(_Pp&& __p) {
        return __tree_.__emplace_multi(std::forward<_Pp>(__p));
    }

    template <class _Pp, std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0>
    MSTD_HIDE_FROM_ABI iterator insert(const_iterator __pos, _Pp&& __p) {
        return __tree_.__emplace_hint_multi(__pos.__i_, std::forward<_Pp>(__p));
    }

    MSTD_HIDE_FROM_ABI iterator insert(value_type&& __v) { return __tree_.__emplace_multi(std::move(__v)); }

    MSTD_HIDE_FROM_ABI iterator insert(const_iterator __p, value_type&& __v) {
        return __tree_.__emplace_hint_multi(__p.__i_, std::move(__v));
    }

    MSTD_HIDE_FROM_ABI void insert(std::initializer_list<value_type> __il) { insert(__il.begin(), __il.end()); }

    MSTD_HIDE_FROM_ABI iterator insert(const value_type& __v) { return __tree_.__emplace_multi(__v); }

    MSTD_HIDE_FROM_ABI iterator insert(const_iterator __p, const value_type& __v) {
        return __tree_.__emplace_hint_multi(__p.__i_, __v);
    }

    template <class _InputIterator>
    MSTD_HIDE_FROM_ABI void insert(_InputIterator __f, _InputIterator __l) {
        __tree_.__insert_range_multi(__f, __l);
    }

    template <_ContainerCompatibleRange<value_type> _Range>
    MSTD_HIDE_FROM_ABI void insert_range(_Range&& __range) {
        __tree_.__insert_range_multi( std::ranges::begin(__range),  std::ranges::end(__range));
    }

    MSTD_HIDE_FROM_ABI iterator erase(const_iterator __p) { return __tree_.erase(__p.__i_); }
    MSTD_HIDE_FROM_ABI iterator erase(iterator __p) { return __tree_.erase(__p.__i_); }
    MSTD_HIDE_FROM_ABI size_type erase(const key_type& __k) { return __tree_.__erase_multi(__k); }
    MSTD_HIDE_FROM_ABI iterator erase(const_iterator __f, const_iterator __l) {
        return __tree_.erase(__f.__i_, __l.__i_);
    }

    MSTD_HIDE_FROM_ABI iterator insert(node_type&& __nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(__nh.empty() || __nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to multimap::insert()");
        return __tree_.template __node_handle_insert_multi<node_type>(std::move(__nh));
    }
    MSTD_HIDE_FROM_ABI iterator insert(const_iterator __hint, node_type&& __nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(__nh.empty() || __nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to multimap::insert()");
        return __tree_.template __node_handle_insert_multi<node_type>(__hint.__i_, std::move(__nh));
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI node_type extract(key_type const& __key) {
        return __tree_.template __node_handle_extract<node_type>(__key);
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI node_type extract(const_iterator __it) {
        return __tree_.template __node_handle_extract<node_type>(__it.__i_);
    }
    template <class _Compare2>
    MSTD_HIDE_FROM_ABI void merge(multimap<key_type, mapped_type, _Compare2, allocator_type>& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        return __tree_.__node_handle_merge_multi(__source.__tree_);
    }
    template <class _Compare2>
    MSTD_HIDE_FROM_ABI void merge(multimap<key_type, mapped_type, _Compare2, allocator_type>&& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        return __tree_.__node_handle_merge_multi(__source.__tree_);
    }
    template <class _Compare2>
    MSTD_HIDE_FROM_ABI void merge(map<key_type, mapped_type, _Compare2, allocator_type>& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        return __tree_.__node_handle_merge_multi(__source.__tree_);
    }
    template <class _Compare2>
    MSTD_HIDE_FROM_ABI void merge(map<key_type, mapped_type, _Compare2, allocator_type>&& __source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            __source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        return __tree_.__node_handle_merge_multi(__source.__tree_);
    }

    MSTD_HIDE_FROM_ABI void clear() noexcept { __tree_.clear(); }

    MSTD_HIDE_FROM_ABI void swap(multimap& __m) noexcept(std::is_nothrow_swappable_v<__base>) {
        __tree_.swap(__m.__tree_);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator find(const key_type& __k) { return __tree_.find(__k); }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator find(const key_type& __k) const { return __tree_.find(__k); }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator find(const _K2& __k) {
        return __tree_.find(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator find(const _K2& __k) const {
        return __tree_.find(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI size_type count(const key_type& __k) const {
        return __tree_.__count_multi(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI size_type count(const _K2& __k) const {
        return __tree_.__count_multi(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI bool contains(const key_type& __k) const { return find(__k) != end(); }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI bool contains(const _K2& __k) const {
        return find(__k) != end();
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator lower_bound(const key_type& __k) {
        return __tree_.__lower_bound_multi(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator lower_bound(const key_type& __k) const {
        return __tree_.__lower_bound_multi(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator lower_bound(const _K2& __k) {
        return __tree_.__lower_bound_multi(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator lower_bound(const _K2& __k) const {
        return __tree_.__lower_bound_multi(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator upper_bound(const key_type& __k) {
        return __tree_.__upper_bound_multi(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator upper_bound(const key_type& __k) const {
        return __tree_.__upper_bound_multi(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI iterator upper_bound(const _K2& __k) {
        return __tree_.__upper_bound_multi(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI const_iterator upper_bound(const _K2& __k) const {
        return __tree_.__upper_bound_multi(__k);
    }

    [[__nodiscard__]] MSTD_HIDE_FROM_ABI std::pair<iterator, iterator> equal_range(const key_type& __k) {
        return __tree_.__equal_range_multi(__k);
    }
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI std::pair<const_iterator, const_iterator> equal_range(const key_type& __k) const {
        return __tree_.__equal_range_multi(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI std::pair<iterator, iterator> equal_range(const _K2& __k) {
        return __tree_.__equal_range_multi(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[__nodiscard__]] MSTD_HIDE_FROM_ABI std::pair<const_iterator, const_iterator> equal_range(const _K2& __k) const {
        return __tree_.__equal_range_multi(__k);
    }

private:
    typedef typename __base::__node __node;
    typedef typename __base::__node_allocator __node_allocator;
    typedef typename __base::__node_pointer __node_pointer;

    typedef __map_node_destructor<__node_allocator> _Dp;
    typedef std::unique_ptr<__node, _Dp> __node_holder;

    friend struct __specialized_algorithm<_Algorithm::__for_each, __single_range<multimap> >;
};

template <class _InputIterator,
class _Compare   = std::less<__iter_key_type<_InputIterator>>,
class _Allocator = std::allocator<__iter_to_alloc_type<_InputIterator>>,
class            = std::enable_if_t<__has_input_iterator_category<_InputIterator>::value, void>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>>
multimap(_InputIterator, _InputIterator, _Compare = _Compare(), _Allocator = _Allocator())
-> multimap<__iter_key_type<_InputIterator>, __iter_mapped_type<_InputIterator>, _Compare, _Allocator>;

template < std::ranges::input_range _Range,
class _Compare   = std::less<__range_key_type<_Range>>,
class _Allocator = std::allocator<__range_to_alloc_type<_Range>>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>>
multimap(std::from_range_t, _Range&&, _Compare = _Compare(), _Allocator = _Allocator())
-> multimap<__range_key_type<_Range>, __range_mapped_type<_Range>, _Compare, _Allocator>;

template <class _Key,
class _Tp,
class _Compare   = std::less<std::remove_const_t<_Key>>,
class _Allocator = std::allocator<std::pair<const _Key, _Tp>>,
class            = std::enable_if_t<!__is_allocator_v<_Compare>>,
class            = std::enable_if_t<__is_allocator_v<_Allocator>>>
multimap(std::initializer_list<std::pair<_Key, _Tp>>, _Compare = _Compare(), _Allocator = _Allocator())
-> multimap<std::remove_const_t<_Key>, _Tp, _Compare, _Allocator>;

template <class _InputIterator,
class _Allocator,
class = std::enable_if_t<__has_input_iterator_category<_InputIterator>::value, void>,
class = std::enable_if_t<__is_allocator_v<_Allocator>>>
multimap(_InputIterator, _InputIterator, _Allocator)
-> multimap<__iter_key_type<_InputIterator>,
__iter_mapped_type<_InputIterator>,
std::less<__iter_key_type<_InputIterator>>,
_Allocator>;

template < std::ranges::input_range _Range, class _Allocator, class = std::enable_if_t<__is_allocator_v<_Allocator>>>
multimap(std::from_range_t, _Range&&, _Allocator)
-> multimap<__range_key_type<_Range>, __range_mapped_type<_Range>, std::less<__range_key_type<_Range>>, _Allocator>;

template <class _Key, class _Tp, class _Allocator, class = std::enable_if_t<__is_allocator_v<_Allocator>>>
multimap(std::initializer_list<std::pair<_Key, _Tp>>, _Allocator)
-> multimap<std::remove_const_t<_Key>, _Tp, std::less<std::remove_const_t<_Key>>, _Allocator>;

template <class _Key, class _Tp, class _Compare, class _Allocator>
struct __specialized_algorithm<_Algorithm::__for_each, __single_range<multimap<_Key, _Tp, _Compare, _Allocator>>> {
    using __map MSTD_NODEBUG = multimap<_Key, _Tp, _Compare, _Allocator>;

    static const bool __has_algorithm = true;

    template <class _Map, class _Func, class _Proj>
    MSTD_HIDE_FROM_ABI static auto operator()(_Map&& __map, _Func __func, _Proj __proj) {
        auto [_, __func2] = __specialized_algorithm<_Algorithm::__for_each, __single_range<typename __map::__base>>()(
            __map.__tree_, std::move(__func), std::move(__proj));
        return std::make_pair(__map.end(), std::move(__func2));
    }
};

template <class _Key, class _Tp, class _Compare, class _Allocator>
inline MSTD_HIDE_FROM_ABI bool
operator==(const multimap<_Key, _Tp, _Compare, _Allocator>& __x, const multimap<_Key, _Tp, _Compare, _Allocator>& __y) {
    return __x.size() == __y.size() && std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
MSTD_HIDE_FROM_ABI __synth_three_way_result<std::pair<const _Key, _Tp>>
operator<=>(const multimap<_Key, _Tp, _Compare, _Allocator>& __x,
            const multimap<_Key, _Tp, _Compare, _Allocator>& __y) {
    return std::lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), __synth_three_way);
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
inline MSTD_HIDE_FROM_ABI void
swap(multimap<_Key, _Tp, _Compare, _Allocator>& __x, multimap<_Key, _Tp, _Compare, _Allocator>& __y)
noexcept(noexcept(__x.swap(__y))) {
    __x.swap(__y);
}

template <class _Key, class _Tp, class _Compare, class _Allocator, class _Predicate>
inline MSTD_HIDE_FROM_ABI typename multimap<_Key, _Tp, _Compare, _Allocator>::size_type
erase_if(multimap<_Key, _Tp, _Compare, _Allocator>& __c, _Predicate __pred) {
    return mstd::_MSTD_erase_if_container(__c, __pred);
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
struct __container_traits<multimap<_Key, _Tp, _Compare, _Allocator> > {
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
    using map MSTD_AVAILABILITY_PMR =
    mstd::map<_KeyT, _ValueT, _CompareT, std::pmr::polymorphic_allocator<std::pair<const _KeyT, _ValueT>>>;
    template <class _KeyT, class _ValueT, class _CompareT = std::less<_KeyT>>
    using multimap MSTD_AVAILABILITY_PMR =
    mstd::multimap<_KeyT, _ValueT, _CompareT, std::pmr::polymorphic_allocator<std::pair<const _KeyT, _ValueT>>>;
} // namespace pmr
} // namespace mstd


#endif // MSTD_MAP
