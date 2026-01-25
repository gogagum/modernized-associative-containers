#include <gtest/gtest.h>

#include <detail/tree.hpp>

struct Node {
  Node* __left_;
  Node* __right_;
  Node* __parent_;
  bool __is_black_;

  Node* parent_unsafe() const { return __parent_; }
  void set_parent(Node* x) { __parent_ = x; }

  Node() : __left_(), __right_(), __parent_(), __is_black_() {}
};

TEST(TreeRotate, Right1) {
  Node root;
  Node x;
  Node y;
  root.__left_ = &x;
  x.__left_    = &y;
  x.__right_   = 0;
  x.__parent_  = &root;
  y.__left_    = 0;
  y.__right_   = 0;
  y.__parent_  = &x;
  mstd::tree_right_rotate(&x);
  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &y);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(y.__parent_, &root);
  EXPECT_EQ(y.__left_, nullptr);
  EXPECT_EQ(y.__right_, &x);
  EXPECT_EQ(x.__parent_, &y);
  EXPECT_EQ(x.__left_, nullptr);
  EXPECT_EQ(x.__right_, nullptr);
}

TEST(TreeRotate, Right2) {
  Node root;
  Node x;
  Node y;
  Node a;
  Node b;
  Node c;
  root.__left_ = &x;
  x.__left_    = &y;
  x.__right_   = &c;
  x.__parent_  = &root;
  y.__left_    = &a;
  y.__right_   = &b;
  y.__parent_  = &x;
  a.__parent_  = &y;
  b.__parent_  = &y;
  c.__parent_  = &x;
  mstd::tree_right_rotate(&x);
  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &y);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(y.__parent_, &root);
  EXPECT_EQ(y.__left_, &a);
  EXPECT_EQ(y.__right_, &x);
  EXPECT_EQ(x.__parent_, &y);
  EXPECT_EQ(x.__left_, &b);
  EXPECT_EQ(x.__right_, &c);
  EXPECT_EQ(a.__parent_, &y);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(b.__parent_, &x);
  EXPECT_EQ(b.__left_, nullptr);
  EXPECT_EQ(b.__right_, nullptr);
  EXPECT_EQ(c.__parent_, &x);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
}

TEST(TreeRotate, Left1) {
  Node root;
  Node x;
  Node y;
  root.__left_ = &x;
  x.__left_    = 0;
  x.__right_   = &y;
  x.__parent_  = &root;
  y.__left_    = 0;
  y.__right_   = 0;
  y.__parent_  = &x;
  mstd::tree_left_rotate(&x);
  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &y);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(y.__parent_, &root);
  EXPECT_EQ(y.__left_, &x);
  EXPECT_EQ(y.__right_, nullptr);
  EXPECT_EQ(x.__parent_, &y);
  EXPECT_EQ(x.__left_, nullptr);
  EXPECT_EQ(x.__right_, nullptr);
}

TEST(TreeRotate, Left2) {
  Node root;
  Node x;
  Node y;
  Node a;
  Node b;
  Node c;
  root.__left_ = &x;
  x.__left_    = &a;
  x.__right_   = &y;
  x.__parent_  = &root;
  y.__left_    = &b;
  y.__right_   = &c;
  y.__parent_  = &x;
  a.__parent_  = &x;
  b.__parent_  = &y;
  c.__parent_  = &y;
  mstd::tree_left_rotate(&x);
  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &y);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(y.__parent_, &root);
  EXPECT_EQ(y.__left_, &x);
  EXPECT_EQ(y.__right_, &c);
  EXPECT_EQ(x.__parent_, &y);
  EXPECT_EQ(x.__left_, &a);
  EXPECT_EQ(x.__right_, &b);
  EXPECT_EQ(a.__parent_, &x);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(b.__parent_, &x);
  EXPECT_EQ(b.__left_, nullptr);
  EXPECT_EQ(b.__right_, nullptr);
  EXPECT_EQ(c.__parent_, &y);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
}

TEST(TreeRemove, Test1) {
  {
    // Left
    // Case 1 -> Case 2 -> x is red turned to black
    Node root;
    Node b;
    Node c;
    Node d;
    Node e;
    Node y;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &y;
    b.__right_    = &d;
    b.__is_black_ = true;

    y.__parent_   = &b;
    y.__left_     = 0;
    y.__right_    = 0;
    y.__is_black_ = true;

    d.__parent_   = &b;
    d.__left_     = &c;
    d.__right_    = &e;
    d.__is_black_ = false;

    c.__parent_   = &d;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = true;

    e.__parent_   = &d;
    e.__left_     = 0;
    e.__right_    = 0;
    e.__is_black_ = true;

    mstd::__tree_remove(root.__left_, &y);
    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &d);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(d.__parent_, &root);
    EXPECT_EQ(d.__left_, &b);
    EXPECT_EQ(d.__right_, &e);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(b.__parent_, &d);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, false);

    EXPECT_EQ(e.__parent_, &d);
    EXPECT_EQ(e.__left_, nullptr);
    EXPECT_EQ(e.__right_, nullptr);
    EXPECT_EQ(e.__is_black_, true);
  }
  {
    // Right
    // Case 1 -> Case 2 -> x is red turned to black
    Node root;
    Node b;
    Node c;
    Node d;
    Node e;
    Node y;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__right_    = &y;
    b.__left_     = &d;
    b.__is_black_ = true;

    y.__parent_   = &b;
    y.__right_    = 0;
    y.__left_     = 0;
    y.__is_black_ = true;

    d.__parent_   = &b;
    d.__right_    = &c;
    d.__left_     = &e;
    d.__is_black_ = false;

    c.__parent_   = &d;
    c.__right_    = 0;
    c.__left_     = 0;
    c.__is_black_ = true;

    e.__parent_   = &d;
    e.__right_    = 0;
    e.__left_     = 0;
    e.__is_black_ = true;

    mstd::__tree_remove(root.__left_, &y);
    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &d);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(d.__parent_, &root);
    EXPECT_EQ(d.__right_, &b);
    EXPECT_EQ(d.__left_, &e);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(b.__parent_, &d);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__left_, &c);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__is_black_, false);

    EXPECT_EQ(e.__parent_, &d);
    EXPECT_EQ(e.__right_, nullptr);
    EXPECT_EQ(e.__left_, nullptr);
    EXPECT_EQ(e.__is_black_, true);
  }
  {
    // Left
    // Case 1 -> Case 3 -> Case 4
    Node root;
    Node b;
    Node c;
    Node d;
    Node e;
    Node f;
    Node y;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &y;
    b.__right_    = &d;
    b.__is_black_ = true;

    y.__parent_   = &b;
    y.__left_     = 0;
    y.__right_    = 0;
    y.__is_black_ = true;

    d.__parent_   = &b;
    d.__left_     = &c;
    d.__right_    = &e;
    d.__is_black_ = false;

    c.__parent_   = &d;
    c.__left_     = &f;
    c.__right_    = 0;
    c.__is_black_ = true;

    e.__parent_   = &d;
    e.__left_     = 0;
    e.__right_    = 0;
    e.__is_black_ = true;

    f.__parent_   = &c;
    f.__left_     = 0;
    f.__right_    = 0;
    f.__is_black_ = false;

    mstd::__tree_remove(root.__left_, &y);
    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &d);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(d.__parent_, &root);
    EXPECT_EQ(d.__left_, &f);
    EXPECT_EQ(d.__right_, &e);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(f.__parent_, &d);
    EXPECT_EQ(f.__left_, &b);
    EXPECT_EQ(f.__right_, &c);
    EXPECT_EQ(f.__is_black_, false);

    EXPECT_EQ(b.__parent_, &f);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(c.__parent_, &f);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, true);

    EXPECT_EQ(e.__parent_, &d);
    EXPECT_EQ(e.__left_, nullptr);
    EXPECT_EQ(e.__right_, nullptr);
    EXPECT_EQ(e.__is_black_, true);
  }
  {
    // Right
    // Case 1 -> Case 3 -> Case 4
    Node root;
    Node b;
    Node c;
    Node d;
    Node e;
    Node f;
    Node y;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__right_    = &y;
    b.__left_     = &d;
    b.__is_black_ = true;

    y.__parent_   = &b;
    y.__right_    = 0;
    y.__left_     = 0;
    y.__is_black_ = true;

    d.__parent_   = &b;
    d.__right_    = &c;
    d.__left_     = &e;
    d.__is_black_ = false;

    c.__parent_   = &d;
    c.__right_    = &f;
    c.__left_     = 0;
    c.__is_black_ = true;

    e.__parent_   = &d;
    e.__right_    = 0;
    e.__left_     = 0;
    e.__is_black_ = true;

    f.__parent_   = &c;
    f.__right_    = 0;
    f.__left_     = 0;
    f.__is_black_ = false;

    mstd::__tree_remove(root.__left_, &y);
    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &d);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(d.__parent_, &root);
    EXPECT_EQ(d.__right_, &f);
    EXPECT_EQ(d.__left_, &e);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(f.__parent_, &d);
    EXPECT_EQ(f.__right_, &b);
    EXPECT_EQ(f.__left_, &c);
    EXPECT_EQ(f.__is_black_, false);

    EXPECT_EQ(b.__parent_, &f);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(c.__parent_, &f);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__is_black_, true);

    EXPECT_EQ(e.__parent_, &d);
    EXPECT_EQ(e.__right_, nullptr);
    EXPECT_EQ(e.__left_, nullptr);
    EXPECT_EQ(e.__is_black_, true);
  }
}

TEST(TreeRemove, Test2) {
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = true;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = true;

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, false);

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &c);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, true);

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = false;

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, false);

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &c);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, true);

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = true;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = true;

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, false);

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = false;

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, false);

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = true;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = true;

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &c);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &c);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &a);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, true);

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &c);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, true);

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = false;

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &c);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &c);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &a);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, true);

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &c);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, true);

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = true;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = true;

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &c);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &c);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &a);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, true);

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &a);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &root);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, true);

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = false;

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &c);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &c);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &a);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, true);

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &a);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &root);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, true);

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = true;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = true;

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &a);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &root);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, true);

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = false;

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &a);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &root);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, true);

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = true;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = true;

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &b;

    b.__parent_   = &root;
    b.__left_     = &a;
    b.__right_    = &c;
    b.__is_black_ = true;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = false;

    mstd::__tree_remove(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    mstd::__tree_remove(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, &b);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    mstd::__tree_remove(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__parent_, nullptr);
    EXPECT_EQ(root.__left_, nullptr);
    EXPECT_EQ(root.__right_, nullptr);
    EXPECT_EQ(root.__is_black_, false);
  }
}

TEST(TreeRemove, Test3) {
  Node root;
  Node a;
  Node b;
  Node c;
  Node d;
  Node e;
  Node f;
  Node g;
  Node h;

  root.__left_ = &e;

  e.__parent_   = &root;
  e.__left_     = &c;
  e.__right_    = &g;
  e.__is_black_ = true;

  c.__parent_   = &e;
  c.__left_     = &b;
  c.__right_    = &d;
  c.__is_black_ = false;

  g.__parent_   = &e;
  g.__left_     = &f;
  g.__right_    = &h;
  g.__is_black_ = false;

  b.__parent_   = &c;
  b.__left_     = &a;
  b.__right_    = 0;
  b.__is_black_ = true;

  d.__parent_   = &c;
  d.__left_     = 0;
  d.__right_    = 0;
  d.__is_black_ = true;

  f.__parent_   = &g;
  f.__left_     = 0;
  f.__right_    = 0;
  f.__is_black_ = true;

  h.__parent_   = &g;
  h.__left_     = 0;
  h.__right_    = 0;
  h.__is_black_ = true;

  a.__parent_   = &b;
  a.__left_     = 0;
  a.__right_    = 0;
  a.__is_black_ = false;

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  mstd::__tree_remove(root.__left_, &h);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &e);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(e.__parent_, &root);
  EXPECT_EQ(e.__left_, &c);
  EXPECT_EQ(e.__right_, &g);
  EXPECT_EQ(e.__is_black_, true);

  EXPECT_EQ(c.__parent_, &e);
  EXPECT_EQ(c.__left_, &b);
  EXPECT_EQ(c.__right_, &d);
  EXPECT_EQ(c.__is_black_, false);

  EXPECT_EQ(g.__parent_, &e);
  EXPECT_EQ(g.__left_, &f);
  EXPECT_EQ(g.__right_, nullptr);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(b.__parent_, &c);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, nullptr);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, false);

  EXPECT_EQ(d.__parent_, &c);
  EXPECT_EQ(d.__left_, nullptr);
  EXPECT_EQ(d.__right_, nullptr);
  EXPECT_EQ(d.__is_black_, true);

  EXPECT_EQ(f.__parent_, &g);
  EXPECT_EQ(f.__left_, nullptr);
  EXPECT_EQ(f.__right_, nullptr);
  EXPECT_EQ(f.__is_black_, false);

  mstd::__tree_remove(root.__left_, &g);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &e);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(e.__parent_, &root);
  EXPECT_EQ(e.__left_, &c);
  EXPECT_EQ(e.__right_, &f);
  EXPECT_EQ(e.__is_black_, true);

  EXPECT_EQ(c.__parent_, &e);
  EXPECT_EQ(c.__left_, &b);
  EXPECT_EQ(c.__right_, &d);
  EXPECT_EQ(c.__is_black_, false);

  EXPECT_EQ(b.__parent_, &c);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, nullptr);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, false);

  EXPECT_EQ(d.__parent_, &c);
  EXPECT_EQ(d.__left_, nullptr);
  EXPECT_EQ(d.__right_, nullptr);
  EXPECT_EQ(d.__is_black_, true);

  EXPECT_EQ(f.__parent_, &e);
  EXPECT_EQ(f.__left_, nullptr);
  EXPECT_EQ(f.__right_, nullptr);
  EXPECT_EQ(f.__is_black_, true);

  mstd::__tree_remove(root.__left_, &f);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &c);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(c.__parent_, &root);
  EXPECT_EQ(c.__left_, &b);
  EXPECT_EQ(c.__right_, &e);
  EXPECT_EQ(c.__is_black_, true);

  EXPECT_EQ(b.__parent_, &c);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, nullptr);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(e.__parent_, &c);
  EXPECT_EQ(e.__left_, &d);
  EXPECT_EQ(e.__right_, nullptr);
  EXPECT_EQ(e.__is_black_, true);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, false);

  EXPECT_EQ(d.__parent_, &e);
  EXPECT_EQ(d.__left_, nullptr);
  EXPECT_EQ(d.__right_, nullptr);
  EXPECT_EQ(d.__is_black_, false);

  mstd::__tree_remove(root.__left_, &e);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &c);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(c.__parent_, &root);
  EXPECT_EQ(c.__left_, &b);
  EXPECT_EQ(c.__right_, &d);
  EXPECT_EQ(c.__is_black_, true);

  EXPECT_EQ(b.__parent_, &c);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, nullptr);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, false);

  EXPECT_EQ(d.__parent_, &c);
  EXPECT_EQ(d.__left_, nullptr);
  EXPECT_EQ(d.__right_, nullptr);
  EXPECT_EQ(d.__is_black_, true);

  mstd::__tree_remove(root.__left_, &d);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &b);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(b.__parent_, &root);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, &c);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, true);

  EXPECT_EQ(c.__parent_, &b);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
  EXPECT_EQ(c.__is_black_, true);

  mstd::__tree_remove(root.__left_, &c);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &b);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(b.__parent_, &root);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, nullptr);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, false);

  mstd::__tree_remove(root.__left_, &b);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &a);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(a.__parent_, &root);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, true);

  mstd::__tree_remove(root.__left_, &a);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, nullptr);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);
}

TEST(TreeRemove, Test4) {
  Node root;
  Node a;
  Node b;
  Node c;
  Node d;
  Node e;
  Node f;
  Node g;
  Node h;

  root.__left_ = &d;

  d.__parent_   = &root;
  d.__left_     = &b;
  d.__right_    = &f;
  d.__is_black_ = true;

  b.__parent_   = &d;
  b.__left_     = &a;
  b.__right_    = &c;
  b.__is_black_ = false;

  f.__parent_   = &d;
  f.__left_     = &e;
  f.__right_    = &g;
  f.__is_black_ = false;

  a.__parent_   = &b;
  a.__left_     = 0;
  a.__right_    = 0;
  a.__is_black_ = true;

  c.__parent_   = &b;
  c.__left_     = 0;
  c.__right_    = 0;
  c.__is_black_ = true;

  e.__parent_   = &f;
  e.__left_     = 0;
  e.__right_    = 0;
  e.__is_black_ = true;

  g.__parent_   = &f;
  g.__left_     = 0;
  g.__right_    = &h;
  g.__is_black_ = true;

  h.__parent_   = &g;
  h.__left_     = 0;
  h.__right_    = 0;
  h.__is_black_ = false;

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  mstd::__tree_remove(root.__left_, &a);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &d);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(d.__parent_, &root);
  EXPECT_EQ(d.__left_, &b);
  EXPECT_EQ(d.__right_, &f);
  EXPECT_EQ(d.__is_black_, true);

  EXPECT_EQ(b.__parent_, &d);
  EXPECT_EQ(b.__left_, nullptr);
  EXPECT_EQ(b.__right_, &c);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(f.__parent_, &d);
  EXPECT_EQ(f.__left_, &e);
  EXPECT_EQ(f.__right_, &g);
  EXPECT_EQ(f.__is_black_, false);

  EXPECT_EQ(c.__parent_, &b);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
  EXPECT_EQ(c.__is_black_, false);

  EXPECT_EQ(e.__parent_, &f);
  EXPECT_EQ(e.__left_, nullptr);
  EXPECT_EQ(e.__right_, nullptr);
  EXPECT_EQ(e.__is_black_, true);

  EXPECT_EQ(g.__parent_, &f);
  EXPECT_EQ(g.__left_, nullptr);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, false);

  mstd::__tree_remove(root.__left_, &b);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &d);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(d.__parent_, &root);
  EXPECT_EQ(d.__left_, &c);
  EXPECT_EQ(d.__right_, &f);
  EXPECT_EQ(d.__is_black_, true);

  EXPECT_EQ(c.__parent_, &d);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
  EXPECT_EQ(c.__is_black_, true);

  EXPECT_EQ(f.__parent_, &d);
  EXPECT_EQ(f.__left_, &e);
  EXPECT_EQ(f.__right_, &g);
  EXPECT_EQ(f.__is_black_, false);

  EXPECT_EQ(e.__parent_, &f);
  EXPECT_EQ(e.__left_, nullptr);
  EXPECT_EQ(e.__right_, nullptr);
  EXPECT_EQ(e.__is_black_, true);

  EXPECT_EQ(g.__parent_, &f);
  EXPECT_EQ(g.__left_, nullptr);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, false);

  mstd::__tree_remove(root.__left_, &c);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &f);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(f.__parent_, &root);
  EXPECT_EQ(f.__left_, &d);
  EXPECT_EQ(f.__right_, &g);
  EXPECT_EQ(f.__is_black_, true);

  EXPECT_EQ(d.__parent_, &f);
  EXPECT_EQ(d.__left_, nullptr);
  EXPECT_EQ(d.__right_, &e);
  EXPECT_EQ(d.__is_black_, true);

  EXPECT_EQ(g.__parent_, &f);
  EXPECT_EQ(g.__left_, nullptr);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(e.__parent_, &d);
  EXPECT_EQ(e.__left_, nullptr);
  EXPECT_EQ(e.__right_, nullptr);
  EXPECT_EQ(e.__is_black_, false);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, false);

  mstd::__tree_remove(root.__left_, &d);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &f);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(f.__parent_, &root);
  EXPECT_EQ(f.__left_, &e);
  EXPECT_EQ(f.__right_, &g);
  EXPECT_EQ(f.__is_black_, true);

  EXPECT_EQ(e.__parent_, &f);
  EXPECT_EQ(e.__left_, nullptr);
  EXPECT_EQ(e.__right_, nullptr);
  EXPECT_EQ(e.__is_black_, true);

  EXPECT_EQ(g.__parent_, &f);
  EXPECT_EQ(g.__left_, nullptr);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, false);

  mstd::__tree_remove(root.__left_, &e);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &g);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(g.__parent_, &root);
  EXPECT_EQ(g.__left_, &f);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(f.__parent_, &g);
  EXPECT_EQ(f.__left_, nullptr);
  EXPECT_EQ(f.__right_, nullptr);
  EXPECT_EQ(f.__is_black_, true);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, true);

  mstd::__tree_remove(root.__left_, &f);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &g);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(g.__parent_, &root);
  EXPECT_EQ(g.__left_, nullptr);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, false);

  mstd::__tree_remove(root.__left_, &g);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &h);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(h.__parent_, &root);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, true);

  mstd::__tree_remove(root.__left_, &h);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, nullptr);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);
}

TEST(TreeBalanceAfterInsert, Test1) {
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &b;
    c.__right_    = &d;
    c.__is_black_ = true;

    b.__parent_   = &c;
    b.__left_     = &a;
    b.__right_    = 0;
    b.__is_black_ = false;

    d.__parent_   = &c;
    d.__left_     = 0;
    d.__right_    = 0;
    d.__is_black_ = false;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    mstd::tree_balance_after_insert(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &c);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &b);
    EXPECT_EQ(c.__right_, &d);
    EXPECT_EQ(c.__is_black_, true);

    EXPECT_EQ(b.__parent_, &c);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &c);
    EXPECT_EQ(d.__left_, nullptr);
    EXPECT_EQ(d.__right_, nullptr);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &b;
    c.__right_    = &d;
    c.__is_black_ = true;

    b.__parent_   = &c;
    b.__left_     = 0;
    b.__right_    = &a;
    b.__is_black_ = false;

    d.__parent_   = &c;
    d.__left_     = 0;
    d.__right_    = 0;
    d.__is_black_ = false;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    mstd::tree_balance_after_insert(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &c);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &b);
    EXPECT_EQ(c.__right_, &d);
    EXPECT_EQ(c.__is_black_, true);

    EXPECT_EQ(b.__parent_, &c);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, &a);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &c);
    EXPECT_EQ(d.__left_, nullptr);
    EXPECT_EQ(d.__right_, nullptr);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &b;
    c.__right_    = &d;
    c.__is_black_ = true;

    b.__parent_   = &c;
    b.__left_     = 0;
    b.__right_    = 0;
    b.__is_black_ = false;

    d.__parent_   = &c;
    d.__left_     = &a;
    d.__right_    = 0;
    d.__is_black_ = false;

    a.__parent_   = &d;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    mstd::tree_balance_after_insert(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &c);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &b);
    EXPECT_EQ(c.__right_, &d);
    EXPECT_EQ(c.__is_black_, true);

    EXPECT_EQ(b.__parent_, &c);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &c);
    EXPECT_EQ(d.__left_, &a);
    EXPECT_EQ(d.__right_, nullptr);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(a.__parent_, &d);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &b;
    c.__right_    = &d;
    c.__is_black_ = true;

    b.__parent_   = &c;
    b.__left_     = 0;
    b.__right_    = 0;
    b.__is_black_ = false;

    d.__parent_   = &c;
    d.__left_     = 0;
    d.__right_    = &a;
    d.__is_black_ = false;

    a.__parent_   = &d;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    mstd::tree_balance_after_insert(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &c);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &b);
    EXPECT_EQ(c.__right_, &d);
    EXPECT_EQ(c.__is_black_, true);

    EXPECT_EQ(b.__parent_, &c);
    EXPECT_EQ(b.__left_, nullptr);
    EXPECT_EQ(b.__right_, nullptr);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &c);
    EXPECT_EQ(d.__left_, nullptr);
    EXPECT_EQ(d.__right_, &a);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(a.__parent_, &d);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;
    Node e;
    Node f;
    Node g;
    Node h;
    Node i;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &b;
    c.__right_    = &d;
    c.__is_black_ = true;

    b.__parent_   = &c;
    b.__left_     = &a;
    b.__right_    = &g;
    b.__is_black_ = false;

    d.__parent_   = &c;
    d.__left_     = &h;
    d.__right_    = &i;
    d.__is_black_ = false;

    a.__parent_   = &b;
    a.__left_     = &e;
    a.__right_    = &f;
    a.__is_black_ = false;

    e.__parent_   = &a;
    e.__is_black_ = true;

    f.__parent_   = &a;
    f.__is_black_ = true;

    g.__parent_   = &b;
    g.__is_black_ = true;

    h.__parent_   = &d;
    h.__is_black_ = true;

    i.__parent_   = &d;
    i.__is_black_ = true;

    mstd::tree_balance_after_insert(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &c);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &b);
    EXPECT_EQ(c.__right_, &d);
    EXPECT_EQ(c.__is_black_, true);

    EXPECT_EQ(b.__parent_, &c);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, &g);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &c);
    EXPECT_EQ(d.__left_, &h);
    EXPECT_EQ(d.__right_, &i);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, &e);
    EXPECT_EQ(a.__right_, &f);
    EXPECT_EQ(a.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;
    Node e;
    Node f;
    Node g;
    Node h;
    Node i;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &b;
    c.__right_    = &d;
    c.__is_black_ = true;

    b.__parent_   = &c;
    b.__left_     = &g;
    b.__right_    = &a;
    b.__is_black_ = false;

    d.__parent_   = &c;
    d.__left_     = &h;
    d.__right_    = &i;
    d.__is_black_ = false;

    a.__parent_   = &b;
    a.__left_     = &e;
    a.__right_    = &f;
    a.__is_black_ = false;

    e.__parent_   = &a;
    e.__is_black_ = true;

    f.__parent_   = &a;
    f.__is_black_ = true;

    g.__parent_   = &b;
    g.__is_black_ = true;

    h.__parent_   = &d;
    h.__is_black_ = true;

    i.__parent_   = &d;
    i.__is_black_ = true;

    mstd::tree_balance_after_insert(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &c);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &b);
    EXPECT_EQ(c.__right_, &d);
    EXPECT_EQ(c.__is_black_, true);

    EXPECT_EQ(b.__parent_, &c);
    EXPECT_EQ(b.__left_, &g);
    EXPECT_EQ(b.__right_, &a);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &c);
    EXPECT_EQ(d.__left_, &h);
    EXPECT_EQ(d.__right_, &i);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, &e);
    EXPECT_EQ(a.__right_, &f);
    EXPECT_EQ(a.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;
    Node e;
    Node f;
    Node g;
    Node h;
    Node i;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &b;
    c.__right_    = &d;
    c.__is_black_ = true;

    b.__parent_   = &c;
    b.__left_     = &g;
    b.__right_    = &h;
    b.__is_black_ = false;

    d.__parent_   = &c;
    d.__left_     = &a;
    d.__right_    = &i;
    d.__is_black_ = false;

    a.__parent_   = &d;
    a.__left_     = &e;
    a.__right_    = &f;
    a.__is_black_ = false;

    e.__parent_   = &a;
    e.__is_black_ = true;

    f.__parent_   = &a;
    f.__is_black_ = true;

    g.__parent_   = &b;
    g.__is_black_ = true;

    h.__parent_   = &b;
    h.__is_black_ = true;

    i.__parent_   = &d;
    i.__is_black_ = true;

    mstd::tree_balance_after_insert(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &c);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &b);
    EXPECT_EQ(c.__right_, &d);
    EXPECT_EQ(c.__is_black_, true);

    EXPECT_EQ(b.__parent_, &c);
    EXPECT_EQ(b.__left_, &g);
    EXPECT_EQ(b.__right_, &h);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &c);
    EXPECT_EQ(d.__left_, &a);
    EXPECT_EQ(d.__right_, &i);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(a.__parent_, &d);
    EXPECT_EQ(a.__left_, &e);
    EXPECT_EQ(a.__right_, &f);
    EXPECT_EQ(a.__is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;
    Node e;
    Node f;
    Node g;
    Node h;
    Node i;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &b;
    c.__right_    = &d;
    c.__is_black_ = true;

    b.__parent_   = &c;
    b.__left_     = &g;
    b.__right_    = &h;
    b.__is_black_ = false;

    d.__parent_   = &c;
    d.__left_     = &i;
    d.__right_    = &a;
    d.__is_black_ = false;

    a.__parent_   = &d;
    a.__left_     = &e;
    a.__right_    = &f;
    a.__is_black_ = false;

    e.__parent_   = &a;
    e.__is_black_ = true;

    f.__parent_   = &a;
    f.__is_black_ = true;

    g.__parent_   = &b;
    g.__is_black_ = true;

    h.__parent_   = &b;
    h.__is_black_ = true;

    i.__parent_   = &d;
    i.__is_black_ = true;

    mstd::tree_balance_after_insert(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &c);

    EXPECT_EQ(c.__parent_, &root);
    EXPECT_EQ(c.__left_, &b);
    EXPECT_EQ(c.__right_, &d);
    EXPECT_EQ(c.__is_black_, true);

    EXPECT_EQ(b.__parent_, &c);
    EXPECT_EQ(b.__left_, &g);
    EXPECT_EQ(b.__right_, &h);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &c);
    EXPECT_EQ(d.__left_, &i);
    EXPECT_EQ(d.__right_, &a);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(a.__parent_, &d);
    EXPECT_EQ(a.__left_, &e);
    EXPECT_EQ(a.__right_, &f);
    EXPECT_EQ(a.__is_black_, false);
  }
}

TEST(TreeBalanceAfterInsert, Test2) {
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &a;
    c.__right_    = 0;
    c.__is_black_ = true;

    a.__parent_   = &c;
    a.__left_     = 0;
    a.__right_    = &b;
    a.__is_black_ = false;

    b.__parent_   = &a;
    b.__left_     = 0;
    b.__right_    = 0;
    b.__is_black_ = false;

    mstd::tree_balance_after_insert(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &b);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, false);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &a;

    a.__parent_   = &root;
    a.__left_     = 0;
    a.__right_    = &c;
    a.__is_black_ = true;

    c.__parent_   = &a;
    c.__left_     = &b;
    c.__right_    = 0;
    c.__is_black_ = false;

    b.__parent_   = &c;
    b.__left_     = 0;
    b.__right_    = 0;
    b.__is_black_ = false;

    mstd::tree_balance_after_insert(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &b);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;
    Node e;
    Node f;
    Node g;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &a;
    c.__right_    = &g;
    c.__is_black_ = true;

    a.__parent_   = &c;
    a.__left_     = &d;
    a.__right_    = &b;
    a.__is_black_ = false;

    b.__parent_   = &a;
    b.__left_     = &e;
    b.__right_    = &f;
    b.__is_black_ = false;

    d.__parent_   = &a;
    d.__is_black_ = true;

    e.__parent_   = &b;
    e.__is_black_ = true;

    f.__parent_   = &b;
    f.__is_black_ = true;

    g.__parent_   = &c;
    g.__is_black_ = true;

    mstd::tree_balance_after_insert(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &b);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, &f);
    EXPECT_EQ(c.__right_, &g);
    EXPECT_EQ(c.__is_black_, false);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, &d);
    EXPECT_EQ(a.__right_, &e);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &a);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(e.__parent_, &a);
    EXPECT_EQ(e.__is_black_, true);

    EXPECT_EQ(f.__parent_, &c);
    EXPECT_EQ(f.__is_black_, true);

    EXPECT_EQ(g.__parent_, &c);
    EXPECT_EQ(g.__is_black_, true);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;
    Node e;
    Node f;
    Node g;

    root.__left_ = &a;

    a.__parent_   = &root;
    a.__left_     = &d;
    a.__right_    = &c;
    a.__is_black_ = true;

    c.__parent_   = &a;
    c.__left_     = &b;
    c.__right_    = &g;
    c.__is_black_ = false;

    b.__parent_   = &c;
    b.__left_     = &e;
    b.__right_    = &f;
    b.__is_black_ = false;

    d.__parent_   = &a;
    d.__is_black_ = true;

    e.__parent_   = &b;
    e.__is_black_ = true;

    f.__parent_   = &b;
    f.__is_black_ = true;

    g.__parent_   = &c;
    g.__is_black_ = true;

    mstd::tree_balance_after_insert(root.__left_, &b);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &b);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, &f);
    EXPECT_EQ(c.__right_, &g);
    EXPECT_EQ(c.__is_black_, false);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, &d);
    EXPECT_EQ(a.__right_, &e);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &a);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(e.__parent_, &a);
    EXPECT_EQ(e.__is_black_, true);

    EXPECT_EQ(f.__parent_, &c);
    EXPECT_EQ(f.__is_black_, true);

    EXPECT_EQ(g.__parent_, &c);
    EXPECT_EQ(g.__is_black_, true);
  }
}

TEST(TreeBalanceAfterInsert, Test3) {
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &b;
    c.__right_    = 0;
    c.__is_black_ = true;

    b.__parent_   = &c;
    b.__left_     = &a;
    b.__right_    = 0;
    b.__is_black_ = false;

    a.__parent_   = &b;
    a.__left_     = 0;
    a.__right_    = 0;
    a.__is_black_ = false;

    mstd::tree_balance_after_insert(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &b);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, false);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.__left_ = &a;

    a.__parent_   = &root;
    a.__left_     = 0;
    a.__right_    = &b;
    a.__is_black_ = true;

    b.__parent_   = &a;
    b.__left_     = 0;
    b.__right_    = &c;
    b.__is_black_ = false;

    c.__parent_   = &b;
    c.__left_     = 0;
    c.__right_    = 0;
    c.__is_black_ = false;

    mstd::tree_balance_after_insert(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &b);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, nullptr);
    EXPECT_EQ(a.__right_, nullptr);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, nullptr);
    EXPECT_EQ(c.__right_, nullptr);
    EXPECT_EQ(c.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;
    Node e;
    Node f;
    Node g;

    root.__left_ = &c;

    c.__parent_   = &root;
    c.__left_     = &b;
    c.__right_    = &g;
    c.__is_black_ = true;

    b.__parent_   = &c;
    b.__left_     = &a;
    b.__right_    = &f;
    b.__is_black_ = false;

    a.__parent_   = &b;
    a.__left_     = &d;
    a.__right_    = &e;
    a.__is_black_ = false;

    d.__parent_   = &a;
    d.__is_black_ = true;

    e.__parent_   = &a;
    e.__is_black_ = true;

    f.__parent_   = &b;
    f.__is_black_ = true;

    g.__parent_   = &c;
    g.__is_black_ = true;

    mstd::tree_balance_after_insert(root.__left_, &a);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &b);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, &f);
    EXPECT_EQ(c.__right_, &g);
    EXPECT_EQ(c.__is_black_, false);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, &d);
    EXPECT_EQ(a.__right_, &e);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &a);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(e.__parent_, &a);
    EXPECT_EQ(e.__is_black_, true);

    EXPECT_EQ(f.__parent_, &c);
    EXPECT_EQ(f.__is_black_, true);

    EXPECT_EQ(g.__parent_, &c);
    EXPECT_EQ(g.__is_black_, true);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;
    Node e;
    Node f;
    Node g;

    root.__left_ = &a;

    a.__parent_   = &root;
    a.__left_     = &d;
    a.__right_    = &b;
    a.__is_black_ = true;

    b.__parent_   = &a;
    b.__left_     = &e;
    b.__right_    = &c;
    b.__is_black_ = false;

    c.__parent_   = &b;
    c.__left_     = &f;
    c.__right_    = &g;
    c.__is_black_ = false;

    d.__parent_   = &a;
    d.__is_black_ = true;

    e.__parent_   = &b;
    e.__is_black_ = true;

    f.__parent_   = &c;
    f.__is_black_ = true;

    g.__parent_   = &c;
    g.__is_black_ = true;

    mstd::tree_balance_after_insert(root.__left_, &c);

    EXPECT_TRUE(mstd::tree_invariant(root.__left_));

    EXPECT_EQ(root.__left_, &b);

    EXPECT_EQ(c.__parent_, &b);
    EXPECT_EQ(c.__left_, &f);
    EXPECT_EQ(c.__right_, &g);
    EXPECT_EQ(c.__is_black_, false);

    EXPECT_EQ(a.__parent_, &b);
    EXPECT_EQ(a.__left_, &d);
    EXPECT_EQ(a.__right_, &e);
    EXPECT_EQ(a.__is_black_, false);

    EXPECT_EQ(b.__parent_, &root);
    EXPECT_EQ(b.__left_, &a);
    EXPECT_EQ(b.__right_, &c);
    EXPECT_EQ(b.__is_black_, true);

    EXPECT_EQ(d.__parent_, &a);
    EXPECT_EQ(d.__is_black_, true);

    EXPECT_EQ(e.__parent_, &a);
    EXPECT_EQ(e.__is_black_, true);

    EXPECT_EQ(f.__parent_, &c);
    EXPECT_EQ(f.__is_black_, true);

    EXPECT_EQ(g.__parent_, &c);
    EXPECT_EQ(g.__is_black_, true);
  }
}

TEST(TreeBalanceAfterInsert, Test4) {
  Node root;
  Node a;
  Node b;
  Node c;
  Node d;
  Node e;
  Node f;
  Node g;
  Node h;

  root.__left_ = &a;
  a.__parent_  = &root;

  mstd::tree_balance_after_insert(root.__left_, &a);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &a);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(a.__parent_, &root);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, true);

  a.__right_  = &b;
  b.__parent_ = &a;

  mstd::tree_balance_after_insert(root.__left_, &b);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &a);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(a.__parent_, &root);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, &b);
  EXPECT_EQ(a.__is_black_, true);

  EXPECT_EQ(b.__parent_, &a);
  EXPECT_EQ(b.__left_, nullptr);
  EXPECT_EQ(b.__right_, nullptr);
  EXPECT_EQ(b.__is_black_, false);

  b.__right_  = &c;
  c.__parent_ = &b;

  mstd::tree_balance_after_insert(root.__left_, &c);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &b);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, false);

  EXPECT_EQ(b.__parent_, &root);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, &c);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(c.__parent_, &b);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
  EXPECT_EQ(c.__is_black_, false);

  c.__right_  = &d;
  d.__parent_ = &c;

  mstd::tree_balance_after_insert(root.__left_, &d);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &b);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, true);

  EXPECT_EQ(b.__parent_, &root);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, &c);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(c.__parent_, &b);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, &d);
  EXPECT_EQ(c.__is_black_, true);

  EXPECT_EQ(d.__parent_, &c);
  EXPECT_EQ(d.__left_, nullptr);
  EXPECT_EQ(d.__right_, nullptr);
  EXPECT_EQ(d.__is_black_, false);

  d.__right_  = &e;
  e.__parent_ = &d;

  mstd::tree_balance_after_insert(root.__left_, &e);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &b);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(b.__parent_, &root);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, &d);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, true);

  EXPECT_EQ(d.__parent_, &b);
  EXPECT_EQ(d.__left_, &c);
  EXPECT_EQ(d.__right_, &e);
  EXPECT_EQ(d.__is_black_, true);

  EXPECT_EQ(c.__parent_, &d);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
  EXPECT_EQ(c.__is_black_, false);

  EXPECT_EQ(e.__parent_, &d);
  EXPECT_EQ(e.__left_, nullptr);
  EXPECT_EQ(e.__right_, nullptr);
  EXPECT_EQ(e.__is_black_, false);

  e.__right_  = &f;
  f.__parent_ = &e;

  mstd::tree_balance_after_insert(root.__left_, &f);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &b);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(b.__parent_, &root);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, &d);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, true);

  EXPECT_EQ(d.__parent_, &b);
  EXPECT_EQ(d.__left_, &c);
  EXPECT_EQ(d.__right_, &e);
  EXPECT_EQ(d.__is_black_, false);

  EXPECT_EQ(c.__parent_, &d);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
  EXPECT_EQ(c.__is_black_, true);

  EXPECT_EQ(e.__parent_, &d);
  EXPECT_EQ(e.__left_, nullptr);
  EXPECT_EQ(e.__right_, &f);
  EXPECT_EQ(e.__is_black_, true);

  EXPECT_EQ(f.__parent_, &e);
  EXPECT_EQ(f.__left_, nullptr);
  EXPECT_EQ(f.__right_, nullptr);
  EXPECT_EQ(f.__is_black_, false);

  f.__right_  = &g;
  g.__parent_ = &f;

  mstd::tree_balance_after_insert(root.__left_, &g);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &b);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(b.__parent_, &root);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, &d);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, true);

  EXPECT_EQ(d.__parent_, &b);
  EXPECT_EQ(d.__left_, &c);
  EXPECT_EQ(d.__right_, &f);
  EXPECT_EQ(d.__is_black_, false);

  EXPECT_EQ(c.__parent_, &d);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
  EXPECT_EQ(c.__is_black_, true);

  EXPECT_EQ(f.__parent_, &d);
  EXPECT_EQ(f.__left_, &e);
  EXPECT_EQ(f.__right_, &g);
  EXPECT_EQ(f.__is_black_, true);

  EXPECT_EQ(e.__parent_, &f);
  EXPECT_EQ(e.__left_, nullptr);
  EXPECT_EQ(e.__right_, nullptr);
  EXPECT_EQ(e.__is_black_, false);

  EXPECT_EQ(g.__parent_, &f);
  EXPECT_EQ(g.__left_, nullptr);
  EXPECT_EQ(g.__right_, nullptr);
  EXPECT_EQ(g.__is_black_, false);

  g.__right_  = &h;
  h.__parent_ = &g;

  mstd::tree_balance_after_insert(root.__left_, &h);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &d);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(d.__parent_, &root);
  EXPECT_EQ(d.__left_, &b);
  EXPECT_EQ(d.__right_, &f);
  EXPECT_EQ(d.__is_black_, true);

  EXPECT_EQ(b.__parent_, &d);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, &c);
  EXPECT_EQ(b.__is_black_, false);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, true);

  EXPECT_EQ(c.__parent_, &b);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
  EXPECT_EQ(c.__is_black_, true);

  EXPECT_EQ(f.__parent_, &d);
  EXPECT_EQ(f.__left_, &e);
  EXPECT_EQ(f.__right_, &g);
  EXPECT_EQ(f.__is_black_, false);

  EXPECT_EQ(e.__parent_, &f);
  EXPECT_EQ(e.__left_, nullptr);
  EXPECT_EQ(e.__right_, nullptr);
  EXPECT_EQ(e.__is_black_, true);

  EXPECT_EQ(g.__parent_, &f);
  EXPECT_EQ(g.__left_, nullptr);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, false);
}

TEST(TreeBalanceAfterInsert, Test5) {
  Node root;
  Node a;
  Node b;
  Node c;
  Node d;
  Node e;
  Node f;
  Node g;
  Node h;

  root.__left_ = &h;
  h.__parent_  = &root;

  mstd::tree_balance_after_insert(root.__left_, &h);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &h);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(h.__parent_, &root);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, true);

  h.__left_   = &g;
  g.__parent_ = &h;

  mstd::tree_balance_after_insert(root.__left_, &g);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &h);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(h.__parent_, &root);
  EXPECT_EQ(h.__left_, &g);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, true);

  EXPECT_EQ(g.__parent_, &h);
  EXPECT_EQ(g.__left_, nullptr);
  EXPECT_EQ(g.__right_, nullptr);
  EXPECT_EQ(g.__is_black_, false);

  g.__left_   = &f;
  f.__parent_ = &g;

  mstd::tree_balance_after_insert(root.__left_, &f);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &g);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(g.__parent_, &root);
  EXPECT_EQ(g.__left_, &f);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(f.__parent_, &g);
  EXPECT_EQ(f.__left_, nullptr);
  EXPECT_EQ(f.__right_, nullptr);
  EXPECT_EQ(f.__is_black_, false);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, false);

  f.__left_   = &e;
  e.__parent_ = &f;

  mstd::tree_balance_after_insert(root.__left_, &e);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &g);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(g.__parent_, &root);
  EXPECT_EQ(g.__left_, &f);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(f.__parent_, &g);
  EXPECT_EQ(f.__left_, &e);
  EXPECT_EQ(f.__right_, nullptr);
  EXPECT_EQ(f.__is_black_, true);

  EXPECT_EQ(e.__parent_, &f);
  EXPECT_EQ(e.__left_, nullptr);
  EXPECT_EQ(e.__right_, nullptr);
  EXPECT_EQ(e.__is_black_, false);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, true);

  e.__left_   = &d;
  d.__parent_ = &e;

  mstd::tree_balance_after_insert(root.__left_, &d);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &g);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(g.__parent_, &root);
  EXPECT_EQ(g.__left_, &e);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(e.__parent_, &g);
  EXPECT_EQ(e.__left_, &d);
  EXPECT_EQ(e.__right_, &f);
  EXPECT_EQ(e.__is_black_, true);

  EXPECT_EQ(d.__parent_, &e);
  EXPECT_EQ(d.__left_, nullptr);
  EXPECT_EQ(d.__right_, nullptr);
  EXPECT_EQ(d.__is_black_, false);

  EXPECT_EQ(f.__parent_, &e);
  EXPECT_EQ(f.__left_, nullptr);
  EXPECT_EQ(f.__right_, nullptr);
  EXPECT_EQ(f.__is_black_, false);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, true);

  d.__left_   = &c;
  c.__parent_ = &d;

  mstd::tree_balance_after_insert(root.__left_, &c);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &g);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(g.__parent_, &root);
  EXPECT_EQ(g.__left_, &e);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(e.__parent_, &g);
  EXPECT_EQ(e.__left_, &d);
  EXPECT_EQ(e.__right_, &f);
  EXPECT_EQ(e.__is_black_, false);

  EXPECT_EQ(d.__parent_, &e);
  EXPECT_EQ(d.__left_, &c);
  EXPECT_EQ(d.__right_, nullptr);
  EXPECT_EQ(d.__is_black_, true);

  EXPECT_EQ(c.__parent_, &d);
  EXPECT_EQ(c.__left_, nullptr);
  EXPECT_EQ(c.__right_, nullptr);
  EXPECT_EQ(c.__is_black_, false);

  EXPECT_EQ(f.__parent_, &e);
  EXPECT_EQ(f.__left_, nullptr);
  EXPECT_EQ(f.__right_, nullptr);
  EXPECT_EQ(f.__is_black_, true);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, true);

  c.__left_   = &b;
  b.__parent_ = &c;

  mstd::tree_balance_after_insert(root.__left_, &b);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &g);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(g.__parent_, &root);
  EXPECT_EQ(g.__left_, &e);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, true);

  EXPECT_EQ(e.__parent_, &g);
  EXPECT_EQ(e.__left_, &c);
  EXPECT_EQ(e.__right_, &f);
  EXPECT_EQ(e.__is_black_, false);

  EXPECT_EQ(c.__parent_, &e);
  EXPECT_EQ(c.__left_, &b);
  EXPECT_EQ(c.__right_, &d);
  EXPECT_EQ(c.__is_black_, true);

  EXPECT_EQ(b.__parent_, &c);
  EXPECT_EQ(b.__left_, nullptr);
  EXPECT_EQ(b.__right_, nullptr);
  EXPECT_EQ(b.__is_black_, false);

  EXPECT_EQ(d.__parent_, &c);
  EXPECT_EQ(d.__left_, nullptr);
  EXPECT_EQ(d.__right_, nullptr);
  EXPECT_EQ(d.__is_black_, false);

  EXPECT_EQ(f.__parent_, &e);
  EXPECT_EQ(f.__left_, nullptr);
  EXPECT_EQ(f.__right_, nullptr);
  EXPECT_EQ(f.__is_black_, true);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, true);

  b.__left_   = &a;
  a.__parent_ = &b;

  mstd::tree_balance_after_insert(root.__left_, &a);

  EXPECT_TRUE(mstd::tree_invariant(root.__left_));

  EXPECT_EQ(root.__parent_, nullptr);
  EXPECT_EQ(root.__left_, &e);
  EXPECT_EQ(root.__right_, nullptr);
  EXPECT_EQ(root.__is_black_, false);

  EXPECT_EQ(e.__parent_, &root);
  EXPECT_EQ(e.__left_, &c);
  EXPECT_EQ(e.__right_, &g);
  EXPECT_EQ(e.__is_black_, true);

  EXPECT_EQ(c.__parent_, &e);
  EXPECT_EQ(c.__left_, &b);
  EXPECT_EQ(c.__right_, &d);
  EXPECT_EQ(c.__is_black_, false);

  EXPECT_EQ(b.__parent_, &c);
  EXPECT_EQ(b.__left_, &a);
  EXPECT_EQ(b.__right_, nullptr);
  EXPECT_EQ(b.__is_black_, true);

  EXPECT_EQ(a.__parent_, &b);
  EXPECT_EQ(a.__left_, nullptr);
  EXPECT_EQ(a.__right_, nullptr);
  EXPECT_EQ(a.__is_black_, false);

  EXPECT_EQ(d.__parent_, &c);
  EXPECT_EQ(d.__left_, nullptr);
  EXPECT_EQ(d.__right_, nullptr);
  EXPECT_EQ(d.__is_black_, true);

  EXPECT_EQ(g.__parent_, &e);
  EXPECT_EQ(g.__left_, &f);
  EXPECT_EQ(g.__right_, &h);
  EXPECT_EQ(g.__is_black_, false);

  EXPECT_EQ(f.__parent_, &g);
  EXPECT_EQ(f.__left_, nullptr);
  EXPECT_EQ(f.__right_, nullptr);
  EXPECT_EQ(f.__is_black_, true);

  EXPECT_EQ(h.__parent_, &g);
  EXPECT_EQ(h.__left_, nullptr);
  EXPECT_EQ(h.__right_, nullptr);
  EXPECT_EQ(h.__is_black_, true);
}
