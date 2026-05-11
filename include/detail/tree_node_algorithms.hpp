#ifndef MSTD__DETAIL_TREE_NODE_ALGORITHMS_HPP
#define MSTD__DETAIL_TREE_NODE_ALGORITHMS_HPP

#include <functional>

#define MSTD_ASSERT_INTERNAL(stmt, message) assert((stmt) && (message));

namespace mstd::detail {

/*
 *
 * NodePtrT algorithms
 *
 * The algorithms taking NodePtrT are red black tree algorithms.  Those
 * algorithms taking a parameter named root should assume that root
 * points to a proper red black tree (unless otherwise specified).
 *
 * Each algorithm herein assumes that root->parent_ points to a non-null
 * structure which has a member left_ which points back to root.  No other
 * member is read or written to at root->parent_.
 *
 * root->parent_ will be referred to below (in comments only) as endNode.
 * endNode->left_ is an externally accessible lvalue for root, and can be
 * changed by node insertion and removal (without explicit reference to endNode).
 *
 * All nodes (with the exception of endNode), even the node referred to as
 * root, have a non-null parent_ field.
 *
 */

// Returns:  true if x is a left child of its parent, else false
// Precondition:  x != nullptr.
template <class NodePtrT>
inline bool tree_is_left_child(NodePtrT x) noexcept {
    return x == x->parent_->left_;
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
    // check node_ptr->left_ consistency
    if (node_ptr->left_ != nullptr && node_ptr->left_->parent_ != node_ptr) {
        return 0;
    }
    // check node_ptr->right_ consistency
    if (node_ptr->right_ != nullptr && node_ptr->right_->parent_ != node_ptr) {
        return 0;
    }
    // check node_ptr->left_ != node_ptr->right_ unless both are nullptr
    if (node_ptr->left_ == node_ptr->right_ && node_ptr->left_ != nullptr) {
        return 0;
    }
    // If this is red, neither child can be red
    if (!node_ptr->is_black_) {
        if (node_ptr->left_ && !node_ptr->left_->is_black_) {
            return 0;
        }
        if (node_ptr->right_ && !node_ptr->right_->is_black_) {
            return 0;
        }
    }
    unsigned h = mstd::detail::tree_sub_invariant(node_ptr->left_);
    if (h == 0) {
        return 0; // invalid left subtree
    }
    if (h != mstd::detail::tree_sub_invariant(node_ptr->right_)) {
        return 0;                    // invalid or different height right subtree
    }
    return h + node_ptr->is_black_; // return black height of this node
}

// Determines if the red black tree rooted at root is a proper red black tree.
//    root == nullptr is a proper tree.  Returns true if root is a proper
//    red black tree, else returns false.
template <class NodePtrT>
bool tree_invariant(NodePtrT root) {
    if (root == nullptr) {
        return true;
    }
    // check node_ptr->parent_ consistency
    if (root->parent_ == nullptr) {
        return false;
    }
    if (!mstd::detail::tree_is_left_child(root)) {
        return false;
    }
    // root must be black
    if (!root->is_black_) {
        return false;
    }
    // do normal node checks
    return mstd::detail::tree_sub_invariant(root) != 0;
}

// Returns:  pointer to the left-most node under node_ptr.
template <class NodePtrT>
inline NodePtrT tree_min(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "Root node shouldn't be null");
    while (node_ptr->left_ != nullptr) {
        node_ptr = node_ptr->left_;
    }
    return node_ptr;
}

// Returns:  pointer to the right-most node under node_ptr.
template <class NodePtrT>
inline NodePtrT tree_max(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "Root node shouldn't be null");
    while (node_ptr->right_ != nullptr) {
        node_ptr = node_ptr->right_;
    }
    return node_ptr;
}

// Returns:  pointer to the next in-order node after node_ptr.
template <class NodePtrT>
NodePtrT tree_next(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "node shouldn't be null");
    if (node_ptr->right_ != nullptr) {
        return mstd::detail::tree_min(node_ptr->right_);
    }
    while (!mstd::detail::tree_is_left_child(node_ptr)) {
        node_ptr = node_ptr->parent_unsafe();
    }
    return node_ptr->parent_unsafe();
}

// tree_next_iter and tree_prev_iter implement iteration through the tree. The order is as follows:
// left sub-tree -> node -> right sub-tree. When the right-most node of a sub-tree is reached, we walk up the tree until
// we find a node where we were in the left sub-tree. We are _always_ in a left sub-tree, since the end_node_ points
// to the actual root of the tree through a left_ pointer. Incrementing the end() pointer is UB, so we can assume that
// never happens.
template <class EndNodePtrT, class NodePtrT>
inline EndNodePtrT tree_next_iter(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "node shouldn't be null");
    if (node_ptr->right_ != nullptr) {
        return static_cast<EndNodePtrT>(mstd::detail::tree_min(node_ptr->right_));
    }
    while (!mstd::detail::tree_is_left_child(node_ptr)) {
        node_ptr = node_ptr->parent_unsafe();
    }
    return static_cast<EndNodePtrT>(node_ptr->parent_);
}

// Returns:  pointer to the previous in-order node before x.
// Note: node_ptr may be the end node.
template <class NodePtrT, class EndNodePtrT>
inline NodePtrT tree_prev_iter(EndNodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "node shouldn't be null");
    if (node_ptr->left_ != nullptr) {
        return mstd::detail::tree_max(node_ptr->left_);
    }
    auto curr_node = static_cast<NodePtrT>(node_ptr);
    while (mstd::detail::tree_is_left_child(curr_node)) {
        curr_node = curr_node->parent_unsafe();
    }
    return curr_node->parent_unsafe();
}

// Effects:  Makes node_ptr->right_ the subtree root with node_ptr as its left child
//           while preserving in-order order.
template <class NodePtrT>
void tree_left_rotate(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "node shouldn't be null");
    MSTD_ASSERT_INTERNAL(node_ptr->right_ != nullptr, "node should have a right child");
    NodePtrT y  = node_ptr->right_;
    node_ptr->right_ = y->left_;
    if (node_ptr->right_ != nullptr) {
        node_ptr->right_->set_parent(node_ptr);
    }
    y->parent_ = node_ptr->parent_;
    if (mstd::detail::tree_is_left_child(node_ptr)) {
        node_ptr->parent_->left_ = y;
    } else {
        node_ptr->parent_unsafe()->right_ = y;
    }
    y->left_ = node_ptr;
    node_ptr->set_parent(y);
}

// Effects:  Makes node_ptr->left_ the subtree root with node_ptr as its right child
//           while preserving in-order order.
template <class NodePtrT>
void tree_right_rotate(NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "node shouldn't be null");
    MSTD_ASSERT_INTERNAL(node_ptr->left_ != nullptr, "node should have a left child");
    NodePtrT y = node_ptr->left_;
    node_ptr->left_ = y->right_;
    if (node_ptr->left_ != nullptr) {
        node_ptr->left_->set_parent(node_ptr);
    }
    y->parent_ = node_ptr->parent_;
    if (mstd::detail::tree_is_left_child(node_ptr)) {
        node_ptr->parent_->left_ = y;
    } else {
        node_ptr->parent_unsafe()->right_ = y;
    }
    y->right_ = node_ptr;
    node_ptr->set_parent(y);
}

// Effects:  Rebalances root after attaching node_ptr to a leaf.
// Precondition:  node_ptr has no children.
//                node_ptr == root or == a direct or indirect child of root.
//                If node_ptr were to be unlinked from root (setting root to
//                  nullptr if root == node_ptr), tree_invariant(root) == true.
// Postcondition: tree_invariant(endNode->left_) == true.  endNode->left_
//                may be different than the value passed in as root.
template <class NodePtrT>
void tree_balance_after_insert(NodePtrT root, NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(root != nullptr, "Root of the tree shouldn't be null");
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "Can't attach null node to a leaf");
    node_ptr->is_black_ = node_ptr == root;
    while (node_ptr != root && !node_ptr->parent_unsafe()->is_black_) {
        // node_ptr->parent_ != root because node_ptr->parent_->__is_black == false
        if (mstd::detail::tree_is_left_child(node_ptr->parent_unsafe())) {
            NodePtrT y = node_ptr->parent_unsafe()->parent_unsafe()->right_;
            if (y != nullptr && !y->is_black_) {
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->is_black_ = true;
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->is_black_ = node_ptr == root;
                y->is_black_ = true;
            } else {
                if (!mstd::detail::tree_is_left_child(node_ptr)) {
                    node_ptr = node_ptr->parent_unsafe();
                    mstd::detail::tree_left_rotate(node_ptr);
                }
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->is_black_ = true;
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->is_black_ = false;
                mstd::detail::tree_right_rotate(node_ptr);
                break;
            }
        } else {
            NodePtrT y = node_ptr->parent_unsafe()->parent_->left_;
            if (y != nullptr && !y->is_black_) {
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->is_black_ = true;
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->is_black_ = node_ptr == root;
                y->is_black_ = true;
            } else {
                if (mstd::detail::tree_is_left_child(node_ptr)) {
                    node_ptr = node_ptr->parent_unsafe();
                    mstd::detail::tree_right_rotate(node_ptr);
                }
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->is_black_ = true;
                node_ptr              = node_ptr->parent_unsafe();
                node_ptr->is_black_ = false;
                mstd::detail::tree_left_rotate(node_ptr);
                break;
            }
        }
    }
}

// Precondition:  node_ptr == root or == a direct or indirect child of root.
// Effects:  unlinks node_ptr from the tree rooted at root, rebalancing as needed.
// Postcondition: tree_invariant(endNode->left_) == true && endNode->left_
//                nor any of its children refer to node_ptr.  endNode->left_
//                may be different than the value passed in as root.
template <class NodePtrT>
void tree_remove(NodePtrT root, NodePtrT node_ptr) noexcept {
    MSTD_ASSERT_INTERNAL(root != nullptr, "Root node should not be null");
    MSTD_ASSERT_INTERNAL(node_ptr != nullptr, "The node to remove should not be null");
    MSTD_ASSERT_INTERNAL(mstd::detail::tree_invariant(root), "The tree invariants should hold");
    // node_ptr will be removed from the tree.  Client still needs to destruct/deallocate it
    // y is either node_ptr, or if node_ptr has two children, tree_next(node_ptr).
    // y will have at most one child.
    // y will be the initial hole in the tree (make the hole at a leaf)
    NodePtrT y
        = (node_ptr->left_ == nullptr || node_ptr->right_ == nullptr)
          ? node_ptr
          : mstd::detail::tree_next(node_ptr);
    // x is y's possibly null single child
    NodePtrT x
        = (y->left_ != nullptr)
          ? y->left_
          : y->right_;
    // w is x's possibly null uncle (will become x's sibling)
    NodePtrT w = nullptr;
    // link x to y's parent, and find w
    if (x != nullptr) {
        x->parent_ = y->parent_;
    }
    if (mstd::detail::tree_is_left_child(y)) {
        y->parent_->left_ = x;
        if (y != root) {
            w = y->parent_unsafe()->right_;
        } else {
            root = x; // w == nullptr
        }
    } else {
        y->parent_unsafe()->right_ = x;
        // y can't be root if it is a right child
        w = y->parent_->left_;
    }
    const bool removed_black = y->is_black_;
    // If we didn't remove node_ptr, do so now by splicing in y for node_ptr,
    //    but copy node_ptr's color.  This does not impact x or w.
    if (y != node_ptr) {
        // node_ptr->left_ != nullptr but node_ptr->right_ might == x == nullptr
        y->parent_ = node_ptr->parent_;
        if (mstd::detail::tree_is_left_child(node_ptr)) {
            y->parent_->left_ = y;
        } else {
            y->parent_unsafe()->right_ = y;
        }
        y->left_ = node_ptr->left_;
        y->left_->set_parent(y);
        y->right_ = node_ptr->right_;
        if (y->right_ != nullptr) {
            y->right_->set_parent(y);
        }
        y->is_black_ = node_ptr->is_black_;
        if (root == node_ptr) {
            root = y;
        }
    }
    // There is no need to rebalance if we removed a red, or if we removed
    //     the last node.
    if (removed_black && root != nullptr) {
        // Rebalance:
        // x has an implicit black color (transferred from the removed y)
        //    associated with it, no matter what its color is.
        // If x is root (in which case it can't be null), it is supposed
        //    to be black anyway, and if it is doubly black, then the double
        //    can just be ignored.
        // If x is red (in which case it can't be null), then it can absorb
        //    the implicit black just by setting its color to black.
        // Since y was black and only had one child (which x points to), x
        //   is either red with no children, else null, otherwise y would have
        //   different black heights under left and right pointers.
        // if (x == root || x != nullptr && !x->is_black_)
        if (x != nullptr) {
            x->is_black_ = true;
        } else {
            //  Else x isn't root, and is "doubly black", even though it may
            //     be null.  w can not be null here, else the parent would
            //     see a black height >= 2 on the x side and a black height
            //     of 1 on the w side (w must be a non-null black or a red
            //     with a non-null black child).
            while (true) {
                if (!mstd::detail::tree_is_left_child(w)) { // if node_ptr is left child
                    if (!w->is_black_) {
                        w->is_black_                    = true;
                        w->parent_unsafe()->is_black_ = false;
                        mstd::detail::tree_left_rotate(w->parent_unsafe());
                        // x is still valid
                        // reset root only if necessary
                        if (root == w->left_) {
                            root = w;
                        }
                        // reset sibling, and it still can't be null
                        w = w->left_->right_;
                    }
                    // w->is_black_ is now true, w may have null children
                    if ((w->left_ == nullptr || w->left_->is_black_)
                            && (w->right_ == nullptr || w->right_->is_black_)) {
                        w->is_black_ = false;
                        x              = w->parent_unsafe();
                        // x can no longer be null
                        if (x == root || !x->is_black_) {
                            x->is_black_ = true;
                            break;
                        }
                        // reset sibling, and it still can't be null
                        w = mstd::detail::tree_is_left_child(x)
                            ? x->parent_unsafe()->right_
                            : x->parent_->left_;
                        // continue;
                    } else { // w has a red child
                        if (w->right_ == nullptr || w->right_->is_black_) {
                            // w left child is non-null and red
                            w->left_->is_black_ = true;
                            w->is_black_          = false;
                            mstd::detail::tree_right_rotate(w);
                            // w is known not to be root, so root hasn't changed
                            // reset sibling, and it still can't be null
                            w = w->parent_unsafe();
                        }
                        // w has a right red child, left child may be null
                        w->is_black_                    = w->parent_unsafe()->is_black_;
                        w->parent_unsafe()->is_black_ = true;
                        w->right_->is_black_          = true;
                        mstd::detail::tree_left_rotate(w->parent_unsafe());
                        break;
                    }
                } else {
                    if (!w->is_black_) {
                        w->is_black_                  = true;
                        w->parent_unsafe()->is_black_ = false;
                        mstd::detail::tree_right_rotate(w->parent_unsafe());
                        // x is still valid
                        // reset root only if necessary
                        if (root == w->right_) {
                            root = w;
                        }
                        // reset sibling, and it still can't be null
                        w = w->right_->left_;
                    }
                    // w->is_black_ is now true, w may have null children
                    if ((w->left_ == nullptr || w->left_->is_black_)
                            && (w->right_ == nullptr || w->right_->is_black_)) {
                        w->is_black_ = false;
                        x              = w->parent_unsafe();
                        // x can no longer be null
                        if (!x->is_black_ || x == root) {
                            x->is_black_ = true;
                            break;
                        }
                        // reset sibling, and it still can't be null
                        w = mstd::detail::tree_is_left_child(x)
                            ? x->parent_unsafe()->right_
                            : x->parent_->left_;
                        // continue;
                    } else { // w has a red child
                        if (w->left_ == nullptr || w->left_->is_black_) {
                            // w right child is non-null and red
                            w->right_->is_black_ = true;
                            w->is_black_           = false;
                            mstd::detail::tree_left_rotate(w);
                            // w is known not to be root, so root hasn't changed
                            // reset sibling, and it still can't be null
                            w = w->parent_unsafe();
                        }
                        // w has a left red child, right child may be null
                        w->is_black_                  = w->parent_unsafe()->is_black_;
                        w->parent_unsafe()->is_black_ = true;
                        w->left_->is_black_         = true;
                        mstd::detail::tree_right_rotate(w->parent_unsafe());
                        break;
                    }
                }
            }
        }
    }
}

// Do an in-order traversal of the tree until `brk` returns true. Takes the root node of the tree.
template <class ReferenceT, class BreakT, class NodePtrT, class FuncT, class ProjT>
bool tree_iterate_from_root(BreakT brk, NodePtrT root, FuncT& func, ProjT& proj) {
    if (root->left_) {
        if (mstd::detail::tree_iterate_from_root<ReferenceT>(brk, static_cast<NodePtrT>(root->left_), func, proj)) {
            return true;
        }
    }
    if (brk(root)) {
        return true;
    }
    std::invoke(func, std::invoke(proj, static_cast<ReferenceT>(root->get_value())));
    if (root->right_) {
        return mstd::detail::tree_iterate_from_root<ReferenceT>(brk, static_cast<NodePtrT>(root->right_), func, proj);
    }
    return false;
}

// Do an in-order traversal of the tree from begin to end.
template <class NodeIterT, class FuncT, class ProjT>
void tree_iterate_subrange(NodeIterT begin, NodeIterT end, FuncT& func, ProjT& proj) {
    using NodePtrT  = NodeIterT::node_pointer;
    using Reference = NodeIterT::reference;

    auto begin_node = begin.ptr_;
    auto end_node   = end.ptr_;

    while (true) {
        if (begin_node == end_node) {
            return;
        }
        const auto nfirst = static_cast<NodePtrT>(begin_node);
        std::invoke(func, std::invoke(proj, static_cast<Reference>(nfirst->get_value())));
        if (nfirst->right_) {
            if (mstd::detail::tree_iterate_from_root<Reference>(
                    [&](NodePtrT node) -> bool { return node == end_node; },
                    static_cast<NodePtrT>(nfirst->right_),
                    func,
                    proj)) {
                return;
            }
        }
        while (!mstd::detail::tree_is_left_child(static_cast<NodePtrT>(begin_node))) {
            begin_node = static_cast<NodePtrT>(begin_node)->parent_;
        }
        begin_node = static_cast<NodePtrT>(begin_node)->parent_;
    }
}

}

#endif
