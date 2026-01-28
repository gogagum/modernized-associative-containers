
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MSTD___TREE
#define MSTD___TREE

#include <algorithm>
#include <cassert>
#include <utility>
#include <iterator>
#include <memory>
#include <type_traits>
#include <new>
#include <limits>
#include <functional>

#include <detail/algorithm/specialized_algorithms.hpp>
#include <detail/config.hpp>
#include <detail/utility/try_key_extraction.hpp>
#include <detail/type_traits/is_specialization.hpp>
#include <detail/type_traits/copy_cvref.hpp>
#include <detail/type_traits/make_transparent.hpp>
#include <detail/memory/pointer_traits.hpp>
#include <detail/memory/allocator_traits.hpp>
#include <detail/memory/compressed_pair.hpp>
#include <detail/memory/swap_allocator.hpp>
#include <detail/utility/lazy_synth_three_way_comparator.hpp>


#define MSTD_ASSERT_INTERNAL(stmt, message) assert((stmt) && (message));

// GCC complains about the backslashes at the end, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=121528
// Tree is a red-black-tree implementation used for the associative containers (i.e. (multi)map/set). It stores
// - (1) a pointer to the node with the smallest (i.e. leftmost) element, namely __begin_node_
// - (2) the number of nodes in the tree, namely __size_
// - (3) a pointer to the root of the tree, namely __end_node_
//
// Storing (1) and (2) is required to allow for constant time lookups. A tree looks like this in memory:
//
//       __end_node_
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
// All nodes except __end_node_ have a __left_ and __right_ pointer as well as a __parent_ pointer.
// __end_node_ only contains a __left_ pointer, which points to the root of the tree.
// This layout allows for iteration through the tree without a need for special handling of the end node. See
// tree_next_iter and tree_prev_iter for more details.

namespace mstd {

template <class _Pointer>
class TreeEndNode;
template <class VoidPtrT>
class TreeNodeBase;
template <class _Tp, class VoidPtrT>
class TreeNode;

template <class KeyT, class _Value>
struct ValueType;

/*
 *
 * NodePtrT algorithms
 *
 * The algorithms taking NodePtrT are red black tree algorithms.  Those
 * algorithms taking a parameter named root should assume that root
 * points to a proper red black tree (unless otherwise specified).
 *
 * Each algorithm herein assumes that root->__parent_ points to a non-null
 * structure which has a member __left_ which points back to root.  No other
 * member is read or written to at root->__parent_.
 *
 * root->__parent_ will be referred to below (in comments only) as endNode.
 * endNode->__left_ is an externally accessible lvalue for root, and can be
 * changed by node insertion and removal (without explicit reference to endNode).
 *
 * All nodes (with the exception of endNode), even the node referred to as
 * root, have a non-null __parent_ field.
 *
 */

// Returns:  true if __x is a left child of its parent, else false
// Precondition:  __x != nullptr.
template <class NodePtrT>
inline bool tree_is_left_child(NodePtrT __x) noexcept {
    return __x == __x->__parent_->__left_;
}

// Determines if the subtree rooted at node_ptr is a proper red black subtree.  If
//    node_ptr is a proper subtree, returns the black height (null counts as 1).  If
//    node_ptr is an improper subtree, returns 0.
template <class NodePtrT>
unsigned tree_sub_invariant(NodePtrT node_ptr) {
    if (node_ptr == nullptr) {
        return 1;
    }
    // parent consistency checked by caller
    // check node_ptr->__left_ consistency
    if (node_ptr->__left_ != nullptr && node_ptr->__left_->__parent_ != node_ptr) {
        return 0;
    }
    // check node_ptr->__right_ consistency
    if (node_ptr->__right_ != nullptr && node_ptr->__right_->__parent_ != node_ptr) {
        return 0;
    }
    // check node_ptr->__left_ != node_ptr->__right_ unless both are nullptr
    if (node_ptr->__left_ == node_ptr->__right_ && node_ptr->__left_ != nullptr) {
        return 0;
    }
    // If this is red, neither child can be red
    if (!node_ptr->__is_black_) {
        if (node_ptr->__left_ && !node_ptr->__left_->__is_black_) {
            return 0;
        }
        if (node_ptr->__right_ && !node_ptr->__right_->__is_black_) {
            return 0;
        }
    }
    unsigned __h = mstd::tree_sub_invariant(node_ptr->__left_);
    if (__h == 0) {
        return 0; // invalid left subtree
    }
    if (__h != mstd::tree_sub_invariant(node_ptr->__right_)) {
        return 0;                    // invalid or different height right subtree
    }
    return __h + node_ptr->__is_black_; // return black height of this node
}

// Determines if the red black tree rooted at root is a proper red black tree.
//    root == nullptr is a proper tree.  Returns true if root is a proper
//    red black tree, else returns false.
template <class NodePtrT>
bool tree_invariant(NodePtrT root) {
    if (root == nullptr) {
        return true;
    }
    // check node_ptr->__parent_ consistency
    if (root->__parent_ == nullptr) {
        return false;
    }
    if (!mstd::tree_is_left_child(root)) {
        return false;
    }
    // root must be black
    if (!root->__is_black_) {
        return false;
    }
    // do normal node checks
    return mstd::tree_sub_invariant(root) != 0;
}

// Returns:  pointer to the left-most node under node_ptr.
template <class NodePtrT>
inline NodePtrT tree_min(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "Root node shouldn't be null");
    while (node_ptr->__left_ != nullptr) {
        node_ptr = node_ptr->__left_;
    }
    return node_ptr;
}

// Returns:  pointer to the right-most node under node_ptr.
template <class NodePtrT>
inline NodePtrT tree_max(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "Root node shouldn't be null");
    while (node_ptr->__right_ != nullptr) {
        node_ptr = node_ptr->__right_;
    }
    return node_ptr;
}

// Returns:  pointer to the next in-order node after node_ptr.
template <class NodePtrT>
NodePtrT tree_next(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "node shouldn't be null");
    if (node_ptr->__right_ != nullptr) {
        return mstd::tree_min(node_ptr->__right_);
    }
    while (!mstd::tree_is_left_child(node_ptr)) {
        node_ptr = node_ptr->parent_unsafe();
    }
    return node_ptr->parent_unsafe();
}

// tree_next_iter and tree_prev_iter implement iteration through the tree. The order is as follows:
// left sub-tree -> node -> right sub-tree. When the right-most node of a sub-tree is reached, we walk up the tree until
// we find a node where we were in the left sub-tree. We are _always_ in a left sub-tree, since the __end_node_ points
// to the actual root of the tree through a __left_ pointer. Incrementing the end() pointer is UB, so we can assume that
// never happens.
template <class EndNodePtrT, class NodePtrT>
inline EndNodePtrT tree_next_iter(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "node shouldn't be null");
    if (node_ptr->__right_ != nullptr) {
        return static_cast<EndNodePtrT>(mstd::tree_min(node_ptr->__right_));
    }
    while (!mstd::tree_is_left_child(node_ptr)) {
        node_ptr = node_ptr->parent_unsafe();
    }
    return static_cast<EndNodePtrT>(node_ptr->__parent_);
}

// Returns:  pointer to the previous in-order node before __x.
// Note: node_ptr may be the end node.
template <class NodePtrT, class EndNodePtrT>
inline NodePtrT tree_prev_iter(EndNodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "node shouldn't be null");
    if (node_ptr->__left_ != nullptr) {
        return mstd::tree_max(node_ptr->__left_);
    }
    auto curr_node = static_cast<NodePtrT>(node_ptr);
    while (mstd::tree_is_left_child(curr_node)) {
        curr_node = curr_node->parent_unsafe();
    }
    return curr_node->parent_unsafe();
}

// Effects:  Makes node_ptr->__right_ the subtree root with node_ptr as its left child
//           while preserving in-order order.
template <class NodePtrT>
void tree_left_rotate(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "node shouldn't be null");
    MSTD_ASSERT_INTERNAL(node_ptr->__right_ != nullptr, "node should have a right child");
    NodePtrT y  = node_ptr->__right_;
    node_ptr->__right_ = y->__left_;
    if (node_ptr->__right_ != nullptr) {
        node_ptr->__right_->set_parent(node_ptr);
    }
    y->__parent_ = node_ptr->__parent_;
    if (mstd::tree_is_left_child(node_ptr)) {
        node_ptr->__parent_->__left_ = y;
    } else {
        node_ptr->parent_unsafe()->__right_ = y;
    }
    y->__left_ = node_ptr;
    node_ptr->set_parent(y);
}

// Effects:  Makes node_ptr->__left_ the subtree root with node_ptr as its right child
//           while preserving in-order order.
template <class NodePtrT>
void tree_right_rotate(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "node shouldn't be null");
    MSTD_ASSERT_INTERNAL(node_ptr->__left_ != nullptr, "node should have a left child");
    NodePtrT y = node_ptr->__left_;
    node_ptr->__left_ = y->__right_;
    if (node_ptr->__left_ != nullptr) {
        node_ptr->__left_->set_parent(node_ptr);
    }
    y->__parent_ = node_ptr->__parent_;
    if (mstd::tree_is_left_child(node_ptr)) {
        node_ptr->__parent_->__left_ = y;
    } else {
        node_ptr->parent_unsafe()->__right_ = y;
    }
    y->__right_ = node_ptr;
    node_ptr->set_parent(y);
}

// Effects:  Rebalances root after attaching node_ptr to a leaf.
// Precondition:  node_ptr has no children.
//                node_ptr == root or == a direct or indirect child of root.
//                If node_ptr were to be unlinked from root (setting root to
//                  nullptr if root == node_ptr), tree_invariant(root) == true.
// Postcondition: tree_invariant(endNode->__left_) == true.  endNode->__left_
//                may be different than the value passed in as root.
template <class NodePtrT>
void tree_balance_after_insert(NodePtrT root, NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(root != nullptr, "Root of the tree shouldn't be null");
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "Can't attach null node to a leaf");
    node_ptr->__is_black_ = node_ptr == root;
    while (node_ptr != root && !node_ptr->parent_unsafe()->__is_black_) {
        // node_ptr->__parent_ != root because node_ptr->__parent_->__is_black == false
        if (mstd::tree_is_left_child(node_ptr->parent_unsafe())) {
            NodePtrT __y = node_ptr->parent_unsafe()->parent_unsafe()->__right_;
            if (__y != nullptr && !__y->__is_black_) {
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->__is_black_ = true;
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->__is_black_ = node_ptr == root;
                __y->__is_black_ = true;
            } else {
                if (!mstd::tree_is_left_child(node_ptr)) {
                    node_ptr = node_ptr->parent_unsafe();
                    mstd::tree_left_rotate(node_ptr);
                }
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->__is_black_ = true;
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->__is_black_ = false;
                mstd::tree_right_rotate(node_ptr);
                break;
            }
        } else {
            NodePtrT __y = node_ptr->parent_unsafe()->__parent_->__left_;
            if (__y != nullptr && !__y->__is_black_) {
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->__is_black_ = true;
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->__is_black_ = node_ptr == root;
                __y->__is_black_ = true;
            } else {
                if (mstd::tree_is_left_child(node_ptr)) {
                    node_ptr = node_ptr->parent_unsafe();
                    mstd::tree_right_rotate(node_ptr);
                }
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->__is_black_ = true;
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->__is_black_ = false;
                mstd::tree_left_rotate(node_ptr);
                break;
            }
        }
    }
}

// Precondition:  node_ptr == root or == a direct or indirect child of root.
// Effects:  unlinks node_ptr from the tree rooted at root, rebalancing as needed.
// Postcondition: tree_invariant(endNode->__left_) == true && endNode->__left_
//                nor any of its children refer to node_ptr.  endNode->__left_
//                may be different than the value passed in as root.
template <class NodePtrT>
void __tree_remove(NodePtrT root, NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(root != nullptr, "Root node should not be null");
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "The node to remove should not be null");
    MSTD_ASSERT_INTERNAL(mstd::tree_invariant(root), "The tree invariants should hold");
    // node_ptr will be removed from the tree.  Client still needs to destruct/deallocate it
    // __y is either node_ptr, or if node_ptr has two children, tree_next(node_ptr).
    // __y will have at most one child.
    // __y will be the initial hole in the tree (make the hole at a leaf)
    NodePtrT __y = (node_ptr->__left_ == nullptr || node_ptr->__right_ == nullptr) ? node_ptr : mstd::tree_next(node_ptr);
    // __x is __y's possibly null single child
    NodePtrT __x = __y->__left_ != nullptr ? __y->__left_ : __y->__right_;
    // __w is __x's possibly null uncle (will become __x's sibling)
    NodePtrT __w = nullptr;
    // link __x to __y's parent, and find __w
    if (__x != nullptr)
        __x->__parent_ = __y->__parent_;
    if (mstd::tree_is_left_child(__y)) {
        __y->__parent_->__left_ = __x;
        if (__y != root)
            __w = __y->parent_unsafe()->__right_;
        else
            root = __x; // __w == nullptr
    } else {
        __y->parent_unsafe()->__right_ = __x;
        // __y can't be root if it is a right child
        __w = __y->__parent_->__left_;
    }
    bool __removed_black = __y->__is_black_;
    // If we didn't remove node_ptr, do so now by splicing in __y for node_ptr,
    //    but copy node_ptr's color.  This does not impact __x or __w.
    if (__y != node_ptr) {
        // node_ptr->__left_ != nullptr but node_ptr->__right_ might == __x == nullptr
        __y->__parent_ = node_ptr->__parent_;
        if (mstd::tree_is_left_child(node_ptr))
            __y->__parent_->__left_ = __y;
        else
            __y->parent_unsafe()->__right_ = __y;
        __y->__left_ = node_ptr->__left_;
        __y->__left_->set_parent(__y);
        __y->__right_ = node_ptr->__right_;
        if (__y->__right_ != nullptr)
            __y->__right_->set_parent(__y);
        __y->__is_black_ = node_ptr->__is_black_;
        if (root == node_ptr)
            root = __y;
    }
    // There is no need to rebalance if we removed a red, or if we removed
    //     the last node.
    if (__removed_black && root != nullptr) {
        // Rebalance:
        // __x has an implicit black color (transferred from the removed __y)
        //    associated with it, no matter what its color is.
        // If __x is root (in which case it can't be null), it is supposed
        //    to be black anyway, and if it is doubly black, then the double
        //    can just be ignored.
        // If __x is red (in which case it can't be null), then it can absorb
        //    the implicit black just by setting its color to black.
        // Since __y was black and only had one child (which __x points to), __x
        //   is either red with no children, else null, otherwise __y would have
        //   different black heights under left and right pointers.
        // if (__x == root || __x != nullptr && !__x->__is_black_)
        if (__x != nullptr)
            __x->__is_black_ = true;
        else {
            //  Else __x isn't root, and is "doubly black", even though it may
            //     be null.  __w can not be null here, else the parent would
            //     see a black height >= 2 on the __x side and a black height
            //     of 1 on the __w side (__w must be a non-null black or a red
            //     with a non-null black child).
            while (true) {
                if (!mstd::tree_is_left_child(__w)) // if node_ptr is left child
                {
                    if (!__w->__is_black_) {
                        __w->__is_black_                    = true;
                        __w->parent_unsafe()->__is_black_ = false;
                        mstd::tree_left_rotate(__w->parent_unsafe());
                        // __x is still valid
                        // reset root only if necessary
                        if (root == __w->__left_)
                            root = __w;
                        // reset sibling, and it still can't be null
                        __w = __w->__left_->__right_;
                    }
                    // __w->__is_black_ is now true, __w may have null children
                    if ((__w->__left_ == nullptr || __w->__left_->__is_black_) &&
                        (__w->__right_ == nullptr || __w->__right_->__is_black_)) {
                        __w->__is_black_ = false;
                    __x              = __w->parent_unsafe();
                    // __x can no longer be null
                    if (__x == root || !__x->__is_black_) {
                        __x->__is_black_ = true;
                        break;
                    }
                    // reset sibling, and it still can't be null
                    __w = mstd::tree_is_left_child(__x) ? __x->parent_unsafe()->__right_ : __x->__parent_->__left_;
                    // continue;
                        } else // __w has a red child
                        {
                            if (__w->__right_ == nullptr || __w->__right_->__is_black_) {
                                // __w left child is non-null and red
                                __w->__left_->__is_black_ = true;
                                __w->__is_black_          = false;
                                mstd::tree_right_rotate(__w);
                                // __w is known not to be root, so root hasn't changed
                                // reset sibling, and it still can't be null
                                __w = __w->parent_unsafe();
                            }
                            // __w has a right red child, left child may be null
                            __w->__is_black_                    = __w->parent_unsafe()->__is_black_;
                            __w->parent_unsafe()->__is_black_ = true;
                            __w->__right_->__is_black_          = true;
                            mstd::tree_left_rotate(__w->parent_unsafe());
                            break;
                        }
                } else {
                    if (!__w->__is_black_) {
                        __w->__is_black_                    = true;
                        __w->parent_unsafe()->__is_black_ = false;
                        mstd::tree_right_rotate(__w->parent_unsafe());
                        // __x is still valid
                        // reset root only if necessary
                        if (root == __w->__right_)
                            root = __w;
                        // reset sibling, and it still can't be null
                        __w = __w->__right_->__left_;
                    }
                    // __w->__is_black_ is now true, __w may have null children
                    if ((__w->__left_ == nullptr || __w->__left_->__is_black_) &&
                        (__w->__right_ == nullptr || __w->__right_->__is_black_)) {
                        __w->__is_black_ = false;
                    __x              = __w->parent_unsafe();
                    // __x can no longer be null
                    if (!__x->__is_black_ || __x == root) {
                        __x->__is_black_ = true;
                        break;
                    }
                    // reset sibling, and it still can't be null
                    __w = mstd::tree_is_left_child(__x) ? __x->parent_unsafe()->__right_ : __x->__parent_->__left_;
                    // continue;
                        } else // __w has a red child
                        {
                            if (__w->__left_ == nullptr || __w->__left_->__is_black_) {
                                // __w right child is non-null and red
                                __w->__right_->__is_black_ = true;
                                __w->__is_black_           = false;
                                mstd::tree_left_rotate(__w);
                                // __w is known not to be root, so root hasn't changed
                                // reset sibling, and it still can't be null
                                __w = __w->parent_unsafe();
                            }
                            // __w has a left red child, right child may be null
                            __w->__is_black_                    = __w->parent_unsafe()->__is_black_;
                            __w->parent_unsafe()->__is_black_ = true;
                            __w->__left_->__is_black_           = true;
                            mstd::tree_right_rotate(__w->parent_unsafe());
                            break;
                        }
                }
            }
        }
    }
}

// node traits

template <class _Tp>
inline const bool __is_tree_value_type_v = __is_specialization_v<_Tp, ValueType>;

template <class _Tp>
struct __get_tree_key_type {
    using type = _Tp;
};

template <class KeyT, class ValueT>
struct __get_tree_key_type<ValueType<KeyT, ValueT> > {
    using type = KeyT;
};

template <class _Tp>
using __get_tree_key_type_t = typename __get_tree_key_type<_Tp>::type;

template <class _Tp>
struct __get_node_value_type {
    using type = _Tp;
};

template <class KeyT, class ValueT>
struct __get_node_value_type<ValueType<KeyT, ValueT> > {
    using type = std::pair<const KeyT, ValueT>;
};

template <class _Tp>
using __get_node_value_type_t = typename __get_node_value_type<_Tp>::type;

template <class NodePtrT, class _NodeT = typename std::pointer_traits<NodePtrT>::element_type>
struct __tree_node_types;

template <class NodePtrT, class _Tp, class VoidPtrT>
struct __tree_node_types<NodePtrT, TreeNode<_Tp, VoidPtrT> > {
    using node_base_pointer = __rebind_pointer_t<VoidPtrT, TreeNodeBase<VoidPtrT> >;
    using end_node_pointer  = __rebind_pointer_t<VoidPtrT, TreeEndNode<node_base_pointer> >;

private:
    static_assert(std::is_same_v<typename std::pointer_traits<VoidPtrT>::element_type, void>,
                  "VoidPtrT does not point to unqualified void type");
};

// node

template <class _Pointer>
class TreeEndNode {
public:
    using pointer = _Pointer;
    pointer __left_;

    TreeEndNode() noexcept : __left_() {}
};

template <class VoidPtrT>
class TreeNodeBase : public TreeEndNode<__rebind_pointer_t<VoidPtrT, TreeNodeBase<VoidPtrT> > > {
public:
    using pointer            = __rebind_pointer_t<VoidPtrT, TreeNodeBase>;
    using end_node_pointer = __rebind_pointer_t<VoidPtrT, TreeEndNode<pointer> >;

    pointer __right_;
    end_node_pointer __parent_;
    bool __is_black_;

    pointer parent_unsafe() const { return static_cast<pointer>(__parent_); }

    void set_parent(pointer parent) { __parent_ = static_cast<end_node_pointer>(parent); }

    TreeNodeBase()             = default;
    TreeNodeBase(TreeNodeBase const&)            = delete;
    TreeNodeBase& operator=(TreeNodeBase const&) = delete;
};

template <class _Tp, class VoidPtrT>
class TreeNode : public TreeNodeBase<VoidPtrT> {
public:
    using __node_value_type = __get_node_value_type_t<_Tp>;

    // We use a union to avoid initialization during member initialization, which allows us
    // to use the allocator from the container to construct the `__node_value_type` in the
    // memory provided by the union member

private:
    union {
        __node_value_type __value_;
    };

public:
    __node_value_type& get_value() { return __value_; }

    template <class _Alloc, class... ArgsT>
    explicit TreeNode(_Alloc& __na, ArgsT&&... args) {
        std::allocator_traits<_Alloc>::construct(__na, std::addressof(get_value()), std::forward<ArgsT>(args)...);
    }
    ~TreeNode()                             = delete;
    TreeNode(TreeNode const&)            = delete;
    TreeNode& operator=(TreeNode const&) = delete;
};

template <class AllocatorT>
class TreeNodeDestructor {
    using allocator_type_ = AllocatorT;
    using alloc_traits_   = std::allocator_traits<allocator_type_>;

public:
    using pointer = typename alloc_traits_::pointer;

private:
    allocator_type_& na_;

public:
    bool value_constructed;

    TreeNodeDestructor(const TreeNodeDestructor&)            = default;
    TreeNodeDestructor& operator=(const TreeNodeDestructor&) = delete;

    explicit TreeNodeDestructor(allocator_type_& na, bool val = false) noexcept
    : na_(na)
    , value_constructed(val) {}

    void operator()(pointer ptr) noexcept {
        if (value_constructed) {
            alloc_traits_::destroy(na_, std::addressof(ptr->get_value()));
        }
        if (ptr) {
            alloc_traits_::deallocate(na_, ptr, 1);
        }
    }

    template <class>
    friend class MapNodeDestructor;
};

template <class _NodeType, class _Alloc>
struct __generic_container_node_destructor;
template <class _Tp, class VoidPtrT, class _Alloc>
struct __generic_container_node_destructor<TreeNode<_Tp, VoidPtrT>, _Alloc> : TreeNodeDestructor<_Alloc> {
    using TreeNodeDestructor<_Alloc>::TreeNodeDestructor;
};

// Do an in-order traversal of the tree until `brk` returns true. Takes the root node of the tree.
template <class ReferenceT, class BreakT, class NodePtrT, class FuncT, class ProjT>
bool tree_iterate_from_root(BreakT brk, NodePtrT root, FuncT& func, ProjT& proj) {
    if (root->__left_) {
        if (mstd::tree_iterate_from_root<ReferenceT>(brk, static_cast<NodePtrT>(root->__left_), func, proj)) {
            return true;
        }
    }
    if (brk(root)) {
        return true;
    }
    std::invoke(func, std::invoke(proj, static_cast<ReferenceT>(root->get_value())));
    if (root->__right_) {
        return mstd::tree_iterate_from_root<ReferenceT>(brk, static_cast<NodePtrT>(root->__right_), func, proj);
    }
    return false;
}

// Do an in-order traversal of the tree from __first to __last.
template <class NodeIterT, class FuncT, class ProjT>
void tree_iterate_subrange(NodeIterT begin, NodeIterT end, FuncT& func, ProjT& proj) {
    using NodePtrT  = typename NodeIterT::node_pointer;
    using Reference = typename NodeIterT::reference;

    auto begin_node = begin.__ptr_;
    auto end_node   = end.__ptr_;

    while (true) {
        if (begin_node == end_node) {
            return;
        }
        const auto nfirst = static_cast<NodePtrT>(begin_node);
        std::invoke(func, std::invoke(proj, static_cast<Reference>(nfirst->get_value())));
        if (nfirst->__right_) {
            if (mstd::tree_iterate_from_root<Reference>(
                    [&](NodePtrT node) -> bool { return node == end_node; },
                    static_cast<NodePtrT>(nfirst->__right_),
                    func,
                    proj)) {
                return;
            }
        }
        while (!mstd::tree_is_left_child(static_cast<NodePtrT>(begin_node))) {
            begin_node = static_cast<NodePtrT>(begin_node)->__parent_;
        }
        begin_node = static_cast<NodePtrT>(begin_node)->__parent_;
    }
}

template <class _Tp, class NodePtrT, class _DiffType>
class TreeIterator {
    using _NodeTypes        = __tree_node_types<NodePtrT>;
    using node_pointer      = NodePtrT;
    using node_base_pointer = typename _NodeTypes::node_base_pointer;
    using end_node_pointer  = typename _NodeTypes::end_node_pointer;

    end_node_pointer __ptr_;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = __get_node_value_type_t<_Tp>;
    using difference_type   = _DiffType;
    using reference         = value_type&;
    using pointer           = __rebind_pointer_t<NodePtrT, value_type>;

    TreeIterator() noexcept : __ptr_(nullptr) {}

    reference operator*() const {
        return __get_np()->get_value();
    }

    pointer operator->() const {
        return std::pointer_traits<pointer>::pointer_to(__get_np()->get_value());
    }

    TreeIterator& operator++() {
        __ptr_ = mstd::tree_next_iter<end_node_pointer>(static_cast<node_base_pointer>(__ptr_));
        return *this;
    }

    TreeIterator operator++(int) {
        TreeIterator iter(*this);
        ++(*this);
        return iter;
    }

    TreeIterator& operator--() {
        __ptr_ = static_cast<end_node_pointer>(mstd::tree_prev_iter<node_base_pointer>(__ptr_));
        return *this;
    }

    TreeIterator operator--(int) {
        TreeIterator iter(*this);
        --(*this);
        return iter;
    }

    friend bool operator==(const TreeIterator& lhs, const TreeIterator& rhs) {
        return lhs.__ptr_ == rhs.__ptr_;
    }

    friend bool operator!=(const TreeIterator& lhs, const TreeIterator& rhs) {
        return !(lhs == rhs);
    }

private:
    explicit TreeIterator(node_pointer ptr) noexcept : __ptr_(ptr) {}
    explicit TreeIterator(end_node_pointer ptr) noexcept : __ptr_(ptr) {}
    node_pointer __get_np() const { return static_cast<node_pointer>(__ptr_); }
    template <class, class, class>
    friend class Tree;
    template <class, class, class>
    friend class TreeConstIterator;

    template <class NodeIterT, class FuncT, class ProjT>
    friend void tree_iterate_subrange(NodeIterT, NodeIterT, FuncT&, ProjT&);
};

// This also handles {multi,}set::iterator, since they're just aliases to Tree::iterator
template <class _Tp, class NodePtrT, class _DiffType>
struct __specialized_algorithm<
_Algorithm::__for_each,
__iterator_pair<TreeIterator<_Tp, NodePtrT, _DiffType>, TreeIterator<_Tp, NodePtrT, _DiffType>>> {
    static const bool __has_algorithm = true;

    using __iterator = TreeIterator<_Tp, NodePtrT, _DiffType>;

    template <class FuncT, class ProjT>
    static void operator()(__iterator __first, __iterator __last, FuncT& __func, ProjT& __proj) {
        mstd::tree_iterate_subrange(__first, __last, __func, __proj);
    }
};

template <class _Tp, class NodePtrT, class _DiffType>
class TreeConstIterator {
    using _NodeTypes          = __tree_node_types<NodePtrT>;
    using node_pointer      = NodePtrT;
    using node_base_pointer = typename _NodeTypes::node_base_pointer;
    using end_node_pointer  = typename _NodeTypes::end_node_pointer;

    end_node_pointer __ptr_;

public:
    using iterator_category    = std::bidirectional_iterator_tag;
    using value_type           = __get_node_value_type_t<_Tp>;
    using difference_type      = _DiffType;
    using reference            = const value_type&;
    using pointer              = __rebind_pointer_t<NodePtrT, const value_type>;
    using __non_const_iterator = TreeIterator<_Tp, node_pointer, difference_type>;

    TreeConstIterator() noexcept : __ptr_(nullptr) {}

    TreeConstIterator(__non_const_iterator __p) noexcept : __ptr_(__p.__ptr_) {}

    reference operator*() const { return __get_np()->get_value(); }
    pointer operator->() const {
        return std::pointer_traits<pointer>::pointer_to(__get_np()->get_value());
    }

    TreeConstIterator& operator++() {
        __ptr_ = mstd::tree_next_iter<end_node_pointer>(static_cast<node_base_pointer>(__ptr_));
        return *this;
    }

    TreeConstIterator operator++(int) {
        TreeConstIterator __t(*this);
        ++(*this);
        return __t;
    }

    TreeConstIterator& operator--() {
        __ptr_ = static_cast<end_node_pointer>(mstd::tree_prev_iter<node_base_pointer>(__ptr_));
        return *this;
    }

    TreeConstIterator operator--(int) {
        TreeConstIterator __t(*this);
        --(*this);
        return __t;
    }

    friend bool operator==(const TreeConstIterator& __x, const TreeConstIterator& __y) {
        return __x.__ptr_ == __y.__ptr_;
    }

    friend bool operator!=(const TreeConstIterator& __x, const TreeConstIterator& __y) {
        return !(__x == __y);
    }

private:
    explicit TreeConstIterator(node_pointer __p) noexcept : __ptr_(__p) {}
    explicit TreeConstIterator(end_node_pointer __p) noexcept : __ptr_(__p) {}
    node_pointer __get_np() const { return static_cast<node_pointer>(__ptr_); }

    template <class, class, class>
    friend class Tree;

    template <class NodeIterT, class FuncT, class ProjT>
    friend void tree_iterate_subrange(NodeIterT, NodeIterT, FuncT&, ProjT&);
};

// This also handles {multi,}set::const_iterator, since they're just aliases to Tree::iterator
template <class _Tp, class NodePtrT, class _DiffType>
struct __specialized_algorithm<
_Algorithm::__for_each,
__iterator_pair<TreeConstIterator<_Tp, NodePtrT, _DiffType>, TreeConstIterator<_Tp, NodePtrT, _DiffType>>> {
    static const bool __has_algorithm = true;

    using __iterator = TreeConstIterator<_Tp, NodePtrT, _DiffType>;

    template <class FuncT, class ProjT>
    static void operator()(__iterator __first, __iterator __last, FuncT& __func, ProjT& __proj) {
        mstd::tree_iterate_subrange(__first, __last, __func, __proj);
    }
};

template <class _Tp, class _Compare>
int __diagnose_non_const_comparator();

template <class _Tp, class _Compare, class _Allocator>
class Tree {
public:
    using value_type     = __get_node_value_type_t<_Tp>;
    using value_compare  = _Compare;
    using allocator_type = _Allocator;

private:
    using __alloc_traits = std::allocator_traits<allocator_type>;
    using key_type                       = __get_tree_key_type_t<_Tp>;

public:
    using pointer         = typename __alloc_traits::pointer;
    using const_pointer   = typename __alloc_traits::const_pointer;
    using size_type       = typename __alloc_traits::size_type;
    using difference_type = typename __alloc_traits::difference_type;

    using void_pointer = typename __alloc_traits::void_pointer;

    using node       = TreeNode<_Tp, void_pointer>;
    using node_pointer = __rebind_pointer_t<void_pointer, node>;

    using node_base         = TreeNodeBase<void_pointer>;
    using node_base_pointer = __rebind_pointer_t<void_pointer, node_base>;

    using end_node_t       = TreeEndNode<node_base_pointer>;
    using end_node_pointer = __rebind_pointer_t<void_pointer, end_node_t>;

    using node_allocator = __rebind_alloc<__alloc_traits, node>;
    using node_traits    = std::allocator_traits<node_allocator>;

private:
    // check for sane allocator pointer rebinding semantics. Rebinding the
    // allocator for a new pointer type should be exactly the same as rebinding
    // the pointer using 'pointer_traits'.
    static_assert(std::is_same_v<node_pointer, typename node_traits::pointer>,
                  "Allocator does not rebind pointers in a sane manner.");
    using __node_base_allocator = __rebind_alloc<node_traits, node_base>;
    using __node_base_traits    = std::allocator_traits<__node_base_allocator>;
    static_assert(std::is_same_v<node_base_pointer, typename __node_base_traits::pointer>,
                  "Allocator does not rebind pointers in a sane manner.");

private:
    end_node_pointer __begin_node_;
    MSTD_COMPRESSED_PAIR(end_node_t, __end_node_, node_allocator, __node_alloc_);
    MSTD_COMPRESSED_PAIR(size_type, __size_, value_compare, __value_comp_);

public:
    end_node_pointer endNode() noexcept {
        return std::pointer_traits<end_node_pointer>::pointer_to(__end_node_);
    }
    end_node_pointer endNode() const noexcept {
        return std::pointer_traits<end_node_pointer>::pointer_to(const_cast<end_node_t&>(__end_node_));
    }
    node_allocator& nodeAlloc() noexcept { return __node_alloc_; }

private:
    const node_allocator& nodeAlloc() const noexcept { return __node_alloc_; }

public:
    allocator_type alloc() const noexcept { return allocator_type(nodeAlloc()); }

    size_type size() const noexcept { return __size_; }
    value_compare& value_comp() noexcept { return __value_comp_; }
    const value_compare& value_comp() const noexcept { return __value_comp_; }

    node_pointer root() const noexcept {
        return static_cast<node_pointer>(endNode()->__left_);
    }

    node_base_pointer* root_ptr() const noexcept {
        return std::addressof(endNode()->__left_);
    }

    using iterator       = TreeIterator<_Tp, node_pointer, difference_type>;
    using const_iterator = TreeConstIterator<_Tp, node_pointer, difference_type>;

    explicit Tree(const value_compare& comp) noexcept(
        std::is_nothrow_default_constructible<node_allocator>::value
     && std::is_nothrow_copy_constructible<value_compare>::value
    )
    : __size_(0)
    , __value_comp_(comp) {
        __begin_node_ = endNode();
    }

    explicit Tree(const allocator_type& alloc)
    : __begin_node_()
    , __node_alloc_(node_allocator(alloc))
    , __size_(0) {
        __begin_node_ = endNode();
    }

    Tree(const value_compare& comp, const allocator_type& alloc)
    : __begin_node_()
    , __node_alloc_(node_allocator(alloc))
    , __size_(0)
    , __value_comp_(comp) {
        __begin_node_ = endNode();
    }

    Tree(const Tree& __t);

    Tree(const Tree& other, const allocator_type& alloc)
    : __begin_node_(endNode())
    , __node_alloc_(alloc)
    , __size_(0)
    , __value_comp_(other.value_comp()) {
        if (other.size() == 0) {
            return;
        }

        *root_ptr()       = static_cast<node_base_pointer>(copy_construct_tree(other.root()));
        root()->__parent_ = endNode();
        __begin_node_       = static_cast<end_node_pointer>(mstd::tree_min(endNode()->__left_));
        __size_             = other.size();
    }

    Tree& operator=(const Tree& __t);
    template <class _ForwardIterator>
    void assign_unique(_ForwardIterator __first, _ForwardIterator __last);

    Tree(Tree&& __t) noexcept(
        std::is_nothrow_move_constructible<node_allocator>::value
     && std::is_nothrow_move_constructible<value_compare>::value
    );
    Tree(Tree&& __t, const allocator_type& __a);

    Tree& operator=(Tree&& other)
    noexcept(
        std::is_nothrow_move_assignable<value_compare>::value
     && (
            (
                node_traits::propagate_on_container_move_assignment::value
             && std::is_nothrow_move_assignable<node_allocator>::value
            )
         || std::allocator_traits<node_allocator>::is_always_equal::value
        )
    ) {
        moveAssign(other, std::integral_constant<bool, node_traits::propagate_on_container_move_assignment::value>());
        return *this;
    }

    ~Tree() {
        static_assert(std::is_copy_constructible<value_compare>::value, "Comparator must be copy-constructible.");
        destroy_(root());
    }

    iterator begin() noexcept { return iterator(__begin_node_); }
    const_iterator begin() const noexcept { return const_iterator(__begin_node_); }
    iterator end() noexcept { return iterator(endNode()); }
    const_iterator end() const noexcept { return const_iterator(endNode()); }

    size_type max_size() const noexcept {
        return std::min<size_type>(
            node_traits::max_size(nodeAlloc()),
            std::numeric_limits<difference_type >::max()
        );
    }

    void clear() noexcept {
        destroy_(root());
        __size_            = 0;
        __begin_node_      = endNode();
        endNode()->__left_ = nullptr;
    }

    void swap(Tree& __t)
    noexcept(std::is_nothrow_swappable_v<value_compare>);

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
        node_holder holder = constructNode_(std::forward<ArgsT>(args)...);
        end_node_pointer parent;
        node_base_pointer& child = __find_leaf(pos, parent, holder->get_value());
        insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
        return iterator(static_cast<node_pointer>(holder.release()));
    }

    template <class... ArgsT>
    std::pair<iterator, bool> emplaceUnique(ArgsT&&... args) {
        return mstd::__try_key_extraction<key_type>(
            [this](const key_type& key, ArgsT&&... args2) {
                auto [parent, child] = find_equal(key);
                auto ret             = static_cast<node_pointer>(child);
                bool inserted        = false;
                if (child == nullptr) {
                    node_holder holder = constructNode_(std::forward<ArgsT>(args2)...);
                    insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
                    ret      = holder.release();
                    inserted = true;
                }
                return std::pair{iterator(ret), inserted};
            },
            [this](ArgsT&&... args2) {
                node_holder holder = constructNode_(std::forward<ArgsT>(args2)...);
                auto [parent, child] = find_equal(holder->get_value());
                auto ret             = static_cast<node_pointer>(child);
                bool inserted        = false;
                if (child == nullptr) {
                    insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
                    ret      = holder.release();
                    inserted = true;
                }
                return std::pair{iterator(ret), inserted};
            },
            std::forward<ArgsT>(args)...);
    }

    template <class... ArgsT>
    std::pair<iterator, bool> emplaceHintUnique(const_iterator pos, ArgsT&&... args) {
        return mstd::__try_key_extraction<key_type>(
            [this, pos](const key_type& key, ArgsT&&... args2) {
                node_base_pointer dummy;
                auto [parent, child] = find_equal(pos, dummy, key);
                auto ret             = static_cast<node_pointer>(child);
                bool inserted        = false;
                if (child == nullptr) {
                    auto holder = constructNode_(std::forward<ArgsT>(args2)...);
                    insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
                    ret        = holder.release();
                    inserted = true;
                }
                return std::pair{
                    iterator(ret),
                    inserted,
                };
            },
            [this, pos](ArgsT&&... args2) {
                auto holder = constructNode_(std::forward<ArgsT>(args2)...);
                node_base_pointer dummy;
                auto [parent, child] = find_equal(pos, dummy, holder->get_value());
                auto ret             = static_cast<node_pointer>(child);
                if (child == nullptr) {
                    insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.get()));
                    ret = holder.release();
                }
                return std::pair{
                    iterator(ret),
                    child == nullptr,
                };
            },
            std::forward<ArgsT>(args)...);
    }

    template <class _InIter, class _Sent>
    void __insert_range_multi(_InIter begin, _Sent end) {
        if (begin == end) {
            return;
        }

        if (root() == nullptr) { // Make sure we always have a root node
            insertNodeAt(
                endNode(),
                endNode()->__left_,
                static_cast<node_base_pointer>(constructNode_(*begin).release())
            );
            ++begin;
        }

        auto max_node = static_cast<node_pointer>(mstd::tree_max(static_cast<node_base_pointer>(root())));

        for (; begin != end; ++begin) {
            node_holder holder = constructNode_(*begin);
            // Always check the max node first. This optimizes for sorted ranges inserted at the end.
            if (!value_comp()(holder->get_value(), max_node->get_value())) { // node >= __max_val
                insertNodeAt(static_cast<end_node_pointer>(max_node),
                             max_node->__right_,
                             static_cast<node_base_pointer>(holder.get()));
                max_node = holder.release();
            } else {
                end_node_pointer parent;
                node_base_pointer& child = findLeafHigh_(parent, holder->get_value());
                insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.release()));
            }
        }
    }

    template <class _InIter, class _Sent>
    void __insert_range_unique(_InIter __first, _Sent __last) {
        if (__first == __last)
            return;

        if (root() == nullptr) {
            insertNodeAt(
                endNode(), endNode()->__left_, static_cast<node_base_pointer>(constructNode_(*__first).release()));
            ++__first;
        }

        auto max_node = static_cast<node_pointer>(mstd::tree_max(static_cast<node_base_pointer>(root())));

        using __reference = decltype(*__first);

        for (; __first != __last; ++__first) {
            mstd::__try_key_extraction<key_type>(
                [this, &max_node](const key_type& key, __reference&& val) {
                    if (value_comp()(max_node->get_value(), key)) { // key > max_node
                        auto holder = constructNode_(std::forward<__reference>(val));
                        insertNodeAt(static_cast<end_node_pointer>(max_node),
                                     max_node->__right_,
                                     static_cast<node_base_pointer>(holder.get()));
                        max_node = holder.release();
                    } else {
                        auto [parent, child] = find_equal(key);
                        if (child == nullptr) {
                            auto holder = constructNode_(std::forward<__reference>(val));
                            insertNodeAt(parent, child, static_cast<node_base_pointer>(holder.release()));
                        }
                    }
                },
                [this, &max_node](__reference&& val) {
                    node_holder __nd = constructNode_(std::forward<__reference>(val));
                    if (value_comp()(max_node->get_value(), __nd->get_value())) { // node > max_node
                        insertNodeAt(static_cast<end_node_pointer>(max_node),
                                         max_node->__right_,
                                         static_cast<node_base_pointer>(__nd.get()));
                        max_node = __nd.release();
                    } else {
                        auto [parent, child] = find_equal(__nd->get_value());
                        if (child == nullptr) {
                            insertNodeAt(parent, child, static_cast<node_base_pointer>(__nd.release()));
                        }
                    }
                },
                *__first);
        }
    }

    iterator __remove_node_pointer(node_pointer) noexcept;

    template <class _NodeHandle, class _InsertReturnType>
    _InsertReturnType __node_handle_insert_unique(_NodeHandle&&);
    template <class _NodeHandle>
    iterator __node_handle_insert_unique(const_iterator, _NodeHandle&&);
    template <class _Comp2>
    void __node_handle_merge_unique(Tree<_Tp, _Comp2, _Allocator>& __source);

    template <class _NodeHandle>
    iterator __node_handle_insert_multi(_NodeHandle&&);
    template <class _NodeHandle>
    iterator __node_handle_insert_multi(const_iterator, _NodeHandle&&);
    template <class _Comp2>
    void __node_handle_merge_multi(Tree<_Tp, _Comp2, _Allocator>& __source);

    template <class _NodeHandle>
    _NodeHandle __node_handle_extract(key_type const&);
    template <class _NodeHandle>
    _NodeHandle __node_handle_extract(const_iterator);

    iterator erase(const_iterator pos) {
        auto node_ptr        = pos.__get_np();
        auto ret             = __remove_node_pointer(node_ptr);
        auto& node_allocator = nodeAlloc();
        node_traits::destroy(node_allocator, std::addressof(const_cast<value_type&>(*pos)));
        node_traits::deallocate(node_allocator, node_ptr, 1);
        return ret;
    }

    iterator erase(const_iterator begin, const_iterator end) {
        while (begin != end) {
            begin = erase(begin);
        }
        return iterator(end.__ptr_);
    }

    template <class KeyT>
    size_type __erase_unique(const KeyT& __k);
    template <class KeyT>
    size_type __erase_multi(const KeyT& __k);

    void insertNodeAt(end_node_pointer parent, node_base_pointer& child, node_base_pointer new_node) noexcept {
        new_node->__left_   = nullptr;
        new_node->__right_  = nullptr;
        new_node->__parent_ = parent;
        // new_node->__is_black_ is initialized in tree_balance_after_insert
        child = new_node;
        if (__begin_node_->__left_ != nullptr)
            __begin_node_ = static_cast<end_node_pointer>(__begin_node_->__left_);
        mstd::tree_balance_after_insert(endNode()->__left_, child);
        ++__size_;
    }

    template <class KeyT>
    iterator find(const KeyT& key) {
        auto [__, match] = find_equal(key);
        if (match == nullptr) {
            return end();
        }
        return iterator(static_cast<node_pointer>(match));
    }

    template <class KeyT>
    const_iterator find(const KeyT& key) const {
        auto [__, match] = find_equal(key);
        if (match == nullptr) {
            return end();
        }
        return const_iterator(static_cast<node_pointer>(match));
    }

    template <class KeyT>
    size_type countUnique(const KeyT& key) const {
        using Comp = LazySynthThreeWayComparator<value_compare, KeyT, value_type>;
        auto root_node = root();
        Comp comp(value_comp());
        while (root_node != nullptr) {
            const auto comp_res = comp(key, root_node->get_value());
            if (comp_res.__less()) {
                root_node = static_cast<node_pointer>(root_node->__left_);
            } else if (comp_res.__greater()) {
                root_node = static_cast<node_pointer>(root_node->__right_);
            } else {
                return 1;
            }
        }
        return 0;
    }

    template <class KeyT>
    size_type countMulti(const KeyT& key) const {
        using Comp = LazySynthThreeWayComparator<value_compare, KeyT, value_type>;
        auto result    = endNode();
        auto root_node = root();
        Comp comp(value_comp());
        while (root_node != nullptr) {
            const auto comp_res = comp(key, root_node->get_value());
            if (comp_res.__less()) {
                result = static_cast<end_node_pointer>(root_node);
                root_node   = static_cast<node_pointer>(root_node->__left_);
            } else if (comp_res.__greater()) {
                root_node = static_cast<node_pointer>(root_node->__right_);
            } else {
                return std::distance(
                    lowerBoundMulti_(key, static_cast<node_pointer>(root_node->__left_), static_cast<end_node_pointer>(root_node)),
                    upperBoundMulti_(key, static_cast<node_pointer>(root_node->__right_), result)
                );
            }
        }
        return 0;
    }

    template <class KeyT>
    iterator lowerBoundUnique(const KeyT& key) {
        return iterator(lowerUpperBoundUniqueImpl_<true>(key));
    }

    template <class KeyT>
    const_iterator lowerBoundUnique(const KeyT& key) const {
        return const_iterator(lowerUpperBoundUniqueImpl_<true>(key));
    }

    template <class KeyT>
    iterator upperBoundUnique(const KeyT& key) {
        return iterator(lowerUpperBoundUniqueImpl_<false>(key));
    }

    template <class KeyT>
    const_iterator upperBoundUnique(const KeyT& key) const {
        return const_iterator(lowerUpperBoundUniqueImpl_<false>(key));
    }

private:
    template <bool _LowerBound, class KeyT>
    end_node_pointer lowerUpperBoundUniqueImpl_(const KeyT& key) const {
        auto root_node = root();
        auto result    = endNode();
        auto comp      = LazySynthThreeWayComparator<_Compare, KeyT, value_type>(value_comp());
        while (root_node != nullptr) {
            const auto comp_res = comp(key, root_node->get_value());

            if (comp_res.__less()) {
                result = static_cast<end_node_pointer>(root_node);
                root_node     = static_cast<node_pointer>(root_node->__left_);
            } else if (comp_res.__greater()) {
                root_node = static_cast<node_pointer>(root_node->__right_);
            } else if constexpr (_LowerBound) {
                return static_cast<end_node_pointer>(root_node);
            } else {
                return root_node->__right_
                    ? static_cast<end_node_pointer>(mstd::tree_min(root_node->__right_))
                    : result;
            }
        }
        return result;
    }

    template <class KeyT>
    iterator lowerBoundMulti_(const KeyT& key, node_pointer root_node, end_node_pointer result) {
        while (root_node != nullptr) {
            if (!value_comp()(root_node->get_value(), key)) {
                result    = static_cast<end_node_pointer>(root_node);
                root_node = static_cast<node_pointer>(root_node->__left_);
            } else {
                root_node = static_cast<node_pointer>(root_node->__right_);
            }
        }
        return iterator(result);
    }

    template <class KeyT>
    const_iterator lowerBoundMulti_(const KeyT& key, node_pointer root_node, end_node_pointer result) const {
        while (root_node != nullptr) {
            if (!value_comp()(root_node->get_value(), key)) {
                result    = static_cast<end_node_pointer>(root_node);
                root_node = static_cast<node_pointer>(root_node->__left_);
            } else {
                root_node = static_cast<node_pointer>(root_node->__right_);
            }
        }
        return const_iterator(result);
    }

    template <class KeyT>
    iterator
    upperBoundMulti_(const KeyT& key, node_pointer root_node, end_node_pointer result) {
        while (root_node != nullptr) {
            if (value_comp()(key, root_node->get_value())) {
                result = static_cast<end_node_pointer>(root_node);
                root_node   = static_cast<node_pointer>(root_node->__left_);
            } else {
                root_node = static_cast<node_pointer>(root_node->__right_);
            }
        }
        return iterator(result);
    }

    template <class KeyT>
    const_iterator
    upperBoundMulti_(const KeyT& key, node_pointer root_node, end_node_pointer result) const {
        while (root_node != nullptr) {
            if (value_comp()(key, root_node->get_value())) {
                result = static_cast<end_node_pointer>(root_node);
                root_node   = static_cast<node_pointer>(root_node->__left_);
            } else {
                root_node = static_cast<node_pointer>(root_node->__right_);
            }
        }
        return const_iterator(result);
    }

public:
    template <class KeyT>
    iterator lowerBoundMulti(const KeyT& key) {
        return lowerBoundMulti_(key, root(), endNode());
    }
    template <class KeyT>
    const_iterator lowerBoundMulti(const KeyT& key) const {
        return lowerBoundMulti_(key, root(), endNode());
    }

    template <class KeyT>
    iterator upperBoundMulti(const KeyT& key) {
        return upperBoundMulti_(key, root(), endNode());
    }

    template <class KeyT>
    const_iterator upperBoundMulti(const KeyT& key) const {
        return upperBoundMulti_(key, root(), endNode());
    }

public:
    template <class KeyT>
    std::pair<iterator, iterator> equalRangeUnique(const KeyT& key) {
        auto result    = endNode();
        auto root_node = root();
        auto comp      = LazySynthThreeWayComparator<value_compare, KeyT, value_type>(value_comp());
        while (root_node != nullptr) {
            const auto comp_res = comp(key, root_node->get_value());
            if (comp_res.__less()) {
                result    = static_cast<end_node_pointer>(root_node);
                root_node = static_cast<node_pointer>(root_node->__left_);
            } else if (comp_res.__greater()) {
                root_node = static_cast<node_pointer>(root_node->__right_);
            } else {
                return {
                    iterator(root_node),
                    iterator(
                        (root_node->__right_ != nullptr)
                        ? static_cast<end_node_pointer>(mstd::tree_min(root_node->__right_))
                        : result
                    ),
                };
            }
        }
        return {
            iterator(result),
            iterator(result),
        };
    }

    template <class KeyT>
    std::pair<const_iterator, const_iterator> equalRangeUnique(const KeyT& key) const {
        auto result    = endNode();
        auto root_node = root();
        auto comp      = LazySynthThreeWayComparator<value_compare, KeyT, value_type>(value_comp());
        while (root_node != nullptr) {
            const auto comp_res = comp(key, root_node->get_value());
            if (comp_res.__less()) {
                result    = static_cast<end_node_pointer>(root_node);
                root_node = static_cast<node_pointer>(root_node->__left_);
            } else if (comp_res.__greater()) {
                root_node = static_cast<node_pointer>(root_node->__right_);
            } else {  // Equal
                return {
                    const_iterator(root_node),
                    const_iterator(
                        (root_node->__right_ != nullptr)
                        ? static_cast<end_node_pointer>(mstd::tree_min(root_node->__right_))
                        : result
                    ),
                };
            }
        }
        return {
            const_iterator(result),
            const_iterator(result),
        };
    }

    template <class KeyT>
    std::pair<iterator, iterator> equalRangeMulti(const KeyT& key) {
        auto result = endNode();
        auto root_node   = root();
        auto comp   = LazySynthThreeWayComparator<value_compare, KeyT, value_type>(value_comp());
        while (root_node != nullptr) {
            const auto comp_res = comp(key, root_node->get_value());
            if (comp_res.__less()) {
                result    = static_cast<end_node_pointer>(root_node);
                root_node = static_cast<node_pointer>(root_node->__left_);
            } else if (comp_res.__greater()) {
                root_node = static_cast<node_pointer>(root_node->__right_);
            } else {  // Equal
                return {
                    lowerBoundMulti_(key, static_cast<node_pointer>(root_node->__left_), static_cast<end_node_pointer>(root_node)),
                    upperBoundMulti_(key, static_cast<node_pointer>(root_node->__right_), result)
                };
            }
        }
        return {
            iterator(result),
            iterator(result),
        };
    }

    template <class KeyT>
    std::pair<const_iterator, const_iterator> equalRangeMulti(const KeyT& key) const {
        auto result    = endNode();
        auto root_node = root();
        auto comp      = LazySynthThreeWayComparator<value_compare, KeyT, value_type>(value_comp());
        while (root_node != nullptr) {
            const auto comp_res = comp(key, root_node->get_value());
            if (comp_res.__less()) {
                result = static_cast<end_node_pointer>(root_node);
                root_node   = static_cast<node_pointer>(root_node->__left_);
            } else if (comp_res.__greater()) {
                root_node = static_cast<node_pointer>(root_node->__right_);
            } else {  // Equal
                return {
                    lowerBoundMulti_(key, static_cast<node_pointer>(root_node->__left_), static_cast<end_node_pointer>(root_node)),
                    upperBoundMulti_(key, static_cast<node_pointer>(root_node->__right_), result),
                };
            }
        }
        return {
            const_iterator(result),
            const_iterator(result),
        };
    }

    using _Dp         = TreeNodeDestructor<node_allocator>;
    using node_holder = std::unique_ptr<node, _Dp>;

    node_holder remove(const_iterator pos) noexcept;

    // FIXME: Make this function const qualified. Unfortunately doing so
    // breaks existing code which uses non-const callable comparators.
    template <class KeyT>
    std::pair<end_node_pointer, node_base_pointer&> find_equal(const KeyT& __v);

    template <class KeyT>
    std::pair<end_node_pointer, node_base_pointer&> find_equal(const KeyT& __v) const {
        return const_cast<Tree*>(this)->find_equal(__v);
    }

    template <class KeyT>
    std::pair<end_node_pointer, node_base_pointer&>
    find_equal(const_iterator hint, node_base_pointer& dummy, const KeyT& __v);

    void copyAssignAlloc(const Tree& other) {
        copyAssignAlloc(other, std::integral_constant<bool, node_traits::propagate_on_container_copy_assignment::value>());
    }

    void copyAssignAlloc(const Tree& other, std::true_type) {
        if (nodeAlloc() != other.nodeAlloc()) {
            clear();
        }
        nodeAlloc() = other.nodeAlloc();
    }
    void copyAssignAlloc(const Tree&, std::false_type) {}

private:
    node_base_pointer& __find_leaf_low(end_node_pointer& parent, const value_type& __v);

    // Find upper_bound place to insert
    // Set parent to parent of null leaf
    // Return reference to null leaf
    node_base_pointer& findLeafHigh_(end_node_pointer& parent, const value_type& value) {
        auto node_ptr = root();
        if (node_ptr != nullptr) {
            while (true) {
                if (value_comp()(value, node_ptr->get_value())) {
                    if (node_ptr->__left_ != nullptr) {
                        node_ptr = static_cast<node_pointer>(node_ptr->__left_);
                    } else {
                        parent = static_cast<end_node_pointer>(node_ptr);
                        return parent->__left_;
                    }
                } else {
                    if (node_ptr->__right_ != nullptr) {
                        node_ptr = static_cast<node_pointer>(node_ptr->__right_);
                    } else {
                        parent = static_cast<end_node_pointer>(node_ptr);
                        return node_ptr->__right_;
                    }
                }
            }
        }
        parent = endNode();
        return parent->__left_;
    }

    node_base_pointer&
    __find_leaf(const_iterator hint, end_node_pointer& parent, const value_type& __v);

    template <class... ArgsT>
    node_holder constructNode_(ArgsT&&... args) {
        node_allocator& __na = nodeAlloc();
        node_holder __h(node_traits::allocate(__na, 1), _Dp(__na));
        std::construct_at(std::addressof(*__h), __na, std::forward<ArgsT>(args)...);
        __h.get_deleter().value_constructed = true;
        return __h;
    }

    void destroy_(node_pointer node_ptr) noexcept {
        (TreeDeleter(__node_alloc_))(node_ptr);
    }

    void moveAssign(Tree& __t, std::false_type);
    void moveAssign(Tree& __t, std::true_type) noexcept(
        std::is_nothrow_move_assignable<value_compare>::value
     && std::is_nothrow_move_assignable<node_allocator>::value
    );

    void __move_assign_alloc(Tree& other) noexcept(
        !node_traits::propagate_on_container_move_assignment::value
     || std::is_nothrow_move_assignable<node_allocator>::value
    ) {
        __move_assign_alloc(other, std::integral_constant<bool, node_traits::propagate_on_container_move_assignment::value>());
    }

    void __move_assign_alloc(Tree& __t, std::true_type)
    noexcept(std::is_nothrow_move_assignable<node_allocator>::value) {
        nodeAlloc() = std::move(__t.nodeAlloc());
    }
    void __move_assign_alloc(Tree&, std::false_type) noexcept {}

    template <
        class _From
      , class ValueT = _Tp
      , std::enable_if_t<__is_tree_value_type_v<ValueT>, int> = 0
    >
    static void assignValue(__get_node_value_type_t<value_type>& lhs, _From&& rhs) {
        using __key_type = std::remove_const_t<typename value_type::first_type>;

        // This is technically UB, since the object was constructed as `const`.
        // Clang doesn't optimize on this currently though.
        const_cast<__key_type&>(lhs.first) = const_cast<__copy_cvref_t<_From, __key_type>&&>(rhs.first);
        lhs.second                         = std::forward<_From>(rhs).second;
    }

    template <
        class _To
      , class _From
      , class ValueT = _Tp
      , std::enable_if_t<!__is_tree_value_type_v<ValueT>, int> = 0
    >
    static void assignValue(_To& lhs, _From&& rhs) {
        lhs = std::forward<_From>(rhs);
    }

    class TreeDeleter {
        node_allocator& __alloc_;

    public:
        TreeDeleter(node_allocator& alloc) : __alloc_(alloc) {}

        void
        operator()(node_pointer node_ptr) {
            if (!node_ptr) {
                return;
            }

            (*this)(static_cast<node_pointer>(node_ptr->__left_));

            auto right = node_ptr->__right_;

            node_traits::destroy(__alloc_, std::addressof(node_ptr->get_value()));
            node_traits::deallocate(__alloc_, node_ptr, 1);

            (*this)(static_cast<node_pointer>(right));
        }
    };

    // This copy construction will always produce a correct red-black-tree assuming the incoming tree is correct, since we
    // copy the exact structure 1:1. Since this is for copy construction _only_ we know that we get a correct tree. If we
    // didn't get a correct tree, the invariants of Tree are broken and we have a much bigger problem than an improperly
    // balanced tree.
    template <class _NodeConstructor>
    node_pointer __construct_from_tree(node_pointer __src, _NodeConstructor __construct) {
        if (!__src)
            return nullptr;

        node_holder new_node = __construct(__src->get_value());

        std::unique_ptr<node, TreeDeleter> __left(
            __construct_from_tree(static_cast<node_pointer>(__src->__left_), __construct), __node_alloc_);
        node_pointer right = __construct_from_tree(static_cast<node_pointer>(__src->__right_), __construct);

        node_pointer __new_node_ptr = new_node.release();

        __new_node_ptr->__is_black_ = __src->__is_black_;
        __new_node_ptr->__left_     = static_cast<node_base_pointer>(__left.release());
        __new_node_ptr->__right_    = static_cast<node_base_pointer>(right);
        if (__new_node_ptr->__left_)
            __new_node_ptr->__left_->__parent_ = static_cast<end_node_pointer>(__new_node_ptr);
        if (__new_node_ptr->__right_)
            __new_node_ptr->__right_->__parent_ = static_cast<end_node_pointer>(__new_node_ptr);
        return __new_node_ptr;
    }

    node_pointer copy_construct_tree(node_pointer src) {
        return __construct_from_tree(src, [this](const value_type& val) { return constructNode_(val); });
    }

    template <
        class ValueT = _Tp
      , std::enable_if_t<__is_tree_value_type_v<ValueT>, int> = 0
    >
    node_pointer move_construct_tree(node_pointer src) {
        return __construct_from_tree(src, [this](value_type& val) {
            return constructNode_(const_cast<key_type&&>(val.first), std::move(val.second));
        });
    }

    template <
        class ValueT = _Tp
      , std::enable_if_t<!__is_tree_value_type_v<ValueT>, int> = 0
    >
    node_pointer move_construct_tree(node_pointer src) {
        return __construct_from_tree(src, [this](value_type& val) {
            return constructNode_(std::move(val));
        });
    }

    template <class _Assignment, class _ConstructionAlg>
    // This copy assignment will always produce a correct red-black-tree assuming the incoming tree is correct, since our
    // own tree is a red-black-tree and the incoming tree is a red-black-tree. The invariants of a red-black-tree are
    // temporarily not met until all of the incoming red-black tree is copied.
    node_pointer assignFromTree(
        node_pointer dest, node_pointer src, _Assignment assign, _ConstructionAlg construct_subtree) {
        if (!src) {
            destroy_(dest);
            return nullptr;
        }

        assign(dest->get_value(), src->get_value());
        dest->__is_black_ = src->__is_black_;

        // If we already have a left node in the destination tree, reuse it and copy-assign recursively
        if (dest->__left_) {
            dest->__left_
                = static_cast<node_base_pointer>(assignFromTree(
                                                       static_cast<node_pointer>(dest->__left_),
                                                       static_cast<node_pointer>(src->__left_),
                                                       assign,
                                                       construct_subtree)
                  );

            // Otherwise, we must create new nodes; copy-construct from here on
        } else if (src->__left_) {
            auto __new_left       = construct_subtree(static_cast<node_pointer>(src->__left_));
            dest->__left_       = static_cast<node_base_pointer>(__new_left);
            __new_left->__parent_ = static_cast<end_node_pointer>(dest);
        }

        // Identical to the left case above, just for the right nodes
        if (dest->__right_) {
            dest->__right_
                = static_cast<node_base_pointer>(assignFromTree(
                                                       static_cast<node_pointer>(dest->__right_),
                                                       static_cast<node_pointer>(src->__right_),
                                                       assign,
                                                       construct_subtree)
                  );
        } else if (src->__right_) {
            auto new_right       = construct_subtree(static_cast<node_pointer>(src->__right_));
            dest->__right_       = static_cast<node_base_pointer>(new_right);
            new_right->__parent_ = static_cast<end_node_pointer>(dest);
        }

        return dest;
    }

    node_pointer copyAssignTree(node_pointer dest, node_pointer src) {
        return assignFromTree(
            dest,
            src,
            [](value_type& lhs, const value_type& rhs) { assignValue(lhs, rhs); },
            [this](node_pointer node_ptr) { return copy_construct_tree(node_ptr); }
        );
    }

    node_pointer moveAssignTree(node_pointer dest, node_pointer src) {
        return assignFromTree(
            dest,
            src,
            [](value_type& lhs, value_type& rhs) { assignValue(lhs, std::move(rhs)); },
            [this](node_pointer node_ptr) { return move_construct_tree(node_ptr); }
        );
    }

    friend struct __specialized_algorithm<_Algorithm::__for_each, __single_range<Tree> >;
};

template <class _Tp, class _Compare, class _Allocator>
struct __specialized_algorithm<_Algorithm::__for_each, __single_range<Tree<_Tp, _Compare, _Allocator> > > {
    static const bool __has_algorithm = true;

    using node_pointer = typename Tree<_Tp, _Compare, _Allocator>::node_pointer;

    template <class _Tree, class FuncT, class ProjT>
    static auto operator()(_Tree&& __range, FuncT __func, ProjT __proj) {
        if (__range.size() != 0)
            mstd::tree_iterate_from_root<__copy_cvref_t<_Tree, typename std::remove_cvref_t<_Tree>::value_type>>(
                [](node_pointer) { return false; }, __range.root(), __func, __proj);
            return std::make_pair(__range.end(), std::move(__func));
    }
};

template <class _Tp, class _Compare, class _Allocator>
Tree<_Tp, _Compare, _Allocator>& Tree<_Tp, _Compare, _Allocator>::operator=(const Tree& other) {
    if (this == std::addressof(other))
        return *this;

    value_comp() = other.value_comp();
    copyAssignAlloc(other);

    if (__size_ != 0) {
        *root_ptr() = static_cast<node_base_pointer>(copyAssignTree(root(), other.root()));
    } else {
        *root_ptr() = static_cast<node_base_pointer>(copy_construct_tree(other.root()));
        if (root()) {
            root()->__parent_ = endNode();
        }
    }
    __begin_node_
        = endNode()->__left_
        ? static_cast<end_node_pointer>(mstd::tree_min(endNode()->__left_))
        : endNode();
    __size_ = other.size();

    return *this;
}

template <class _Tp, class _Compare, class _Allocator>
Tree<_Tp, _Compare, _Allocator>::Tree(const Tree& other)
: __begin_node_(endNode())
, __node_alloc_(node_traits::select_on_container_copy_construction(other.nodeAlloc()))
, __size_(0)
, __value_comp_(other.value_comp()) {
    if (other.size() == 0)
        return;

    *root_ptr()       = static_cast<node_base_pointer>(copy_construct_tree(other.root()));
    root()->__parent_ = endNode();
    __begin_node_       = static_cast<end_node_pointer>(mstd::tree_min(endNode()->__left_));
    __size_             = other.size();
}

template <class _Tp, class _Compare, class _Allocator>
Tree<_Tp, _Compare, _Allocator>::Tree(Tree&& other) noexcept(
    std::is_nothrow_move_constructible<node_allocator>::value
 && std::is_nothrow_move_constructible<value_compare>::value
)
: __begin_node_(std::move(other.__begin_node_))
, __end_node_(std::move(other.__end_node_))
, __node_alloc_(std::move(other.__node_alloc_))
, __size_(other.__size_)
, __value_comp_(std::move(other.__value_comp_)) {
    if (__size_ == 0) {
        __begin_node_ = endNode();
    } else {
        endNode()->__left_->__parent_ = static_cast<end_node_pointer>(endNode());
        other.__begin_node_                = other.endNode();
        other.endNode()->__left_        = nullptr;
        other.__size_                      = 0;
    }
}

template <class _Tp, class _Compare, class _Allocator>
Tree<_Tp, _Compare, _Allocator>::Tree(Tree&& other, const allocator_type& alloc)
: __begin_node_(endNode())
, __node_alloc_(node_allocator(alloc))
, __size_(0)
, __value_comp_(std::move(other.value_comp())) {
    if (other.size() == 0)
        return;
    if (alloc == other.alloc()) {
        __begin_node_                    = other.__begin_node_;
        endNode()->__left_            = other.endNode()->__left_;
        endNode()->__left_->__parent_ = static_cast<end_node_pointer>(endNode());
        __size_                          = other.__size_;
        other.__begin_node_                = other.endNode();
        other.endNode()->__left_        = nullptr;
        other.__size_                      = 0;
    } else {
        *root_ptr()     = static_cast<node_base_pointer>(move_construct_tree(other.root()));
        root()->__parent_ = endNode();
        __begin_node_     = static_cast<end_node_pointer>(mstd::tree_min(endNode()->__left_));
        __size_           = other.size();
        other.clear(); // Ensure that other is in a valid state after moving out the keys
    }
}

template <class _Tp, class _Compare, class _Allocator>
void Tree<_Tp, _Compare, _Allocator>::moveAssign(Tree& other, std::true_type)
noexcept(
    std::is_nothrow_move_assignable<value_compare>::value
 && std::is_nothrow_move_assignable<node_allocator>::value
) {
    destroy_(static_cast<node_pointer>(endNode()->__left_));
    __begin_node_ = other.__begin_node_;
    __end_node_   = other.__end_node_;
    __move_assign_alloc(other);
    __size_       = other.__size_;
    __value_comp_ = std::move(other.__value_comp_);
    if (__size_ == 0) {
        __begin_node_ = endNode();
    } else {
        endNode()->__left_->__parent_ = static_cast<end_node_pointer>(endNode());
        other.__begin_node_              = other.endNode();
        other.endNode()->__left_      = nullptr;
        other.__size_                    = 0;
    }
}

template <class _Tp, class _Compare, class _Allocator>
void Tree<_Tp, _Compare, _Allocator>::moveAssign(Tree& other, std::false_type) {
    if (nodeAlloc() == other.nodeAlloc()) {
        moveAssign(other, std::true_type());
    } else {
        value_comp() = std::move(other.value_comp());
        if (__size_ != 0) {
            *root_ptr() = static_cast<node_base_pointer>(moveAssignTree(root(), other.root()));
        } else {
            *root_ptr() = static_cast<node_base_pointer>(move_construct_tree(other.root()));
            if (root())
                root()->__parent_ = endNode();
        }
        __begin_node_
            = endNode()->__left_
            ? static_cast<end_node_pointer>(mstd::tree_min(endNode()->__left_))
            : endNode();
        __size_ = other.size();
        other.clear(); // Ensure that other is in a valid state after moving out the keys
    }
}

template <class _Tp, class _Compare, class _Allocator>
void Tree<_Tp, _Compare, _Allocator>::swap(Tree& other)
noexcept(std::is_nothrow_swappable_v<value_compare>)
{
    using std::swap;
    swap(__begin_node_, other.__begin_node_);
    swap(__end_node_, other.__end_node_);
    mstd::__swap_allocator(nodeAlloc(), other.nodeAlloc());
    swap(__size_, other.__size_);
    swap(__value_comp_, other.__value_comp_);
    if (__size_ == 0) {
        __begin_node_ = endNode();
    } else {
        endNode()->__left_->__parent_ = endNode();
    } if (other.__size_ == 0) {
        other.__begin_node_ = other.endNode();
    } else {
        other.endNode()->__left_->__parent_ = other.endNode();
    }
}

// Find lower_bound place to insert
// Set parent to parent of null leaf
// Return reference to null leaf
template <class _Tp, class _Compare, class _Allocator>
typename Tree<_Tp, _Compare, _Allocator>::node_base_pointer&
Tree<_Tp, _Compare, _Allocator>::__find_leaf_low(end_node_pointer& parent, const value_type& __v) {
    node_pointer __nd = root();
    if (__nd != nullptr) {
        while (true) {
            if (value_comp()(__nd->get_value(), __v)) {
                if (__nd->__right_ != nullptr) {
                    __nd = static_cast<node_pointer>(__nd->__right_);
                } else {
                    parent = static_cast<end_node_pointer>(__nd);
                    return __nd->__right_;
                }
            } else {
                if (__nd->__left_ != nullptr) {
                    __nd = static_cast<node_pointer>(__nd->__left_);
                } else {
                    parent = static_cast<end_node_pointer>(__nd);
                    return parent->__left_;
                }
            }
        }
    }
    parent = endNode();
    return parent->__left_;
}

// Find leaf place to insert closest to hint
// First check prior to hint.
// Next check after hint.
// Next do O(log N) search.
// Set parent to parent of null leaf
// Return reference to null leaf
template <class _Tp, class _Compare, class _Allocator>
auto Tree<_Tp, _Compare, _Allocator>::__find_leaf(
    const_iterator hint, end_node_pointer& parent, const value_type& __v)
  -> node_base_pointer& {
    if (hint == end() || !value_comp()(*hint, __v)) // check before
    {
        // __v <= *hint
        const_iterator __prior = hint;
        if (__prior == begin() || !value_comp()(__v, *--__prior)) {
            // *prev(hint) <= __v <= *hint
            if (hint.__ptr_->__left_ == nullptr) {
                parent = static_cast<end_node_pointer>(hint.__ptr_);
                return parent->__left_;
            } else {
                parent = static_cast<end_node_pointer>(__prior.__ptr_);
                return static_cast<node_base_pointer>(__prior.__ptr_)->__right_;
            }
        }
        // __v < *prev(hint)
        return findLeafHigh_(parent, __v);
    }
    // else __v > *hint
    return __find_leaf_low(parent, __v);
}

// Find __v
// If __v exists, return the parent of the node of __v and a reference to the pointer to the node of __v.
// If __v doesn't exist, return the parent of the null leaf and a reference to the pointer to the null leaf.
template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::find_equal(const KeyT& key) -> std::pair<end_node_pointer, node_base_pointer&> {
    node_pointer __nd = root();

    if (__nd == nullptr) {
        auto __end = endNode();
        return {
            __end,
            __end->__left_,
        };
    }

    node_base_pointer* __node_ptr = root_ptr();
    auto&& __transparent            = mstd::__as_transparent(value_comp());
    auto comp = LazySynthThreeWayComparator<__make_transparent_t<_Compare>, KeyT, value_type>(__transparent);

    while (true) {
        const auto comp_res = comp(key, __nd->get_value());

        if (comp_res.__less()) {
            if (__nd->__left_ == nullptr) {
                return {
                    static_cast<end_node_pointer>(__nd),
                    __nd->__left_,
                };
            }

            __node_ptr = std::addressof(__nd->__left_);
            __nd       = static_cast<node_pointer>(__nd->__left_);
        } else if (comp_res.__greater()) {
            if (__nd->__right_ == nullptr) {
                return {
                    static_cast<end_node_pointer>(__nd),
                    __nd->__right_,
                };
            }
            __node_ptr = std::addressof(__nd->__right_);
            __nd       = static_cast<node_pointer>(__nd->__right_);
        } else {
            return {
                static_cast<end_node_pointer>(__nd),
                *__node_ptr,
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
template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::find_equal(const_iterator hint, node_base_pointer& dummy, const KeyT& key)
  -> std::pair<end_node_pointer, node_base_pointer&> {
    if (hint == end() || value_comp()(key, *hint)) { // check before
        // key < *hint
        const_iterator __prior = hint;
        if (__prior == begin() || value_comp()(*--__prior, key)) {
            // *prev(hint) < key < *hint
            if (hint.__ptr_->__left_ == nullptr) {
                return {
                    hint.__ptr_,
                    hint.__ptr_->__left_,
                };
            }
            return {
                __prior.__ptr_,
                static_cast<node_pointer>(__prior.__ptr_)->__right_,
            };
        }
        // key <= *prev(hint)
        return find_equal(key);
    }

    if (value_comp()(*hint, key)) { // check after
        // *hint < key
        const_iterator __next = std::next(hint);
        if (__next == end() || value_comp()(key, *__next)) {
            // *hint < key < *std::next(hint)
            if (hint.__get_np()->__right_ == nullptr) {
                return {
                    hint.__ptr_,
                    static_cast<node_pointer>(hint.__ptr_)->__right_,
                };
            }
            return {
                __next.__ptr_,
                __next.__ptr_->__left_,
            };
        }
        // *next(hint) <= key
        return find_equal(key);
    }

    // else key == *hint
    dummy = static_cast<node_base_pointer>(hint.__ptr_);
    return {
        hint.__ptr_,
        dummy,
    };
}

template <class _Tp, class _Compare, class _Allocator>
auto Tree<_Tp, _Compare, _Allocator>::__remove_node_pointer(node_pointer ptr) noexcept -> iterator {
    iterator ret(ptr);
    ++ret;
    if (__begin_node_ == ptr) {
        __begin_node_ = ret.__ptr_;
    }
    --__size_;
    mstd::__tree_remove(endNode()->__left_, static_cast<node_base_pointer>(ptr));
    return ret;
}

template <class _Tp, class _Compare, class _Allocator>
template <class _NodeHandle, class _InsertReturnType>
_InsertReturnType
Tree<_Tp, _Compare, _Allocator>::__node_handle_insert_unique(_NodeHandle&& nh) {
    if (nh.empty()) {
        return _InsertReturnType{end(), false, _NodeHandle()};
    }
    auto ptr = nh.__ptr_;
    auto [parent, child] = find_equal(ptr->get_value());
    if (child != nullptr) {
        return _InsertReturnType{iterator(static_cast<node_pointer>(child)), false, std::move(nh)};
    }
    insertNodeAt(parent, child, static_cast<node_base_pointer>(ptr));
    nh.__release_ptr();
    return _InsertReturnType{iterator(ptr), true, _NodeHandle()};
}

template <class _Tp, class _Compare, class _Allocator>
template <class _NodeHandle>
auto Tree<_Tp, _Compare, _Allocator>::__node_handle_insert_unique(const_iterator hint, _NodeHandle&& nh)
  -> iterator {
    if (nh.empty()) {
        return end();
    }
    auto ptr = nh.__ptr_;
    node_base_pointer dummy;
    auto [parent, child] = find_equal(hint, dummy, ptr->get_value());
    auto ret             = static_cast<node_pointer>(child);
    if (child == nullptr) {
        insertNodeAt(parent, child, static_cast<node_base_pointer>(ptr));
        ret = ptr;
        nh.__release_ptr();
    }
    return iterator(ret);
}

template <class _Tp, class _Compare, class _Allocator>
template <class _NodeHandle>
_NodeHandle Tree<_Tp, _Compare, _Allocator>::__node_handle_extract(key_type const& key) {
    if (const auto iter = find(key); iter != end()) {
        return __node_handle_extract<_NodeHandle>(iter);
    }
    return _NodeHandle();
}

template <class _Tp, class _Compare, class _Allocator>
template <class _NodeHandle>
_NodeHandle Tree<_Tp, _Compare, _Allocator>::__node_handle_extract(const_iterator pos) {
    auto node_ptr = pos.__get_np();
    __remove_node_pointer(node_ptr);
    return _NodeHandle(node_ptr, alloc());
}

template <class _Tp, class _Compare, class _Allocator>
template <class _Comp2>
void Tree<_Tp, _Compare, _Allocator>::__node_handle_merge_unique(Tree<_Tp, _Comp2, _Allocator>& source) {
    for (iterator iter = source.begin(); iter != source.end();) {
        auto src_ptr = iter.__get_np();
        auto [parent, child] = find_equal(src_ptr->get_value());
        ++iter;
        if (child != nullptr) {
            continue;
        }
        source.__remove_node_pointer(src_ptr);
        insertNodeAt(parent, child, static_cast<node_base_pointer>(src_ptr));
    }
}

template <class _Tp, class _Compare, class _Allocator>
template <class _NodeHandle>
auto Tree<_Tp, _Compare, _Allocator>::__node_handle_insert_multi(_NodeHandle&& nh)
  -> iterator {
    if (nh.empty()) {
        return end();
    }
    auto ptr = nh.__ptr_;
    end_node_pointer parent;
    auto& child = findLeafHigh_(parent, ptr->get_value());
    insertNodeAt(parent, child, static_cast<node_base_pointer>(ptr));
    nh.__release_ptr();
    return iterator(ptr);
}

template <class _Tp, class _Compare, class _Allocator>
template <class _NodeHandle>
auto
Tree<_Tp, _Compare, _Allocator>::__node_handle_insert_multi(const_iterator hint, _NodeHandle&& nh)
  -> iterator {
    if (nh.empty()) {
        return end();
    }
    auto ptr = nh.__ptr_;
    end_node_pointer parent;
    auto& child = __find_leaf(hint, parent, ptr->get_value());
    insertNodeAt(parent, child, static_cast<node_base_pointer>(ptr));
    nh.__release_ptr();
    return iterator(ptr);
}

template <class _Tp, class _Compare, class _Allocator>
template <class _Comp2>
void Tree<_Tp, _Compare, _Allocator>::__node_handle_merge_multi(Tree<_Tp, _Comp2, _Allocator>& source) {
    for (iterator iter = source.begin(); iter != source.end();) {
        auto src_ptr = iter.__get_np();
        end_node_pointer parent;
        auto& child = findLeafHigh_(parent, src_ptr->get_value());
        ++iter;
        source.__remove_node_pointer(src_ptr);
        insertNodeAt(parent, child, static_cast<node_base_pointer>(src_ptr));
    }
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__erase_unique(const KeyT& key) -> size_type {
    if (const auto iter = find(key); iter != end()) {
        erase(iter);
        return 1;
    }
    return 0;
}

template <class _Tp, class _Compare, class _Allocator>
template <class KeyT>
auto Tree<_Tp, _Compare, _Allocator>::__erase_multi(const KeyT& key) -> size_type {
    auto [begin, end] = equalRangeMulti(key);
    size_type ret_cnt = 0;
    for (; begin != end; ++ret_cnt) {
        begin = erase(begin);
    }
    return ret_cnt;
}

template <class _Tp, class _Compare, class _Allocator>
typename Tree<_Tp, _Compare, _Allocator>::node_holder
Tree<_Tp, _Compare, _Allocator>::remove(const_iterator pos) noexcept {
    auto node_ptr = pos.__get_np();
    if (__begin_node_ == pos.__ptr_) {
        if (node_ptr->__right_ != nullptr) {
            __begin_node_ = static_cast<end_node_pointer>(node_ptr->__right_);
        } else {
            __begin_node_ = static_cast<end_node_pointer>(node_ptr->__parent_);
        }
    }
    --__size_;
    mstd::__tree_remove(endNode()->__left_, static_cast<node_base_pointer>(node_ptr));
    return node_holder(node_ptr, _Dp(nodeAlloc(), true));
}

template <class _Tp, class _Compare, class _Allocator>
inline void swap(Tree<_Tp, _Compare, _Allocator>& __x, Tree<_Tp, _Compare, _Allocator>& __y)
noexcept(noexcept(__x.swap(__y))) {
    __x.swap(__y);
}

} // namespace mstd


#endif // MSTD___TREE
