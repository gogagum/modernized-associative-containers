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

namespace mstd {

template <class _Key, class _CP, class _Compare>
class MapValueCompare {
    MSTD_COMPRESSED_ELEMENT(_Compare, comp_);

public:
    MapValueCompare() noexcept(std::is_nothrow_default_constructible<_Compare>::value)
    : comp_() {}
    MapValueCompare(_Compare c) noexcept(std::is_nothrow_copy_constructible<_Compare>::value)
    : comp_(c) {}
    const _Compare& key_comp() const noexcept { return comp_; }

    bool operator()(const _CP& x, const _CP& y) const { return comp_(x.first, y.first); }
    bool operator()(const _CP& x, const _Key& y) const { return comp_(x.first, y); }
    bool operator()(const _Key& x, const _CP& y) const { return comp_(x, y.first); }
    void swap(MapValueCompare& y) noexcept(std::is_nothrow_swappable_v<_Compare>) { std::swap(comp_, y.comp_); }

    template <typename _K2>
    bool operator()(const _K2& x, const _CP& y) const { return comp_(x, y.first); }

    template <typename _K2>
    bool operator()(const _CP& x, const _K2& y) const { return comp_(x.first, y); }
};

template <class _Key, class _MapValueT, class _Compare>
struct __make_transparent<MapValueCompare<_Key, _MapValueT, _Compare> > {
    using type = MapValueCompare<_Key, _MapValueT, __make_transparent_t<_Compare> >;
};

template <class _MapValueT, class _Key, class _Compare>
struct LazySynthThreeWayComparator<MapValueCompare<_Key, _MapValueT, _Compare>, _MapValueT, _MapValueT> {
    LazySynthThreeWayComparator<_Compare, _Key, _Key> comp_;

    LazySynthThreeWayComparator(const MapValueCompare<_Key, _MapValueT, _Compare>& comp)
    : comp_(comp.key_comp()) {}

    auto operator()(const _MapValueT& __lhs, const _MapValueT& __rhs) const {
        return comp_(__lhs.first, __rhs.first);
    }
};

template <class _MapValueT, class _Key, class _TransparentKey, class _Compare>
struct LazySynthThreeWayComparator<MapValueCompare<_Key, _MapValueT, _Compare>, _TransparentKey, _MapValueT> {
    LazySynthThreeWayComparator<_Compare, _TransparentKey, _Key> comp_;

    LazySynthThreeWayComparator(const MapValueCompare<_Key, _MapValueT, _Compare>& comp)
    : comp_(comp.key_comp()) {}

    auto operator()(const _TransparentKey& __lhs, const _MapValueT& __rhs) const {
        return comp_(__lhs, __rhs.first);
    }
};

template <class _MapValueT, class _Key, class _TransparentKey, class _Compare>
struct LazySynthThreeWayComparator<MapValueCompare<_Key, _MapValueT, _Compare>, _MapValueT, _TransparentKey> {
    LazySynthThreeWayComparator<_Compare, _Key, _TransparentKey> comp_;

    LazySynthThreeWayComparator(const MapValueCompare<_Key, _MapValueT, _Compare>& comp)
    : comp_(comp.key_comp()) {}

    auto operator()(const _MapValueT& __lhs, const _TransparentKey& __rhs) const {
        return comp_(__lhs.first, __rhs);
    }
};

template <class _Key, class _CP, class _Compare>
inline void
swap(MapValueCompare<_Key, _CP, _Compare>& x, MapValueCompare<_Key, _CP, _Compare>& y)
noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

template <class _Allocator>
class MapNodeDestructor {
    typedef _Allocator allocator_type;
    typedef allocator_traits<allocator_type> AllocTraits_;

public:
    typedef typename AllocTraits_::pointer pointer;

private:
    allocator_type& na_;

public:
    bool first_constructed;
    bool second_constructed;

    explicit MapNodeDestructor(allocator_type& __na) noexcept
    : na_(__na),
      first_constructed(false),
      second_constructed(false) {}

    MapNodeDestructor(__tree_node_destructor<allocator_type>&& x) noexcept
    : na_(x.na_),
      first_constructed(x.__value_constructed),
      second_constructed(x.__value_constructed) {
        x.__value_constructed = false;
    }

    MapNodeDestructor& operator=(const MapNodeDestructor&) = delete;

    void operator()(pointer __p) noexcept {
        if (second_constructed)
            AllocTraits_::destroy(na_, std::addressof(__p->__get_value().second));
        if (first_constructed)
            AllocTraits_::destroy(na_, std::addressof(__p->__get_value().first));
        if (__p)
            AllocTraits_::deallocate(na_, __p, 1);
    }
};

template <class _Key, class _Tp>
struct ValueType;

template <class _TreeIterator>
class MapIterator {
    _TreeIterator i_;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = typename _TreeIterator::value_type;
    using difference_type   = typename _TreeIterator::difference_type;
    using reference         = value_type&;
    using pointer           = typename _TreeIterator::pointer;

    MapIterator() noexcept {}

    MapIterator(_TreeIterator __i) noexcept : i_(__i) {}

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

template <class _Alg, class _TreeIterator>
struct __specialized_algorithm<_Alg, __iterator_pair<MapIterator<_TreeIterator>, MapIterator<_TreeIterator>>> {
    using Tree_ = __specialized_algorithm<_Alg, __iterator_pair<_TreeIterator, _TreeIterator>>;

    static const bool __has_algorithm = Tree_::__has_algorithm;

    using Iterator_ = MapIterator<_TreeIterator>;

    template <class... _Args>
    static void operator()(Iterator_ first, Iterator_ last, _Args&&... __args) {
        Tree_()(first.i_, last.i_, std::forward<_Args>(__args)...);
    }
};

template <class _TreeIterator>
class MapConstIterator {
    _TreeIterator i_;

public:
    using iterator_category = bidirectional_iterator_tag;
    using value_type        = typename _TreeIterator::value_type;
    using difference_type   = typename _TreeIterator::difference_type;
    using reference         = const value_type&;
    using pointer           = typename _TreeIterator::pointer;

    MapConstIterator() noexcept {}

    MapConstIterator(_TreeIterator i) noexcept : i_(i) {}
    MapConstIterator(MapIterator< typename _TreeIterator::__non_const_iterator> i) noexcept : i_(i.i_) {}

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

template <class _Alg, class _TreeIterator>
struct __specialized_algorithm<_Alg, __iterator_pair<MapConstIterator<_TreeIterator>, MapConstIterator<_TreeIterator>>> {
    using Tree_ = __specialized_algorithm<_Alg, __iterator_pair<_TreeIterator, _TreeIterator>>;

    static const bool __has_algorithm = Tree_::__has_algorithm;

    using Iterator_ = MapConstIterator<_TreeIterator>;

    template <class... _Args>
    static void operator()(Iterator_ first, Iterator_ last, _Args&&... args) {
        Tree_()(first.i_, last.i_, std::forward<_Args>(args)...);
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

        value_compare(key_compare c) : comp(c) {}

    public:
        bool operator()(const value_type& x, const value_type& y) const {
            return comp(x.first, y.first);
        }
    };

private:
    using ValueType_ = ValueType<key_type, mapped_type>;
    typedef MapValueCompare<key_type, value_type, key_compare> __vc;
    using Tree_ = Tree<ValueType_, __vc, allocator_type>;
    typedef typename Tree_::__node_traits __node_traits;
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
    : tree_(__vc(key_compare())) {}

    explicit map(const key_compare& comp) noexcept(
           std::is_nothrow_default_constructible<allocator_type>::value 
        && std::is_nothrow_copy_constructible<key_compare>::value)
    : tree_(__vc(comp)) {}

    explicit map(const key_compare& comp, const allocator_type& __a)
    : tree_(__vc(comp), typename Tree_::allocator_type(__a)) {}

    template <class _InputIterator>
    map(_InputIterator __f, _InputIterator __l, const key_compare& comp = key_compare())
    : tree_(__vc(comp)) {
        insert(__f, __l);
    }

    template <class _InputIterator>
    map(_InputIterator __f, _InputIterator __l, const key_compare& comp, const allocator_type& __a)
    : tree_(__vc(comp), typename Tree_::allocator_type(__a)) {
        insert(__f, __l);
    }

    template <_ContainerCompatibleRange<value_type> _Range>
    map(std::from_range_t,
        _Range&& __range,
        const key_compare& comp = key_compare(),
        const allocator_type& __a = allocator_type())
    : tree_(__vc(comp), typename Tree_::allocator_type(__a)) {
        insert_range(std::forward<_Range>(__range));
    }
    
    template <class _InputIterator>
    map(_InputIterator __f, _InputIterator __l, const allocator_type& alloc)
    : map(__f, __l, key_compare(), alloc) {}

    template <_ContainerCompatibleRange<value_type> _Range>
    map(std::from_range_t, _Range&& __range, const allocator_type& alloc)
    : map(std::from_range, std::forward<_Range>(__range), key_compare(), alloc) {}

    map(const map& __m) = default;

    map& operator=(const map& __m) = default;

    map(map&& m) = default;

    map(map&& m, const allocator_type& alloc) : tree_(std::move(m.tree_), alloc) {}

    map& operator=(map&& __m) = default;

    map(std::initializer_list<value_type> init_list, const key_compare& comp = key_compare())
    : tree_(__vc(comp)) {
        insert(init_list.begin(), init_list.end());
    }

    map(std::initializer_list<value_type> init_list, const key_compare& comp, const allocator_type& __a)
    : tree_(__vc(comp), typename Tree_::allocator_type(__a)) {
        insert(init_list.begin(), init_list.end());
    }

    map(std::initializer_list<value_type> init_list, const allocator_type& __a)
    : map(init_list, key_compare(), __a) {}

    map& operator=(std::initializer_list<value_type> init_list) {
        clear();
        insert(init_list.begin(), init_list.end());
        return *this;
    }

    explicit map(const allocator_type& __a) : tree_(typename Tree_::allocator_type(__a)) {}

    map(const map& __m, const allocator_type& __alloc) : tree_(__m.tree_, __alloc) {}

    ~map() { static_assert(sizeof(mstd::__diagnose_non_const_comparator<_Key, _Compare>()), ""); }

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

    mapped_type& operator[](const key_type& __k);
    mapped_type& operator[](key_type&& __k);

    template <class _Arg,
    std::enable_if_t<__is_transparently_comparable_v<_Compare, key_type, std::remove_cvref_t<_Arg> >, int> = 0>
    [[nodiscard]] mapped_type& at(_Arg&& __arg) {
        auto [_, child] = tree_.__find_equal(__arg);
        if (child == nullptr)
            std::__throw_out_of_range("map::at:  key not found");
        return static_cast<__node_pointer>(child)->__get_value().second;
    }

    template <class _Arg,
    std::enable_if_t<__is_transparently_comparable_v<_Compare, key_type, std::remove_cvref_t<_Arg> >, int> = 0>
    [[nodiscard]] const mapped_type& at(_Arg&& __arg) const {
        auto [_, child] = tree_.__find_equal(__arg);
        if (child == nullptr)
            std::__throw_out_of_range("map::at:  key not found");
        return static_cast<__node_pointer>(child)->__get_value().second;
    }

    [[nodiscard]] mapped_type& at(const key_type& __k);
    [[nodiscard]] const mapped_type& at(const key_type& __k) const;

    [[nodiscard]] allocator_type get_allocator() const noexcept {
        return allocator_type(tree_.__alloc());
    }
    [[nodiscard]] key_compare key_comp() const { return tree_.value_comp().key_comp(); }
    [[nodiscard]] value_compare value_comp() const {
        return value_compare(tree_.value_comp().key_comp());
    }

    template <class... _Args>
    std::pair<iterator, bool> emplace(_Args&&... __args) {
        return tree_.__emplace_unique(std::forward<_Args>(__args)...);
    }

    template <class... _Args>
    iterator emplace_hint(const_iterator __p, _Args&&... __args) {
        return tree_.__emplace_hint_unique(__p.i_, std::forward<_Args>(__args)...).first;
    }

    template <class _Pp, std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0>
    std::pair<iterator, bool> insert(_Pp&& __p) {
        return tree_.__emplace_unique(std::forward<_Pp>(__p));
    }

    template <class _Pp, std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0>
    iterator insert(const_iterator __pos, _Pp&& __p) {
        return tree_.__emplace_hint_unique(__pos.i_, std::forward<_Pp>(__p)).first;
    }

    std::pair<iterator, bool> insert(const value_type& __v) { return tree_.__emplace_unique(__v); }

    iterator insert(const_iterator __p, const value_type& __v) {
        return tree_.__emplace_hint_unique(__p.i_, __v).first;
    }

    std::pair<iterator, bool> insert(value_type&& __v) {
        return tree_.__emplace_unique(std::move(__v));
    }

    iterator insert(const_iterator __p, value_type&& __v) {
        return tree_.__emplace_hint_unique(__p.i_, std::move(__v)).first;
    }

    void insert(std::initializer_list<value_type> init_list) { insert(init_list.begin(), init_list.end()); }

    template <class _InputIterator>
    void insert(_InputIterator first, _InputIterator last) {
        tree_.__insert_range_unique(first, last);
    }

    template <_ContainerCompatibleRange<value_type> _Range>
    void insert_range(_Range&& __range) {
        tree_.__insert_range_unique(
            std::ranges::begin(__range),
            std::ranges::end(__range)
        );
    }

    template <class... _Args>
    std::pair<iterator, bool> try_emplace(const key_type& k, _Args&&... args) {
        return tree_.__emplace_unique(
            std::piecewise_construct,
            std::forward_as_tuple(k),
            std::forward_as_tuple(std::forward<_Args>(args)...)
        );
    }

    template <class... _Args>
    std::pair<iterator, bool> try_emplace(key_type&& k, _Args&&... args) {
        return tree_.__emplace_unique(
            std::piecewise_construct,
            std::forward_as_tuple(std::move(k)),
            std::forward_as_tuple(std::forward<_Args>(args)...)
        );
    }

    template <class... _Args>
    iterator try_emplace(const_iterator hint, const key_type& k, _Args&&... args) {
        return tree_.__emplace_hint_unique(
            hint.i_,
            std::piecewise_construct,
            std::forward_as_tuple(k),
            std::forward_as_tuple(std::forward<_Args>(args)...)
        ).first;
    }

    template <class... _Args>
    iterator try_emplace(const_iterator hint, key_type&& k, _Args&&... args) {
        return tree_.__emplace_hint_unique(
            hint.i_,
            std::piecewise_construct,
            std::forward_as_tuple(std::move(k)),
            std::forward_as_tuple(std::forward<_Args>(args)...)
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

    [[nodiscard]] iterator find(const key_type& __k) { return tree_.find(__k); }
    [[nodiscard]] const_iterator find(const key_type& __k) const { return tree_.find(__k); }
    template <typename _K2,
    std::enable_if_t<__is_transparent_v<_Compare, _K2> || __is_transparently_comparable_v<_Compare, key_type, _K2>,
    int> = 0>
    [[nodiscard]] iterator find(const _K2& __k) {
        return tree_.find(__k);
    }
    template <typename _K2,
    std::enable_if_t<__is_transparent_v<_Compare, _K2> || __is_transparently_comparable_v<_Compare, key_type, _K2>,
    int> = 0>
    [[nodiscard]] const_iterator find(const _K2& __k) const {
        return tree_.find(__k);
    }

    [[nodiscard]] size_type count(const key_type& __k) const {
        return tree_.__count_unique(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] size_type count(const _K2& __k) const {
        return tree_.__count_multi(__k);
    }

    [[nodiscard]] bool contains(const key_type& __k) const { return find(__k) != end(); }
    template <typename _K2,
    std::enable_if_t<__is_transparent_v<_Compare, _K2> || __is_transparently_comparable_v<_Compare, key_type, _K2>,
    int> = 0>
    [[nodiscard]] bool contains(const _K2& __k) const {
        return find(__k) != end();
    }

    [[nodiscard]] iterator lower_bound(const key_type& __k) {
        return tree_.__lower_bound_unique(__k);
    }

    [[nodiscard]] const_iterator lower_bound(const key_type& __k) const {
        return tree_.__lower_bound_unique(__k);
    }

    // The transparent versions of the lookup functions use the _multi version, since a non-element key is allowed to
    // match multiple elements.
    template <
        typename _K2
      , std::enable_if_t<
            __is_transparent_v<_Compare, _K2>
         || __is_transparently_comparable_v<_Compare, key_type, _K2>
         ,  int
        > = 0
    >
    [[nodiscard]] iterator lower_bound(const _K2& k) {
        return tree_.__lower_bound_multi(k);
    }

    template <
        typename _K2
      , std::enable_if_t<
            __is_transparent_v<_Compare, _K2>
         || __is_transparently_comparable_v<_Compare, key_type, _K2>
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
            __is_transparent_v<_Compare, _K2>
         || __is_transparently_comparable_v<_Compare, key_type, _K2>
         ,  int
        > = 0
    >
    [[nodiscard]] iterator upper_bound(const _K2& k) {
        return tree_.__upper_bound_multi(k);
    }
    template <
        typename _K2
      , std::enable_if_t<
            __is_transparent_v<_Compare, _K2>
         || __is_transparently_comparable_v<_Compare, key_type, _K2>
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
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] std::pair<iterator, iterator> equal_range(const _K2& k) {
        return tree_.__equal_range_multi(k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
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
    using __map = map<_Key, _Tp, _Compare, _Allocator>;

    static const bool __has_algorithm = true;

    template <class _Map, class _Func, class _Proj>
    static auto operator()(_Map&& __map, _Func __func, _Proj __proj) {
        auto [_, __func2] = __specialized_algorithm<_Algorithm::__for_each, __single_range<typename __map::Tree_>>()(
            __map.tree_, std::move(__func), std::move(__proj));
        return std::make_pair(__map.end(), std::move(__func2));
    }
};

template <class _Key, class _Tp, class _Compare, class _Allocator>
_Tp& map<_Key, _Tp, _Compare, _Allocator>::operator[](const key_type& __k) {
    return tree_.__emplace_unique(std::piecewise_construct, std::forward_as_tuple(__k), std::forward_as_tuple())
    .first->second;
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
_Tp& map<_Key, _Tp, _Compare, _Allocator>::operator[](key_type&& __k) {
    return tree_
    .__emplace_unique(std::piecewise_construct, std::forward_as_tuple(std::move(__k)), std::forward_as_tuple())
    .first->second;
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
_Tp& map<_Key, _Tp, _Compare, _Allocator>::at(const key_type& __k) {
    auto [_, child] = tree_.__find_equal(__k);
    if (child == nullptr)
        std::__throw_out_of_range("map::at:  key not found");
    return static_cast<__node_pointer>(child)->__get_value().second;
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
const _Tp& map<_Key, _Tp, _Compare, _Allocator>::at(const key_type& __k) const {
    auto [_, child] = tree_.__find_equal(__k);
    if (child == nullptr)
        std::__throw_out_of_range("map::at:  key not found");
    return static_cast<__node_pointer>(child)->__get_value().second;
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
inline bool
operator==(const map<_Key, _Tp, _Compare, _Allocator>& x, const map<_Key, _Tp, _Compare, _Allocator>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
__synth_three_way_result<std::pair<const _Key, _Tp>>
operator<=>(const map<_Key, _Tp, _Compare, _Allocator>& x, const map<_Key, _Tp, _Compare, _Allocator>& y) {
    return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(), y.end(), mstd::__synth_three_way);
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
inline void
swap(map<_Key, _Tp, _Compare, _Allocator>& x, map<_Key, _Tp, _Compare, _Allocator>& y)
noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

template <class _Key, class _Tp, class _Compare, class _Allocator, class _Predicate>
inline typename map<_Key, _Tp, _Compare, _Allocator>::size_type
erase_if(map<_Key, _Tp, _Compare, _Allocator>& __c, _Predicate pred) {
    return mstd::_MSTD_erase_if_container(__c, pred);
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

        value_compare(key_compare __c) : comp(__c) {}

    public:
        bool operator()(const value_type& x, const value_type& y) const {
            return comp(x.first, y.first);
        }
    };

private:
    using ValueType_ = ValueType<key_type, mapped_type>;
    typedef MapValueCompare<key_type, value_type, key_compare> __vc;
    using Tree_ = Tree<ValueType_, __vc, allocator_type>;
    typedef typename Tree_::__node_traits __node_traits;
    typedef allocator_traits<allocator_type> AllocTraits_;

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
        std::is_nothrow_default_constructible<allocator_type>::value && std::is_nothrow_default_constructible<key_compare>::value&&
        std::is_nothrow_copy_constructible<key_compare>::value)
    : tree_(__vc(key_compare())) {}

    explicit multimap(const key_compare& comp) noexcept(
        std::is_nothrow_default_constructible<allocator_type>::value && std::is_nothrow_copy_constructible<key_compare>::value)
    : tree_(__vc(comp)) {}

    explicit multimap(const key_compare& comp, const allocator_type& __a)
    : tree_(__vc(comp), typename Tree_::allocator_type(__a)) {}

    template <class _InputIterator>
    multimap(_InputIterator __f, _InputIterator __l, const key_compare& comp = key_compare())
    : tree_(__vc(comp)) {
        insert(__f, __l);
    }

    template <class _InputIterator>
    multimap(_InputIterator __f, _InputIterator __l, const key_compare& comp, const allocator_type& __a)
    : tree_(__vc(comp), typename Tree_::allocator_type(__a)) {
        insert(__f, __l);
    }

    template <_ContainerCompatibleRange<value_type> _Range>
    multimap(std::from_range_t,
             _Range&& __range,
             const key_compare& comp = key_compare(),
             const allocator_type& __a = allocator_type())
    : tree_(__vc(comp), typename Tree_::allocator_type(__a)) {
        insert_range(std::forward<_Range>(__range));
    }

    template <class _InputIterator>
    multimap(_InputIterator __f, _InputIterator __l, const allocator_type& __a)
    : multimap(__f, __l, key_compare(), __a) {}
    
    template <_ContainerCompatibleRange<value_type> _Range>
    multimap(std::from_range_t, _Range&& __range, const allocator_type& __a)
    : multimap(std::from_range, std::forward<_Range>(__range), key_compare(), __a) {}
    
    multimap(const multimap& __m) = default;

    multimap& operator=(const multimap& __m) = default;

    multimap(multimap&& __m) = default;

    multimap(multimap&& __m, const allocator_type& __a) : tree_(std::move(__m.tree_), __a) {}

    multimap& operator=(multimap&& __m) = default;

    multimap(std::initializer_list<value_type> init_list, const key_compare& comp = key_compare())
    : tree_(__vc(comp)) {
        insert(init_list.begin(), init_list.end());
    }

    multimap(std::initializer_list<value_type> init_list, const key_compare& comp, const allocator_type& __a)
    : tree_(__vc(comp), typename Tree_::allocator_type(__a)) {
        insert(init_list.begin(), init_list.end());
    }

    multimap(std::initializer_list<value_type> init_list, const allocator_type& __a)
    : multimap(init_list, key_compare(), __a) {}
    
    multimap& operator=(std::initializer_list<value_type> init_list) {
        clear();
        insert(init_list.begin(), init_list.end());
        return *this;
    }

    explicit multimap(const allocator_type& __a) : tree_(typename Tree_::allocator_type(__a)) {}

    multimap(const multimap& __m, const allocator_type& __a) : tree_(__m.tree_, __a) {}

    ~multimap() {
        static_assert(sizeof(mstd::__diagnose_non_const_comparator<_Key, _Compare>()), "");
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

    template <class... _Args>
    iterator emplace(_Args&&... __args) {
        return tree_.__emplace_multi(std::forward<_Args>(__args)...);
    }

    template <class... _Args>
    iterator emplace_hint(const_iterator __p, _Args&&... __args) {
        return tree_.__emplace_hint_multi(__p.i_, std::forward<_Args>(__args)...);
    }

    template <class _Pp, std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0>
    iterator insert(_Pp&& __p) {
        return tree_.__emplace_multi(std::forward<_Pp>(__p));
    }

    template <class _Pp, std::enable_if_t<std::is_constructible_v<value_type, _Pp>, int> = 0>
    iterator insert(const_iterator __pos, _Pp&& __p) {
        return tree_.__emplace_hint_multi(__pos.i_, std::forward<_Pp>(__p));
    }

    iterator insert(value_type&& __v) { return tree_.__emplace_multi(std::move(__v)); }

    iterator insert(const_iterator __p, value_type&& __v) {
        return tree_.__emplace_hint_multi(__p.i_, std::move(__v));
    }

    void insert(std::initializer_list<value_type> init_list) { insert(init_list.begin(), init_list.end()); }

    iterator insert(const value_type& __v) { return tree_.__emplace_multi(__v); }

    iterator insert(const_iterator __p, const value_type& __v) {
        return tree_.__emplace_hint_multi(__p.i_, __v);
    }

    template <class _InputIterator>
    void insert(_InputIterator __f, _InputIterator __l) {
        tree_.__insert_range_multi(__f, __l);
    }

    template <_ContainerCompatibleRange<value_type> _Range>
    void insert_range(_Range&& __range) {
        tree_.__insert_range_multi( std::ranges::begin(__range),  std::ranges::end(__range));
    }

    iterator erase(const_iterator __p) { return tree_.erase(__p.i_); }
    iterator erase(iterator __p) { return tree_.erase(__p.i_); }
    size_type erase(const key_type& __k) { return tree_.__erase_multi(__k); }
    iterator erase(const_iterator __f, const_iterator __l) {
        return tree_.erase(__f.i_, __l.i_);
    }

    iterator insert(node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(nh.empty() || nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to multimap::insert()");
        return tree_.template __node_handle_insert_multi<node_type>(std::move(nh));
    }
    iterator insert(const_iterator __hint, node_type&& nh) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(nh.empty() || nh.get_allocator() == get_allocator(),
                                            "node_type with incompatible allocator passed to multimap::insert()");
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
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        return tree_.__node_handle_merge_multi(source.tree_);
    }
    template <class _Compare2>
    void merge(multimap<key_type, mapped_type, _Compare2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        return tree_.__node_handle_merge_multi(source.tree_);
    }
    template <class _Compare2>
    void merge(map<key_type, mapped_type, _Compare2, allocator_type>& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        return tree_.__node_handle_merge_multi(source.tree_);
    }
    template <class _Compare2>
    void merge(map<key_type, mapped_type, _Compare2, allocator_type>&& source) {
        MSTD_ASSERT_COMPATIBLE_ALLOCATOR(
            source.get_allocator() == get_allocator(), "merging container with incompatible allocator");
        return tree_.__node_handle_merge_multi(source.tree_);
    }

    void clear() noexcept { tree_.clear(); }

    void swap(multimap& __m) noexcept(std::is_nothrow_swappable_v<Tree_>) {
        tree_.swap(__m.tree_);
    }

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
        return tree_.__count_multi(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] size_type count(const _K2& __k) const {
        return tree_.__count_multi(__k);
    }

    [[nodiscard]] bool contains(const key_type& __k) const { return find(__k) != end(); }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] bool contains(const _K2& __k) const {
        return find(__k) != end();
    }

    [[nodiscard]] iterator lower_bound(const key_type& __k) {
        return tree_.__lower_bound_multi(__k);
    }

    [[nodiscard]] const_iterator lower_bound(const key_type& __k) const {
        return tree_.__lower_bound_multi(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] iterator lower_bound(const _K2& __k) {
        return tree_.__lower_bound_multi(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] const_iterator lower_bound(const _K2& __k) const {
        return tree_.__lower_bound_multi(__k);
    }

    [[nodiscard]] iterator upper_bound(const key_type& __k) {
        return tree_.__upper_bound_multi(__k);
    }

    [[nodiscard]] const_iterator upper_bound(const key_type& __k) const {
        return tree_.__upper_bound_multi(__k);
    }

    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] iterator upper_bound(const _K2& __k) {
        return tree_.__upper_bound_multi(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] const_iterator upper_bound(const _K2& __k) const {
        return tree_.__upper_bound_multi(__k);
    }

    [[nodiscard]] std::pair<iterator, iterator> equal_range(const key_type& __k) {
        return tree_.__equal_range_multi(__k);
    }
    [[nodiscard]] std::pair<const_iterator, const_iterator> equal_range(const key_type& __k) const {
        return tree_.__equal_range_multi(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] std::pair<iterator, iterator> equal_range(const _K2& __k) {
        return tree_.__equal_range_multi(__k);
    }
    template <typename _K2, std::enable_if_t<__is_transparent_v<_Compare, _K2>, int> = 0>
    [[nodiscard]] std::pair<const_iterator, const_iterator> equal_range(const _K2& __k) const {
        return tree_.__equal_range_multi(__k);
    }

private:
    typedef typename Tree_::__node __node;
    typedef typename Tree_::__node_allocator __node_allocator;
    typedef typename Tree_::__node_pointer __node_pointer;

    typedef MapNodeDestructor<__node_allocator> _Dp;
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
    using __map = multimap<_Key, _Tp, _Compare, _Allocator>;

    static const bool __has_algorithm = true;

    template <class _Map, class _Func, class _Proj>
    static auto operator()(_Map&& __map, _Func __func, _Proj __proj) {
        auto [_, __func2] = __specialized_algorithm<_Algorithm::__for_each, __single_range<typename __map::Tree_>>()(
            __map.tree_, std::move(__func), std::move(__proj));
        return std::make_pair(__map.end(), std::move(__func2));
    }
};

template <class _Key, class _Tp, class _Compare, class _Allocator>
inline bool
operator==(const multimap<_Key, _Tp, _Compare, _Allocator>& x, const multimap<_Key, _Tp, _Compare, _Allocator>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
__synth_three_way_result<std::pair<const _Key, _Tp>>
operator<=>(const multimap<_Key, _Tp, _Compare, _Allocator>& x,
            const multimap<_Key, _Tp, _Compare, _Allocator>& y) {
    return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(), y.end(), __synth_three_way);
}

template <class _Key, class _Tp, class _Compare, class _Allocator>
inline void
swap(multimap<_Key, _Tp, _Compare, _Allocator>& x, multimap<_Key, _Tp, _Compare, _Allocator>& y)
noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

template <class _Key, class _Tp, class _Compare, class _Allocator, class _Predicate>
inline typename multimap<_Key, _Tp, _Compare, _Allocator>::size_type
erase_if(multimap<_Key, _Tp, _Compare, _Allocator>& __c, _Predicate pred) {
    return mstd::_MSTD_erase_if_container(__c, pred);
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
    using map = mstd::map<_KeyT, _ValueT, _CompareT, std::pmr::polymorphic_allocator<std::pair<const _KeyT, _ValueT>>>;
    template <class _KeyT, class _ValueT, class _CompareT = std::less<_KeyT>>
    using multimap = mstd::multimap<_KeyT, _ValueT, _CompareT, std::pmr::polymorphic_allocator<std::pair<const _KeyT, _ValueT>>>;
} // namespace pmr
} // namespace mstd


#endif // MSTD_MAP
