
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD___TREE2
#define MSTD___TREE2

#include "detail/concepts/compare_concepts.hpp"
#include <algorithm>
#include <cassert>
#include <compare>
#include <utility>
#include <iterator>
#include <memory>
#include <type_traits>
#include <limits>
#include <functional>

#include <detail/utility/try_key_extraction.hpp>
#include <detail/node_handle.hpp>
#include <detail/tree_node_algorithms.hpp>

// GCC complains about the backslashes at the end, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=121528
// Tree is a red-black-tree implementation used for the associative containers (i.e. (multi)map/set). It stores
// - (1) a pointer to the node with the smallest (i.e. leftmost) element, namely begin_node_
// - (2) the number of nodes in the tree, namely size_
// - (3) a pointer to the root of the tree, namely end_node_
//
// Storing (1) and (2) is required to allow for constant time lookups. A tree looks like this in memory:
//
//       end_node_
//            |
//           root---
//          /      |
//         /       |
//        /        |
//       /         |
//      l1        r1--
//     /  \      /   |
//   ...  ...  ...   ...
//
// All nodes except end_node_ have a left_ and right_ pointer as well as a parent_ pointer.
// end_node_ only contains a left_ pointer, which points to the root of the tree.
// This layout allows for iteration through the tree without a need for special handling of the end node. See
// tree_next_iter and tree_prev_iter for more details.

namespace mstd {

template <
    class ValueT
  , class KeyProj
  , class KeyCompareT
  , class AllocatorT
>
class Tree {
public:
    using value_type     = ValueT;
    using key_compare    = KeyCompareT;
    using key_proj       = KeyProj;
    using allocator_type = AllocatorT;

private:
    using AllocTraits_ = std::allocator_traits<allocator_type>;
    using KeyType_     = std::remove_cvref_t<decltype(std::invoke(std::declval<const KeyProj&>(), std::declval<value_type&>()))>;
    static_assert(OrdersAtLeastWeakly<key_compare, KeyType_>, "key_compare must be at least a weak ordering on the projected key type");

public:
    using pointer         = AllocTraits_::pointer;
    using const_pointer   = AllocTraits_::const_pointer;
    using size_type       = AllocTraits_::size_type;
    using difference_type = AllocTraits_::difference_type;

    using void_pointer = AllocTraits_::void_pointer;
    using void_pointer_traits = std::pointer_traits<void_pointer>;

private:

    class EndNode;

    class NodeBase : public EndNode {
    public:
        using pointer          = void_pointer_traits::template rebind<NodeBase>;
        using end_node_pointer = void_pointer_traits::template rebind<EndNode>;
    
        pointer right_;
        end_node_pointer parent_;
        bool is_black_;
    
        pointer parent_unsafe() const { return static_cast<pointer>(parent_); }
    
        void set_parent(pointer parent) {
            parent_ = static_cast<end_node_pointer>(parent);
        }
    
        NodeBase()             = default;
        NodeBase(NodeBase const&)            = delete;
        NodeBase& operator=(NodeBase const&) = delete;
    };

    class EndNode {
    public:
        using pointer = typename void_pointer_traits::template rebind<NodeBase>;
        pointer left_;
    
        EndNode() noexcept : left_() {}
    };

    class Node : public NodeBase {
    public:
        // We use a union to avoid initialization during member initialization, which allows us
        // to use the allocator from the container to construct the `node_value_type` in the
        // memory provided by the union member
    
    private:
        template <class NodeAllocatorT>
        class Destructor {
            using AllocatorType_ = NodeAllocatorT;
            using AllocTraits_   = std::allocator_traits<AllocatorType_>;
        
        public:
            using pointer = AllocTraits_::pointer;
        
        private:
            AllocatorType_& na_;
        
        public:
            bool value_constructed;
        
            Destructor(const Destructor&)            = default;
            Destructor& operator=(const Destructor&) = delete;
        
            explicit Destructor(AllocatorType_& na, bool val = false) noexcept
            : na_(na)
            , value_constructed(val) {}
        
            void operator()(pointer ptr) noexcept {
                if (value_constructed) {
                    AllocTraits_::destroy(na_, std::addressof(ptr->get_value()));
                }
                if (ptr) {
                    AllocTraits_::deallocate(na_, ptr, 1);
                }
            }
        };
    
        union {
            ValueT value_;
        };
    
    public:
        ValueT& get_value() { return value_; }
        const ValueT& get_value() const { return value_; }
    
        template <class AllocT, class... ArgsT>
        explicit Node(AllocT& node_alloc, ArgsT&&... args) {
            std::allocator_traits<AllocT>::construct(node_alloc, std::addressof(get_value()), std::forward<ArgsT>(args)...);
        }
        ~Node()                      = delete;
        Node(Node const&)            = delete;
        Node& operator=(Node const&) = delete;
    
        template <class, class>
        friend class BasicNodeHandle;
    
        template <class, class, class, class>
        friend class Tree;
    };

public:

    using node         = Node;
    using node_pointer = void_pointer_traits::template rebind<node>;

    using node_base         = NodeBase;
    using node_base_pointer = void_pointer_traits::template rebind<node_base>;

    using end_node_t       = EndNode;
    using end_node_pointer = void_pointer_traits::template rebind<end_node_t>;

    using node_allocator = AllocTraits_::template rebind_alloc<node>;
    using node_traits    = std::allocator_traits<node_allocator>;

private:
    // check for sane allocator pointer rebinding semantics. Rebinding the
    // allocator for a new pointer type should be exactly the same as rebinding
    // the pointer using 'pointer_traits'.
    static_assert(std::is_same_v<node_pointer, typename node_traits::pointer>,
                  "Allocator does not rebind pointers in a sane manner.");
    using NodeBaseAllocator_ = node_traits::template rebind_alloc <node_base>;
    using NodeBaseTraits_    = std::allocator_traits<NodeBaseAllocator_>;
    static_assert(std::is_same_v<node_base_pointer, typename NodeBaseTraits_::pointer>,
                  "Allocator does not rebind pointers in a sane manner.");

    template<bool is_const>
    class TreeIterator {
        using NodeBasePointer_ = Tree::node_base_pointer;
        using EndNodePointer_  = Tree::end_node_pointer;
    
        EndNodePointer_ ptr_;
    
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = Tree::value_type;
        using difference_type   = Tree::difference_type;
        using reference         = std::conditional_t<is_const, const value_type&, value_type&>;
        using pointer           = void_pointer_traits::template rebind<value_type>;
    
        TreeIterator() noexcept : ptr_(nullptr) {}

        template <bool other_is_const>
        TreeIterator(TreeIterator<other_is_const> non_const_iter) noexcept requires (is_const || (other_is_const == is_const))
            : ptr_(non_const_iter.ptr_) {}
    
        reference operator*() const {
            return __get_np()->get_value();
        }
    
        pointer operator->() const {
            return std::pointer_traits<pointer>::pointer_to(__get_np()->get_value());
        }
    
        TreeIterator& operator++() {
            ptr_ = mstd::detail::tree_next_iter<EndNodePointer_>(static_cast<NodeBasePointer_>(ptr_));
            return *this;
        }
    
        TreeIterator operator++(int) {
            TreeIterator iter(*this);
            ++(*this);
            return iter;
        }
    
        TreeIterator& operator--() {
            ptr_ = static_cast<EndNodePointer_>(mstd::detail::tree_prev_iter<NodeBasePointer_>(ptr_));
            return *this;
        }
    
        TreeIterator operator--(int) {
            TreeIterator iter(*this);
            --(*this);
            return iter;
        }
    
        friend bool operator==(const TreeIterator& lhs, const TreeIterator& rhs) {
            return lhs.ptr_ == rhs.ptr_;
        }

        friend bool operator!=(const TreeIterator& lhs, const TreeIterator& rhs) {
            return !(lhs == rhs);
        }

    private:
        explicit TreeIterator(node_pointer ptr) noexcept : ptr_(ptr) {}
        explicit TreeIterator(EndNodePointer_ ptr) noexcept : ptr_(ptr) {}
        node_pointer __get_np() const {
            return static_cast<node_pointer>(ptr_);
        }
        template <class, class, class, class>
        friend class Tree;
    
        template <class NodeIterT, class FuncT, class ProjT>
        friend void tree_iterate_subrange(NodeIterT, NodeIterT, FuncT&, ProjT&);
    };

private:
    end_node_pointer begin_node_;
    end_node_t end_node_;
    node_allocator node_alloc_;
    size_type size_;
    key_compare key_comp_;
    key_proj key_proj_;


public:
    end_node_pointer endNode() noexcept {
        return std::pointer_traits<end_node_pointer>::pointer_to(end_node_);
    }
    end_node_pointer endNode() const noexcept {
        return std::pointer_traits<end_node_pointer>::pointer_to(const_cast<end_node_t&>(end_node_));
    }
    node_allocator& nodeAlloc() noexcept { return node_alloc_; }

private:
    const node_allocator& nodeAlloc() const noexcept { return node_alloc_; }

public:
    allocator_type alloc() const noexcept { return allocator_type(nodeAlloc()); }

    size_type size() const noexcept { return size_; }
    key_compare& key_comp() noexcept { return key_comp_; }
    const key_compare& key_comp() const noexcept { return key_comp_; }

    node_pointer root() const noexcept {
        return static_cast<node_pointer>(endNode()->left_);
    }

    node_base_pointer* root_ptr() const noexcept {
        return std::addressof(endNode()->left_);
    }

    using iterator       = TreeIterator<false>;
    using const_iterator = TreeIterator<true>;

    using insert_return_type = InsertReturnType<iterator>;

    template <class Self>
    using SelfIterator = std::conditional_t<std::is_const_v<Self>, const_iterator, iterator>;

    template <class Self>
    using SelfSubrange = std::ranges::subrange<SelfIterator<Self>>;

    explicit Tree(const key_compare& comp) noexcept(
        std::is_nothrow_default_constructible<node_allocator>::value
     && std::is_nothrow_copy_constructible<key_compare>::value
    )
    : size_(0)
    , key_comp_(comp) {
        begin_node_ = endNode();
    }

    explicit Tree(const allocator_type& alloc)
    : begin_node_()
    , node_alloc_(node_allocator(alloc))
    , size_(0) {
        begin_node_ = endNode();
    }

    Tree(const key_compare& comp, const allocator_type& alloc)
    : begin_node_()
    , node_alloc_(node_allocator(alloc))
    , size_(0)
    , key_comp_(comp) {
        begin_node_ = endNode();
    }

    Tree(const Tree& other)
    : begin_node_(endNode())
    , node_alloc_(node_traits::select_on_container_copy_construction(other.nodeAlloc()))
    , size_(0)
    , key_comp_(other.key_comp()) {
        if (other.size() == 0) {
            return;
        }
        *root_ptr()     = static_cast<node_base_pointer>(copyConstructTree_(other.root()));
        root()->parent_ = endNode();
        begin_node_     = static_cast<end_node_pointer>(mstd::detail::tree_min(endNode()->left_));
        size_           = other.size();
    }

    Tree(const Tree& other, const allocator_type& alloc)
    : begin_node_(endNode())
    , node_alloc_(alloc)
    , size_(0)
    , key_comp_(other.key_comp_) {
        if (other.size() == 0) {
            return;
        }

        *root_ptr()     = static_cast<node_base_pointer>(copyConstructTree_(other.root()));
        root()->parent_ = endNode();
        begin_node_     = static_cast<end_node_pointer>(mstd::detail::tree_min(endNode()->left_));
        size_           = other.size();
    }

    Tree& operator=(const Tree& other) {
        if (this == std::addressof(other)) {
            return *this;
        }
        key_comp_ = other.key_comp_;
        if constexpr (node_traits::propagate_on_container_copy_assignment::value) {
            if (nodeAlloc() != other.nodeAlloc()) {
                clear();
            }
            nodeAlloc() = other.nodeAlloc();
        }
        if (size_ != 0) {
            *root_ptr() = static_cast<node_base_pointer>(copyAssignTree(root(), other.root()));
        } else {
            *root_ptr() = static_cast<node_base_pointer>(copyConstructTree_(other.root()));
            if (root()) {
                root()->parent_ = endNode();
            }
        }
        begin_node_
            = endNode()->left_
              ? static_cast<end_node_pointer>(mstd::detail::tree_min(endNode()->left_))
              : endNode();
        size_ = other.size();

        return *this;
    }

    template <class _ForwardIterator>
    void assign_unique(_ForwardIterator __first, _ForwardIterator __last);

    Tree(Tree&& other) noexcept(
        std::is_nothrow_move_constructible<node_allocator>::value
     && std::is_nothrow_move_constructible<key_compare>::value
    )
    : begin_node_(std::move(other.begin_node_))
    , end_node_(std::move(other.end_node_))
    , node_alloc_(std::move(other.node_alloc_))
    , size_(other.size_)
    , key_comp_(std::move(other.key_comp_)) {
        if (size_ == 0) {
            begin_node_ = endNode();
        } else {
            endNode()->left_->parent_ = static_cast<end_node_pointer>(endNode());
            other.begin_node_         = other.endNode();
            other.endNode()->left_    = nullptr;
            other.size_               = 0;
        }
    }

    Tree(Tree&& other, const allocator_type& alloc)
    : begin_node_(endNode())
    , node_alloc_(node_allocator(alloc))
    , size_(0)
    , key_comp_(std::move(other.key_comp())) {
        if (other.size() == 0) {
            return;
        }
        if (alloc == other.alloc()) {
            begin_node_               = other.begin_node_;
            endNode()->left_          = other.endNode()->left_;
            endNode()->left_->parent_ = static_cast<end_node_pointer>(endNode());
            size_                     = other.size_;
            other.begin_node_         = other.endNode();
            other.endNode()->left_    = nullptr;
            other.size_               = 0;
        } else {
            *root_ptr()     = static_cast<node_base_pointer>(moveConstructTree_(other.root()));
            root()->parent_ = endNode();
            begin_node_     = static_cast<end_node_pointer>(mstd::detail::tree_min(endNode()->left_));
            size_           = other.size();
            other.clear(); // Ensure that other is in a valid state after moving out the keys
        }
    }

    Tree& operator=(Tree&& other)
    noexcept(std::is_nothrow_move_assignable<key_compare>::value)
    requires (node_traits::is_always_equal::value) {
        moveAssignRelinking_(other);
        return *this;
    }

    Tree& operator=(Tree&& other) 
    noexcept(
        std::is_nothrow_move_assignable<key_compare>::value
     && std::is_nothrow_move_assignable<node_allocator>::value
    )
    requires (
        !node_traits::is_always_equal::value
     && node_traits::propagate_on_container_move_assignment::value
    ) {
        moveAssignRelinking_(other);
        return *this;
    }

    Tree& operator=(Tree&& other)
    requires (
        !node_traits::is_always_equal::value
     && !node_traits::propagate_on_container_move_assignment::value
    ) {
        if (nodeAlloc() == other.nodeAlloc()) {
            moveAssignRelinking_(other);
        } else {
            moveAssign_(other);
        }
        return *this;
    }

    ~Tree() {
        static_assert(
            std::is_copy_constructible<key_compare>::value,
            "Comparator must be copy-constructible."
        );
        destroy_(root());
    }

    template <class Self>
    SelfIterator<Self> begin(this Self& self) noexcept {
        return SelfIterator<Self>{self.begin_node_};
    }

    template <class Self>
    SelfIterator<Self> end(this Self& self) noexcept {
        return SelfIterator<Self>{self.endNode()};
    }

    size_type max_size() const noexcept {
        return std::min<size_type>(
            node_traits::max_size(nodeAlloc()),
            std::numeric_limits<difference_type >::max()
        );
    }

    void clear() noexcept {
        destroy_(root());
        size_            = 0;
        begin_node_      = endNode();
        endNode()->left_ = nullptr;
    }

    void swap(Tree& other) noexcept(std::is_nothrow_swappable_v<key_compare>) {
        std::swap(begin_node_, other.begin_node_);
        std::swap(end_node_, other.end_node_);
        if constexpr (node_traits::propagate_on_container_swap::value) {
            std::swap(nodeAlloc(), other.nodeAlloc());
        }
        std::swap(size_, other.size_);
        std::swap(key_comp_, other.key_comp_);
        if (size_ == 0) {
            begin_node_ = endNode();
        } else {
            endNode()->left_->parent_ = endNode();
        }
        if (other.size_ == 0) {
            other.begin_node_ = other.endNode();
        } else {
            other.endNode()->left_->parent_ = other.endNode();
        }
    }

    template <class... ArgsT>
    iterator emplaceMulti(ArgsT&&... args) {
        auto holder = constructNode_(std::forward<ArgsT>(args)...);
        end_node_pointer parent;
        auto& child = findLeafHigh_(parent, holder->get_value());
        insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
        return iterator(static_cast<node_pointer>(holder.release()));
    }

    template <class... ArgsT>
    iterator emplaceHintMulti(const_iterator pos, ArgsT&&... args) {
        auto holder = constructNode_(std::forward<ArgsT>(args)...);
        end_node_pointer parent;
        node_base_pointer& child = findLeaf_(pos, parent, holder->get_value());
        insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
        return iterator(static_cast<node_pointer>(holder.release()));
    }

    template <class... ArgsT>
    insert_return_type emplaceUnique(ArgsT&&... args) {
        return mstd::try_key_extraction<KeyType_>(
            [this](const KeyType_& key, ArgsT&&... args2) {
                auto [parent, child] = find_equivalent(key);
                auto ret             = static_cast<node_pointer>(child);
                bool inserted        = false;
                if (child == nullptr) {
                    node_holder holder = constructNode_(std::forward<ArgsT>(args2)...);
                    insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
                    ret      = holder.release();
                    inserted = true;
                }
                return insert_return_type{
                    .position = iterator(ret),
                    .inserted = inserted,
                };
            },
            [this](ArgsT&&... args2) {
                node_holder holder = constructNode_(std::forward<ArgsT>(args2)...);
                const auto& holder_key = key_proj_(holder->get_value());
                auto [parent, child] = find_equivalent(holder_key);
                auto ret             = static_cast<node_pointer>(child);
                bool inserted        = false;
                if (child == nullptr) {
                    insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
                    ret      = holder.release();
                    inserted = true;
                }
                return insert_return_type{
                    .position = iterator(ret),
                    .inserted = inserted,
                };
            },
            std::forward<ArgsT>(args)...);
    }

    template <class... ArgsT>
    insert_return_type emplaceHintUnique(const_iterator pos, ArgsT&&... args) {
        return mstd::try_key_extraction<KeyType_>(
            [this, pos](const KeyType_& key, ArgsT&&... args2) {
                node_base_pointer dummy;
                auto [parent, child] = find_equivalent(pos, dummy, key);
                auto ret             = static_cast<node_pointer>(child);
                bool inserted        = false;
                if (child == nullptr) {
                    auto holder = constructNode_(std::forward<ArgsT>(args2)...);
                    insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
                    ret        = holder.release();
                    inserted = true;
                }
                return insert_return_type{
                    .position = iterator(ret),
                    .inserted = inserted,
                };
            },
            [this, pos](ArgsT&&... args2) {
                auto holder = constructNode_(std::forward<ArgsT>(args2)...);
                node_base_pointer dummy;
                const auto& holder_key = key_proj_(holder->get_value());
                auto [parent, child] = find_equivalent(pos, dummy, holder_key);
                auto ret             = static_cast<node_pointer>(child);
                if (child == nullptr) {
                    insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
                    ret = holder.release();
                }
                return insert_return_type{
                    .position = iterator(ret),
                    .inserted = (child == nullptr),
                };
            },
            std::forward<ArgsT>(args)...);
    }

    template <class InIterT, class SentT>
    void insertRangeMulti(InIterT begin, SentT end) {
        if (begin == end) {
            return;
        }

        if (root() == nullptr) { // Make sure we always have a root node
            insertNodeAt(
                endNode(),
                endNode()->left_,
                static_cast<node_base_pointer>(constructNode_(*begin).release())
            );
            ++begin;
        }

        auto max_node = static_cast<node_pointer>(mstd::detail::tree_max(static_cast<node_base_pointer>(root())));

        for (; begin != end; ++begin) {
            auto holder = constructNode_(*begin);
            const auto& max_node_key = key_proj_(max_node->get_value());
            const auto& holder_key = key_proj_(holder->get_value());
            // Always check the max node first. This optimizes for sorted ranges inserted at the end.
            if (key_comp_(max_node_key, holder_key) >= 0) { // node >= __max_val
                insertNodeAt(static_cast<end_node_pointer>(max_node),
                             max_node->right_,
                             static_cast<node_base_pointer>(holder.get()));
                max_node = holder.release();
            } else {
                end_node_pointer parent;
                node_base_pointer& child = findLeafHigh_(parent, holder->get_value());
                insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.release()));
            }
        }
    }

    template <class InIterT, class SentT>
    void insertRangeUnique(InIterT begin, SentT end) {
        if (begin == end) {
            return;
        }

        if (root() == nullptr) {
            insertNodeAt(endNode(),
                         endNode()->left_,
                         static_cast<node_base_pointer>(constructNode_(*begin).release()));
            ++begin;
        }

        auto max_node = static_cast<node_pointer>(mstd::detail::tree_max(static_cast<node_base_pointer>(root())));

        using Reference = decltype(*begin);

        for (; begin != end; ++begin) {
            mstd::try_key_extraction<KeyType_>(
                [this, &max_node](const KeyType_& key, Reference&& val) {
                    const auto& max_node_key = key_proj_(max_node->get_value());
                    if (key_comp_(max_node_key, key) < 0) { // key > max_node
                        auto holder = constructNode_(std::forward<Reference>(val));
                        insertNodeAt(static_cast<end_node_pointer>(max_node),
                                     max_node->right_,
                                     static_cast<node_base_pointer>(holder.get()));
                        max_node = holder.release();
                    } else {
                        auto [parent, child] = find_equivalent(key);
                        if (child == nullptr) {
                            auto holder = constructNode_(std::forward<Reference>(val));
                            insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.release()));
                        }
                    }
                },
                [this, &max_node](Reference&& val) {
                    auto holder = constructNode_(std::forward<Reference>(val));
                    const auto& max_node_key = key_proj_(max_node->get_value());
                    const auto& holder_key = key_proj_(holder->get_value());
                    if (key_comp_(max_node_key, holder_key) < 0) { // node > max_node
                        insertNodeAt(static_cast<end_node_pointer>(max_node),
                                     max_node->right_,
                                     static_cast<node_base_pointer>(holder.get()));
                        max_node = holder.release();
                    } else {
                        auto [parent, child] = find_equivalent(key_proj_(holder->get_value()));
                        if (child == nullptr) {
                            insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.release()));
                        }
                    }
                },
                *begin);
        }
    }

    iterator removeNodePointer(node_pointer ptr) noexcept {
        iterator ret(ptr);
        ++ret;
        if (begin_node_ == ptr) {
            begin_node_ = ret.ptr_;
        }
        --size_;
        mstd::detail::tree_remove(endNode()->left_, static_cast<node_base_pointer>(ptr));
        return ret;
    }

    template <class NodeHandleT>
    NodeHandleInsertReturnType<iterator, NodeHandleT>
    nodeHandleInsertUnique(NodeHandleT&& nh) {
        if (nh.empty()) {
            return NodeHandleInsertReturnType{
                .position = end(),
                .inserted = false,
                .node     = NodeHandleT(),
            };
        }
        auto ptr = nh.ptr_;
        auto [parent, child] = find_equivalent(ptr->get_value());
        if (child != nullptr) {
            return NodeHandleInsertReturnType{
                .position = iterator(static_cast<node_pointer>(child)),
                .inserted = false,
                .node     = std::move(nh),
            };
        }
        insertNodeAt(parent, child, static_cast<node_base_pointer>(ptr));
        nh.__release_ptr();
        return NodeHandleInsertReturnType{
            .position = iterator(ptr),
            .inserted = true,
            .node     = NodeHandleT(),
        };
    }

    template <class NodeHandleT>
    iterator nodeHandleInsertUnique(const_iterator hint, NodeHandleT&& nh) {
        if (nh.empty()) {
            return end();
        }
        auto ptr = nh.ptr_;
        node_base_pointer dummy;
        auto [parent, child] = find_equivalent(hint, dummy, ptr->get_value());
        auto ret             = static_cast<node_pointer>(child);
        if (child == nullptr) {
            insertNodeAt(parent, child, static_cast<node_base_pointer>(ptr));
            ret = ptr;
            nh.__release_ptr();
        }
        return iterator(ret);
    }

    template <class Comp2T>
    void nodeHandleMergeUnique(Tree<ValueT, KeyProj, Comp2T, AllocatorT>& source) {
        for (iterator iter = source.begin(); iter != source.end();) {
            auto src_ptr = iter.__get_np();
            auto [parent, child] = find_equivalent(src_ptr->get_value());
            ++iter;
            if (child != nullptr) {
                continue;
            }
            source.removeNodePointer(src_ptr);
            insertNodeAt(parent, child, static_cast<node_base_pointer>(src_ptr));
        }
    }

    template <class NodeHandleT>
    iterator nodeHandleInsertMulti(NodeHandleT&& nh) {
        if (nh.empty()) {
            return end();
        }
        auto ptr = nh.ptr_;
        end_node_pointer parent;
        auto& child = findLeafHigh_(parent, ptr->get_value());
        insertNodeAt(parent, child, static_cast<node_base_pointer>(ptr));
        nh.__release_ptr();
        return iterator(ptr);
    }

    template <class NodeHandleT>
    iterator nodeHandleInsertMulti(const_iterator hint, NodeHandleT&& nh) {
        if (nh.empty()) {
            return end();
        }
        auto ptr = nh.ptr_;
        end_node_pointer parent;
        auto& child = findLeaf_(hint, parent, ptr->get_value());
        insertNodeAt(parent, child, static_cast<node_base_pointer>(ptr));
        nh.__release_ptr();
        return iterator(ptr);
    }

    template <class Comp2T>
    void nodeHandleMergeMulti(Tree<ValueT, KeyProj, Comp2T, AllocatorT>& source) {
        for (iterator iter = source.begin(); iter != source.end();) {
            auto src_ptr = iter.__get_np();
            end_node_pointer parent;
            auto& child = findLeafHigh_(parent, src_ptr->get_value());
            ++iter;
            source.removeNodePointer(src_ptr);
            insertNodeAt(parent, child, static_cast<node_base_pointer>(src_ptr));
        }
    }

    template <class NodeHandleT>
    NodeHandleT nodeHandleExtract(KeyType_ const& key) {
        if (const auto iter = find(key); iter != end()) {
            return nodeHandleExtract<NodeHandleT>(iter);
        }
        return NodeHandleT();
    }

    template <class NodeHandleT>
    NodeHandleT nodeHandleExtract(const_iterator pos) {
        auto node_ptr = pos.__get_np();
        removeNodePointer(node_ptr);
        return NodeHandleT(node_ptr, alloc());
    }

    iterator erase(const_iterator pos) {
        node_pointer node_ptr = pos.__get_np();
        iterator ret          = removeNodePointer(node_ptr);
        auto& node_allocator  = nodeAlloc();
        // Node value must be destroyed after the node is removed, 
        // while node is not destroyable itself.
        node_traits::destroy(node_allocator, std::addressof(node_ptr->get_value()));
        node_traits::deallocate(node_allocator, node_ptr, 1);
        return ret;
    }

    iterator erase(const_iterator begin, const_iterator end) {
        while (begin != end) {
            begin = erase(begin);
        }
        return iterator(end.ptr_);
    }

    template <class KeyT>
    size_type eraseUnique(const KeyT& key) {
        if (const auto iter = find(key); iter != end()) {
            erase(iter);
            return 1;
        }
        return 0;
    }

    template <class KeyT>
    size_type eraseMulti(const KeyT& key) {
        auto [begin, end] = equalRangeMulti(key);
        size_type ret_cnt = 0;
        for (; begin != end; ++ret_cnt) {
            begin = erase(end);
        }
        return ret_cnt;
    }

    void insertNodeAt(end_node_pointer parent, node_base_pointer& child, node_base_pointer new_node) noexcept {
        new_node->left_   = nullptr;
        new_node->right_  = nullptr;
        new_node->parent_ = parent;
        // new_node->is_black_ is initialized in tree_balance_after_insert
        child = new_node;
        if (begin_node_->left_ != nullptr) {
            begin_node_ = static_cast<end_node_pointer>(begin_node_->left_);
        }
        mstd::detail::tree_balance_after_insert(endNode()->left_, child);
        ++size_;
    }

    template <class Self, class KeyT>
    SelfIterator<Self> find(this Self& self, const KeyT& key) {
        auto [__, match] = self.find_equivalent(key);
        if (match == nullptr) {
            return self.end();
        }
        return SelfIterator<Self>{static_cast<node_pointer>(match)};
    }

    template <class KeyT>
    size_type countUnique(const KeyT& key) const {
        auto root_node = root();
        while (root_node != nullptr) {
            const auto& root_key = key_proj_(root_node->get_value());
            const auto comp_res      = key_comp_(key, root_key);
            if (comp_res < 0) {
                root_node = static_cast<node_pointer>(root_node->left_);
            } else if (comp_res > 0) {
                root_node = static_cast<node_pointer>(root_node->right_);
            } else {
                return 1;
            }
        }
        return 0;
    }

    template <class KeyT>
    size_type countMulti(const KeyT& key) const {
        auto result    = endNode();
        auto root_node = root();
        while (root_node != nullptr) {
            const auto& root_key = key_proj_(root_node->get_value());
            const auto comp_res = key_comp_(key, root_key);
            if (comp_res < 0) {
                result    = static_cast<end_node_pointer>(root_node);
                root_node = static_cast<node_pointer>(root_node->left_);
            } else if (comp_res > 0) {
                root_node = static_cast<node_pointer>(root_node->right_);
            } else {
                return std::distance(
                    const_iterator{lowerUpperBoundMultiImpl_<true>(key, static_cast<node_pointer>(root_node->left_), static_cast<end_node_pointer>(root_node))},
                    const_iterator{lowerUpperBoundMultiImpl_<false>(key, static_cast<node_pointer>(root_node->right_), result)}
                );
            }
        }
        return 0;
    }

    template <class Self, class KeyT>
    SelfIterator<Self> lowerBoundUnique(this Self& self, const KeyT& key) {
        return SelfIterator<Self>{self.template lowerUpperBoundUniqueImpl_<true>(key)};
    }

    template <class Self, class KeyT>
    SelfIterator<Self> upperBoundUnique(this Self& self, const KeyT& key) {
        return SelfIterator<Self>(self.template lowerUpperBoundUniqueImpl_<false>(key));
    }

private:
    template <bool lower_bound, class KeyT>
    end_node_pointer lowerUpperBoundUniqueImpl_(const KeyT& key) const {
        auto root_node = root();
        auto result    = endNode();
        while (root_node != nullptr) {
            const auto& root_key = key_proj_(root_node->get_value());
            const auto comp_res = key_comp_(key, root_key);

            if (comp_res < 0) {
                result    = static_cast<end_node_pointer>(root_node);
                root_node = static_cast<node_pointer>(root_node->left_);
            } else if (comp_res > 0) {
                root_node = static_cast<node_pointer>(root_node->right_);
            } else if constexpr (lower_bound) {
                return static_cast<end_node_pointer>(root_node);
            } else {
                return root_node->right_
                    ? static_cast<end_node_pointer>(mstd::detail::tree_min(root_node->right_))
                    : result;
            }
        }
        return result;
    }

    template <bool lower_bound, class KeyT>
    end_node_pointer lowerUpperBoundMultiImpl_(const KeyT& key, node_pointer root_node, end_node_pointer result) const {
        while (root_node != nullptr) {
            const auto& root_key = key_proj_(root_node->get_value());
            const auto comp_res = key_comp_(key, root_key);
            if (lower_bound ? (comp_res <= 0) : (comp_res < 0)) {
                result    = static_cast<end_node_pointer>(root_node);
                root_node = static_cast<node_pointer>(root_node->left_);
            } else {
                root_node = static_cast<node_pointer>(root_node->right_);
            }
        }
        return result;
    }

public:
    template <class Self, class KeyT>
    SelfIterator<Self> lowerBoundMulti(this Self& self, const KeyT& key) {
        return SelfIterator<Self>{self.template lowerUpperBoundMultiImpl_<true>(key, self.root(), self.endNode())};
    }
    
    template <class Self, class KeyT>
    SelfIterator<Self> upperBoundMulti(this Self& self, const KeyT& key) {
        return SelfIterator<Self>{self.template lowerUpperBoundMultiImpl_<false>(key, self.root(), self.endNode())};
    }

public:
    template <class Self, class KeyT>
    SelfSubrange<Self> equalRangeUnique(this Self& self, const KeyT& key) {
        auto result    = self.endNode();
        auto root_node = self.root();
        while (root_node != nullptr) {
            const auto& root_key = self.key_proj_(root_node->get_value());
            const auto comp_res  = self.key_comp_(key, root_key);
            if (comp_res < 0) {
                result    = static_cast<end_node_pointer>(root_node);
                root_node = static_cast<node_pointer>(root_node->left_);
            } else if (comp_res > 0) {
                root_node = static_cast<node_pointer>(root_node->right_);
            } else {
                return {
                    SelfIterator<Self>(root_node),
                    SelfIterator<Self>(
                        (root_node->right_ != nullptr)
                        ? static_cast<end_node_pointer>(mstd::detail::tree_min(root_node->right_))
                        : result
                    ),
                };
            }
        }
        return {
            SelfIterator<Self>(result),
            SelfIterator<Self>(result),
        };
    }

    template <class Self, class KeyT>
    SelfSubrange<Self> equalRangeMulti(this Self& self, const KeyT& key) {
        auto result    = self.endNode();
        auto root_node = self.root();
        while (root_node != nullptr) {
            const auto& root_key = self.key_proj_(root_node->get_value());
            const auto comp_res  = self.key_comp_(key, root_key);
            if (comp_res < 0) {
                result    = static_cast<end_node_pointer>(root_node);
                root_node = static_cast<node_pointer>(root_node->left_);
            } else if (comp_res > 0) {
                root_node = static_cast<node_pointer>(root_node->right_);
            } else {  // Equal
                auto begin = self.template lowerUpperBoundMultiImpl_<true>(
                                 key,
                                 static_cast<node_pointer>(root_node->left_),
                                 static_cast<end_node_pointer>(root_node));
                auto end = self.template lowerUpperBoundMultiImpl_<false>(
                               key,
                               static_cast<node_pointer>(root_node->right_),
                               result);
                return {
                    SelfIterator<Self>{begin},
                    SelfIterator<Self>{end},
                };
            }
        }
        return {
            SelfIterator<Self>(result),
            SelfIterator<Self>(result),
        };
    }

    using node_holder = std::unique_ptr<node, typename node::template Destructor<node_allocator>>;

    node_holder remove(const_iterator pos) noexcept {
        auto node_ptr = pos.__get_np();
        if (begin_node_ == pos.ptr_) {
            if (node_ptr->right_ != nullptr) {
                begin_node_ = static_cast<end_node_pointer>(node_ptr->right_);
            } else {
                begin_node_ = static_cast<end_node_pointer>(node_ptr->parent_);
            }
        }
        --size_;
        mstd::detail::tree_remove(endNode()->left_, static_cast<node_base_pointer>(node_ptr));
        return node_holder{
            node_ptr,
            typename node_holder::deleter_type{nodeAlloc(), true},
        };
    }

    // FIXME: Make this function const qualified. Unfortunately doing so
    // breaks existing code which uses non-const callable comparators.

    // Find key
    // If key exists, return the parent of the node of key and a reference to the pointer to the node of key.
    // If key doesn't exist, return the parent of the null leaf and a reference to the pointer to the null leaf.
    template <class KeyT>
    std::pair<end_node_pointer, node_base_pointer&> find_equivalent(const KeyT& key) const
    requires OrdersWithAtLeastWeakly<key_compare, KeyType_, KeyT> {
        auto node_ptr = root();

        if (node_ptr == nullptr) {
            auto end = endNode();
            return {
                end,
                end->left_,
            };
        }

        auto* node_base_ptr = root_ptr();
    
        while (true) {
            const std::weak_ordering comp_res = key_comp_(key, key_proj_(node_ptr->get_value()));
            if (std::is_lt(comp_res)) {
                if (node_ptr->left_ == nullptr) {
                    return {
                        static_cast<end_node_pointer>(node_ptr),
                        node_ptr->left_,
                    };
                }
                node_base_ptr = std::addressof(node_ptr->left_);
                node_ptr      = static_cast<node_pointer>(node_ptr->left_);
            } else if (std::is_gt(comp_res)) {
                if (node_ptr->right_ == nullptr) {
                    return {
                        static_cast<end_node_pointer>(node_ptr),
                        node_ptr->right_,
                    };
                }
                node_base_ptr = std::addressof(node_ptr->right_);
                node_ptr      = static_cast<node_pointer>(node_ptr->right_);
            } else {
                return {
                    static_cast<end_node_pointer>(node_ptr),
                    *node_base_ptr,
                };
            }
        }
    }

    // Find key
    // First check prior to hint.
    // Next check after hint.
    // Next do O(log N) search.
    // If key exists, return the parent of the node of key and a reference to the pointer to the node of key.
    // If key doesn't exist, return the parent of the null leaf and a reference to the pointer to the null leaf.
    template <class KeyT>
    std::pair<end_node_pointer, node_base_pointer&>
    find_equivalent(const_iterator hint, node_base_pointer& dummy, const KeyT& key)
    requires OrdersWithAtLeastWeakly<key_compare, KeyType_, KeyT>{
        const auto& hint_key = key_proj_(*hint);
        if (hint == end() || std::is_lt(key_comp_(key, hint_key))) { // check before
            // key < *hint
            const_iterator prior = hint;
            if (prior == begin() || (std::is_lt(key_comp_(key, key_proj_(*--prior))))) {
                // *prev(hint) < key < *hint
                if (hint.ptr_->left_ == nullptr) {
                    return {
                        hint.ptr_,
                        hint.ptr_->left_,
                    };
                }
                return {
                    prior.ptr_,
                    static_cast<node_pointer>(prior.ptr_)->right_,
                };
            }
            // key <= *prev(hint)
            return find_equivalent(key);
        }
    
        if (std::is_gt(key_comp_(key, hint_key))) { // check after
            // *hint < key
            const_iterator next = std::next(hint);
            if (next == end() || (std::is_lt(key_comp_(key, key_proj_(*next))))) {
                // *hint < key < *std::next(hint)
                if (hint.__get_np()->right_ == nullptr) {
                    return {
                        hint.ptr_,
                        static_cast<node_pointer>(hint.ptr_)->right_,
                    };
                }
                return {
                    next.ptr_,
                    next.ptr_->left_,
                };
            }
            // *next(hint) <= key
            return find_equivalent(key);
        }
    
        // else key == *hint
        dummy = static_cast<node_base_pointer>(hint.ptr_);
        return {
            hint.ptr_,
            dummy,
        };
    }

private:

    // Find lower_bound place to insert
    // Set parent to parent of null leaf
    // Return reference to null leaf
    node_base_pointer& findLeafLow_(end_node_pointer& parent, const value_type& value) {
        node_pointer node_ptr = root();
        if (node_ptr != nullptr) {
            while (true) {
                const auto& node_key = key_proj_(node_ptr->get_value());
                const auto& value_key = key_proj_(value);
                if (key_comp_(node_key, value_key) < 0) {
                    if (node_ptr->right_ != nullptr) {
                        node_ptr = static_cast<node_pointer>(node_ptr->right_);
                    } else {
                        parent = static_cast<end_node_pointer>(node_ptr);
                        return node_ptr->right_;
                    }
                } else {
                    if (node_ptr->left_ != nullptr) {
                        node_ptr = static_cast<node_pointer>(node_ptr->left_);
                    } else {
                        parent = static_cast<end_node_pointer>(node_ptr);
                        return parent->left_;
                    }
                }
            }
        }
        parent = endNode();
        return parent->left_;
    }

    // Find upper_bound place to insert
    // Set parent to parent of null leaf
    // Return reference to null leaf
    node_base_pointer& findLeafHigh_(end_node_pointer& parent, const value_type& value) {
        auto node_ptr = root();
        if (node_ptr != nullptr) {
            while (true) {
                const auto& node_key  = key_proj_(node_ptr->get_value());
                const auto& value_key = key_proj_(value);
                if (key_comp_(value_key, node_key) < 0) {
                    if (node_ptr->left_ != nullptr) {
                        node_ptr = static_cast<node_pointer>(node_ptr->left_);
                    } else {
                        parent = static_cast<end_node_pointer>(node_ptr);
                        return parent->left_;
                    }
                } else {
                    if (node_ptr->right_ != nullptr) {
                        node_ptr = static_cast<node_pointer>(node_ptr->right_);
                    } else {
                        parent = static_cast<end_node_pointer>(node_ptr);
                        return node_ptr->right_;
                    }
                }
            }
        }
        parent = endNode();
        return parent->left_;
    }

    // Find leaf place to insert closest to hint
    // First check prior to hint.
    // Next check after hint.
    // Next do O(log N) search.
    // Set parent to parent of null leaf
    // Return reference to null leaf
    node_base_pointer&
    findLeaf_(const_iterator hint, end_node_pointer& parent, const value_type& value) {
        auto& value_key = key_proj_(value);
        auto& hint_key = key_proj_(*hint);
        if (hint == end() || (key_comp_(hint_key, value_key) >= 0)) // check before
        {
            // value <= *hint
            const_iterator prior = hint;
            if (prior == begin() || (key_comp_(value_key, key_proj_(*--prior)) >= 0)) {
                // *prev(hint) <= value <= *hint
                if (hint.ptr_->left_ == nullptr) {
                    parent = static_cast<end_node_pointer>(hint.ptr_);
                    return parent->left_;
                } else {
                    parent = static_cast<end_node_pointer>(prior.ptr_);
                    return static_cast<node_base_pointer>(prior.ptr_)->right_;
                }
            }
            // value < *prev(hint)
            return findLeafHigh_(parent, value);
        }
        // else value > *hint
        return findLeafLow_(parent, value);
    }

    template <class... ArgsT>
    node_holder constructNode_(ArgsT&&... args) {
        auto& node_alloc = nodeAlloc();
        node_holder holder{
            node_traits::allocate(node_alloc, 1),
            typename node_holder::deleter_type(node_alloc),
        };
        std::construct_at(std::addressof(*holder), node_alloc, std::forward<ArgsT>(args)...);
        holder.get_deleter().value_constructed = true;
        return holder;
    }

    void destroy_(node_pointer node_ptr) noexcept {
        (TreeDeleter(node_alloc_))(node_ptr);
    }

    void moveAssign_(Tree& other) {
        key_comp_ = std::move(other.key_comp_);
        if (size_ != 0) {
            *root_ptr() = static_cast<node_base_pointer>(moveAssignTree(root(), other.root()));
        } else {
            *root_ptr() = static_cast<node_base_pointer>(moveConstructTree_(other.root()));
            if (root()) {
                root()->parent_ = endNode();
            }
        }
        begin_node_
            = endNode()->left_
            ? static_cast<end_node_pointer>(mstd::detail::tree_min(endNode()->left_))
            : endNode();
        size_ = other.size();
        other.clear(); // Ensure that other is in a valid state after moving out the keys
    }

    void moveAssignRelinking_(Tree& other) noexcept(
        std::is_nothrow_move_assignable<key_compare>::value
     && std::is_nothrow_move_assignable<node_allocator>::value
    ) {
        destroy_(static_cast<node_pointer>(endNode()->left_));
        begin_node_ = other.begin_node_;
        end_node_   = other.end_node_;
        if constexpr (node_traits::propagate_on_container_move_assignment::value) {
            nodeAlloc() = std::move(other.nodeAlloc());
        }
        size_       = other.size_;
        key_comp_ = std::move(other.key_comp_);
        if (size_ == 0) {
            begin_node_ = endNode();
        } else {
            endNode()->left_->parent_ = static_cast<end_node_pointer>(endNode());
            other.begin_node_         = other.endNode();
            other.endNode()->left_    = nullptr;
            other.size_               = 0;
        }
    }

    template <class ToT, class FromT>
    static void assignValue_(ToT& lhs, FromT&& rhs) {
        lhs = std::forward<FromT>(rhs);
    }

    class TreeDeleter {
        node_allocator& alloc_;

    public:
        TreeDeleter(node_allocator& alloc) : alloc_(alloc) {}

        void
        operator()(node_pointer node_ptr) {
            if (!node_ptr) {
                return;
            }

            (*this)(static_cast<node_pointer>(node_ptr->left_));

            auto right = node_ptr->right_;

            node_traits::destroy(alloc_, std::addressof(node_ptr->get_value()));
            node_traits::deallocate(alloc_, node_ptr, 1);

            (*this)(static_cast<node_pointer>(right));
        }
    };

    // This copy construction will always produce a correct red-black-tree assuming the incoming tree is correct, since we
    // copy the exact structure 1:1. Since this is for copy construction _only_ we know that we get a correct tree. If we
    // didn't get a correct tree, the invariants of Tree are broken and we have a much bigger problem than an improperly
    // balanced tree.
    template <class NodeConstructorT>
    node_pointer constructFromTree_(node_pointer src, NodeConstructorT construct) {
        if (!src) {
            return nullptr;
        }

        node_holder new_node = construct(src->get_value());

        std::unique_ptr<node, TreeDeleter> left{
            constructFromTree_(static_cast<node_pointer>(src->left_), construct),
            node_alloc_,
        };
        node_pointer right = constructFromTree_(static_cast<node_pointer>(src->right_), construct);

        node_pointer new_node_ptr = new_node.release();

        new_node_ptr->is_black_ = src->is_black_;
        new_node_ptr->left_     = static_cast<node_base_pointer>(left.release());
        new_node_ptr->right_    = static_cast<node_base_pointer>(right);
        if (new_node_ptr->left_) {
            new_node_ptr->left_->parent_ = static_cast<end_node_pointer>(new_node_ptr);
        }
        if (new_node_ptr->right_) {
            new_node_ptr->right_->parent_ = static_cast<end_node_pointer>(new_node_ptr);
        }
        return new_node_ptr;
    }

    node_pointer copyConstructTree_(node_pointer src) {
        return constructFromTree_(src, [this](const value_type& val) { return constructNode_(val); });
    }

    node_pointer moveConstructTree_(node_pointer src) {
        return constructFromTree_(src, [this](value_type& val) { return constructNode_(std::move(val)); });
    }

    template <class AssignmentT, class ConstructionAlgT>
    // This copy assignment will always produce a correct red-black-tree assuming the incoming tree is correct, since our
    // own tree is a red-black-tree and the incoming tree is a red-black-tree. The invariants of a red-black-tree are
    // temporarily not met until all of the incoming red-black tree is copied.
    node_pointer assignFromTree(
        node_pointer dest, node_pointer src, AssignmentT assign, ConstructionAlgT construct_subtree) {
        if (!src) {
            destroy_(dest);
            return nullptr;
        }

        assign(dest->get_value(), src->get_value());
        dest->is_black_ = src->is_black_;

        // If we already have a left node in the destination tree, reuse it and copy-assign recursively
        if (dest->left_) {
            dest->left_
                = static_cast<node_base_pointer>(assignFromTree(
                                                 static_cast<node_pointer>(dest->left_),
                                                 static_cast<node_pointer>(src->left_),
                                                 assign,
                                                 construct_subtree));
            // Otherwise, we must create new nodes; copy-construct from here on
        } else if (src->left_) {
            auto new_left     = construct_subtree(static_cast<node_pointer>(src->left_));
            dest->left_       = static_cast<node_base_pointer>(new_left);
            new_left->parent_ = static_cast<end_node_pointer>(dest);
        }

        // Identical to the left case above, just for the right nodes
        if (dest->right_) {
            dest->right_
                = static_cast<node_base_pointer>(assignFromTree(
                                                 static_cast<node_pointer>(dest->right_),
                                                 static_cast<node_pointer>(src->right_),
                                                 assign,
                                                 construct_subtree));
        } else if (src->right_) {
            auto new_right     = construct_subtree(static_cast<node_pointer>(src->right_));
            dest->right_       = static_cast<node_base_pointer>(new_right);
            new_right->parent_ = static_cast<end_node_pointer>(dest);
        }

        return dest;
    }

    node_pointer copyAssignTree(node_pointer dest, node_pointer src) {
        return assignFromTree(
            dest,
            src,
            [](value_type& lhs, const value_type& rhs) { assignValue_(lhs, rhs); },
            [this](node_pointer node_ptr) { return copyConstructTree_(node_ptr); }
        );
    }

    node_pointer moveAssignTree(node_pointer dest, node_pointer src) {
        return assignFromTree(
            dest,
            src,
            [](value_type& lhs, value_type& rhs) { assignValue_(lhs, std::move(rhs)); },
            [this](node_pointer node_ptr) { return moveConstructTree_(node_ptr); }
        );
    }
};

} // namespace mstd


#endif // MSTD___TREE2
