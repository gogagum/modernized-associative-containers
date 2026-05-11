#include <gtest/gtest.h>

#include <detail/tree.hpp>

struct Node {
  Node* left_;
  Node* right_;
  Node* parent_;
  bool is_black_;

  Node* parent_unsafe() const { return parent_; }
  void set_parent(Node* x) { parent_ = x; }

  Node() : left_(), right_(), parent_(), is_black_() {}
};

TEST(TreeRotate, Right1) {
  Node root;
  Node x;
  Node y;
  root.left_ = &x;
  x.left_    = &y;
  x.right_   = 0;
  x.parent_  = &root;
  y.left_    = 0;
  y.right_   = 0;
  y.parent_  = &x;
  mstd::detail::tree_right_rotate(&x);
  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &y);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(y.parent_, &root);
  EXPECT_EQ(y.left_, nullptr);
  EXPECT_EQ(y.right_, &x);
  EXPECT_EQ(x.parent_, &y);
  EXPECT_EQ(x.left_, nullptr);
  EXPECT_EQ(x.right_, nullptr);
}

TEST(TreeRotate, Right2) {
  Node root;
  Node x;
  Node y;
  Node a;
  Node b;
  Node c;
  root.left_ = &x;
  x.left_    = &y;
  x.right_   = &c;
  x.parent_  = &root;
  y.left_    = &a;
  y.right_   = &b;
  y.parent_  = &x;
  a.parent_  = &y;
  b.parent_  = &y;
  c.parent_  = &x;
  mstd::detail::tree_right_rotate(&x);
  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &y);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(y.parent_, &root);
  EXPECT_EQ(y.left_, &a);
  EXPECT_EQ(y.right_, &x);
  EXPECT_EQ(x.parent_, &y);
  EXPECT_EQ(x.left_, &b);
  EXPECT_EQ(x.right_, &c);
  EXPECT_EQ(a.parent_, &y);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(b.parent_, &x);
  EXPECT_EQ(b.left_, nullptr);
  EXPECT_EQ(b.right_, nullptr);
  EXPECT_EQ(c.parent_, &x);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
}

TEST(TreeRotate, Left1) {
  Node root;
  Node x;
  Node y;
  root.left_ = &x;
  x.left_    = 0;
  x.right_   = &y;
  x.parent_  = &root;
  y.left_    = 0;
  y.right_   = 0;
  y.parent_  = &x;
  mstd::detail::tree_left_rotate(&x);
  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &y);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(y.parent_, &root);
  EXPECT_EQ(y.left_, &x);
  EXPECT_EQ(y.right_, nullptr);
  EXPECT_EQ(x.parent_, &y);
  EXPECT_EQ(x.left_, nullptr);
  EXPECT_EQ(x.right_, nullptr);
}

TEST(TreeRotate, Left2) {
  Node root;
  Node x;
  Node y;
  Node a;
  Node b;
  Node c;
  root.left_ = &x;
  x.left_    = &a;
  x.right_   = &y;
  x.parent_  = &root;
  y.left_    = &b;
  y.right_   = &c;
  y.parent_  = &x;
  a.parent_  = &x;
  b.parent_  = &y;
  c.parent_  = &y;
  mstd::detail::tree_left_rotate(&x);
  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &y);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(y.parent_, &root);
  EXPECT_EQ(y.left_, &x);
  EXPECT_EQ(y.right_, &c);
  EXPECT_EQ(x.parent_, &y);
  EXPECT_EQ(x.left_, &a);
  EXPECT_EQ(x.right_, &b);
  EXPECT_EQ(a.parent_, &x);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(b.parent_, &x);
  EXPECT_EQ(b.left_, nullptr);
  EXPECT_EQ(b.right_, nullptr);
  EXPECT_EQ(c.parent_, &y);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
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

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &y;
    b.right_    = &d;
    b.is_black_ = true;

    y.parent_   = &b;
    y.left_     = 0;
    y.right_    = 0;
    y.is_black_ = true;

    d.parent_   = &b;
    d.left_     = &c;
    d.right_    = &e;
    d.is_black_ = false;

    c.parent_   = &d;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = true;

    e.parent_   = &d;
    e.left_     = 0;
    e.right_    = 0;
    e.is_black_ = true;

    mstd::detail::tree_remove(root.left_, &y);
    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &d);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(d.parent_, &root);
    EXPECT_EQ(d.left_, &b);
    EXPECT_EQ(d.right_, &e);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(b.parent_, &d);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, false);

    EXPECT_EQ(e.parent_, &d);
    EXPECT_EQ(e.left_, nullptr);
    EXPECT_EQ(e.right_, nullptr);
    EXPECT_EQ(e.is_black_, true);
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

    root.left_ = &b;

    b.parent_   = &root;
    b.right_    = &y;
    b.left_     = &d;
    b.is_black_ = true;

    y.parent_   = &b;
    y.right_    = 0;
    y.left_     = 0;
    y.is_black_ = true;

    d.parent_   = &b;
    d.right_    = &c;
    d.left_     = &e;
    d.is_black_ = false;

    c.parent_   = &d;
    c.right_    = 0;
    c.left_     = 0;
    c.is_black_ = true;

    e.parent_   = &d;
    e.right_    = 0;
    e.left_     = 0;
    e.is_black_ = true;

    mstd::detail::tree_remove(root.left_, &y);
    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &d);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(d.parent_, &root);
    EXPECT_EQ(d.right_, &b);
    EXPECT_EQ(d.left_, &e);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(b.parent_, &d);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.left_, &c);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.is_black_, false);

    EXPECT_EQ(e.parent_, &d);
    EXPECT_EQ(e.right_, nullptr);
    EXPECT_EQ(e.left_, nullptr);
    EXPECT_EQ(e.is_black_, true);
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

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &y;
    b.right_    = &d;
    b.is_black_ = true;

    y.parent_   = &b;
    y.left_     = 0;
    y.right_    = 0;
    y.is_black_ = true;

    d.parent_   = &b;
    d.left_     = &c;
    d.right_    = &e;
    d.is_black_ = false;

    c.parent_   = &d;
    c.left_     = &f;
    c.right_    = 0;
    c.is_black_ = true;

    e.parent_   = &d;
    e.left_     = 0;
    e.right_    = 0;
    e.is_black_ = true;

    f.parent_   = &c;
    f.left_     = 0;
    f.right_    = 0;
    f.is_black_ = false;

    mstd::detail::tree_remove(root.left_, &y);
    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &d);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(d.parent_, &root);
    EXPECT_EQ(d.left_, &f);
    EXPECT_EQ(d.right_, &e);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(f.parent_, &d);
    EXPECT_EQ(f.left_, &b);
    EXPECT_EQ(f.right_, &c);
    EXPECT_EQ(f.is_black_, false);

    EXPECT_EQ(b.parent_, &f);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(c.parent_, &f);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, true);

    EXPECT_EQ(e.parent_, &d);
    EXPECT_EQ(e.left_, nullptr);
    EXPECT_EQ(e.right_, nullptr);
    EXPECT_EQ(e.is_black_, true);
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

    root.left_ = &b;

    b.parent_   = &root;
    b.right_    = &y;
    b.left_     = &d;
    b.is_black_ = true;

    y.parent_   = &b;
    y.right_    = 0;
    y.left_     = 0;
    y.is_black_ = true;

    d.parent_   = &b;
    d.right_    = &c;
    d.left_     = &e;
    d.is_black_ = false;

    c.parent_   = &d;
    c.right_    = &f;
    c.left_     = 0;
    c.is_black_ = true;

    e.parent_   = &d;
    e.right_    = 0;
    e.left_     = 0;
    e.is_black_ = true;

    f.parent_   = &c;
    f.right_    = 0;
    f.left_     = 0;
    f.is_black_ = false;

    mstd::detail::tree_remove(root.left_, &y);
    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &d);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(d.parent_, &root);
    EXPECT_EQ(d.right_, &f);
    EXPECT_EQ(d.left_, &e);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(f.parent_, &d);
    EXPECT_EQ(f.right_, &b);
    EXPECT_EQ(f.left_, &c);
    EXPECT_EQ(f.is_black_, false);

    EXPECT_EQ(b.parent_, &f);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(c.parent_, &f);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.is_black_, true);

    EXPECT_EQ(e.parent_, &d);
    EXPECT_EQ(e.right_, nullptr);
    EXPECT_EQ(e.left_, nullptr);
    EXPECT_EQ(e.is_black_, true);
  }
}

TEST(TreeRemove, Test2) {
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = true;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = true;

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, false);

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &c);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, true);

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = false;

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, false);

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &c);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, true);

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = true;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = true;

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, false);

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = false;

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, false);

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = true;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = true;

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &c);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &c);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &a);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, true);

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &c);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, true);

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = false;

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &c);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &c);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &a);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, true);

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &c);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, true);

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = true;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = true;

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &c);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &c);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &a);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, true);

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &a);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &root);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, true);

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = false;

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &c);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &c);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &a);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, true);

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &a);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &root);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, true);

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = true;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = true;

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &a);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &root);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, true);

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = false;

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &a);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &root);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, true);

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = true;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = true;

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &b;

    b.parent_   = &root;
    b.left_     = &a;
    b.right_    = &c;
    b.is_black_ = true;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = false;

    mstd::detail::tree_remove(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    mstd::detail::tree_remove(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, &b);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    mstd::detail::tree_remove(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.parent_, nullptr);
    EXPECT_EQ(root.left_, nullptr);
    EXPECT_EQ(root.right_, nullptr);
    EXPECT_EQ(root.is_black_, false);
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

  root.left_ = &e;

  e.parent_   = &root;
  e.left_     = &c;
  e.right_    = &g;
  e.is_black_ = true;

  c.parent_   = &e;
  c.left_     = &b;
  c.right_    = &d;
  c.is_black_ = false;

  g.parent_   = &e;
  g.left_     = &f;
  g.right_    = &h;
  g.is_black_ = false;

  b.parent_   = &c;
  b.left_     = &a;
  b.right_    = 0;
  b.is_black_ = true;

  d.parent_   = &c;
  d.left_     = 0;
  d.right_    = 0;
  d.is_black_ = true;

  f.parent_   = &g;
  f.left_     = 0;
  f.right_    = 0;
  f.is_black_ = true;

  h.parent_   = &g;
  h.left_     = 0;
  h.right_    = 0;
  h.is_black_ = true;

  a.parent_   = &b;
  a.left_     = 0;
  a.right_    = 0;
  a.is_black_ = false;

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  mstd::detail::tree_remove(root.left_, &h);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &e);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(e.parent_, &root);
  EXPECT_EQ(e.left_, &c);
  EXPECT_EQ(e.right_, &g);
  EXPECT_EQ(e.is_black_, true);

  EXPECT_EQ(c.parent_, &e);
  EXPECT_EQ(c.left_, &b);
  EXPECT_EQ(c.right_, &d);
  EXPECT_EQ(c.is_black_, false);

  EXPECT_EQ(g.parent_, &e);
  EXPECT_EQ(g.left_, &f);
  EXPECT_EQ(g.right_, nullptr);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(b.parent_, &c);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, nullptr);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, false);

  EXPECT_EQ(d.parent_, &c);
  EXPECT_EQ(d.left_, nullptr);
  EXPECT_EQ(d.right_, nullptr);
  EXPECT_EQ(d.is_black_, true);

  EXPECT_EQ(f.parent_, &g);
  EXPECT_EQ(f.left_, nullptr);
  EXPECT_EQ(f.right_, nullptr);
  EXPECT_EQ(f.is_black_, false);

  mstd::detail::tree_remove(root.left_, &g);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &e);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(e.parent_, &root);
  EXPECT_EQ(e.left_, &c);
  EXPECT_EQ(e.right_, &f);
  EXPECT_EQ(e.is_black_, true);

  EXPECT_EQ(c.parent_, &e);
  EXPECT_EQ(c.left_, &b);
  EXPECT_EQ(c.right_, &d);
  EXPECT_EQ(c.is_black_, false);

  EXPECT_EQ(b.parent_, &c);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, nullptr);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, false);

  EXPECT_EQ(d.parent_, &c);
  EXPECT_EQ(d.left_, nullptr);
  EXPECT_EQ(d.right_, nullptr);
  EXPECT_EQ(d.is_black_, true);

  EXPECT_EQ(f.parent_, &e);
  EXPECT_EQ(f.left_, nullptr);
  EXPECT_EQ(f.right_, nullptr);
  EXPECT_EQ(f.is_black_, true);

  mstd::detail::tree_remove(root.left_, &f);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &c);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(c.parent_, &root);
  EXPECT_EQ(c.left_, &b);
  EXPECT_EQ(c.right_, &e);
  EXPECT_EQ(c.is_black_, true);

  EXPECT_EQ(b.parent_, &c);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, nullptr);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(e.parent_, &c);
  EXPECT_EQ(e.left_, &d);
  EXPECT_EQ(e.right_, nullptr);
  EXPECT_EQ(e.is_black_, true);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, false);

  EXPECT_EQ(d.parent_, &e);
  EXPECT_EQ(d.left_, nullptr);
  EXPECT_EQ(d.right_, nullptr);
  EXPECT_EQ(d.is_black_, false);

  mstd::detail::tree_remove(root.left_, &e);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &c);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(c.parent_, &root);
  EXPECT_EQ(c.left_, &b);
  EXPECT_EQ(c.right_, &d);
  EXPECT_EQ(c.is_black_, true);

  EXPECT_EQ(b.parent_, &c);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, nullptr);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, false);

  EXPECT_EQ(d.parent_, &c);
  EXPECT_EQ(d.left_, nullptr);
  EXPECT_EQ(d.right_, nullptr);
  EXPECT_EQ(d.is_black_, true);

  mstd::detail::tree_remove(root.left_, &d);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &b);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(b.parent_, &root);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, &c);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, true);

  EXPECT_EQ(c.parent_, &b);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
  EXPECT_EQ(c.is_black_, true);

  mstd::detail::tree_remove(root.left_, &c);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &b);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(b.parent_, &root);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, nullptr);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, false);

  mstd::detail::tree_remove(root.left_, &b);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &a);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(a.parent_, &root);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, true);

  mstd::detail::tree_remove(root.left_, &a);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, nullptr);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);
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

  root.left_ = &d;

  d.parent_   = &root;
  d.left_     = &b;
  d.right_    = &f;
  d.is_black_ = true;

  b.parent_   = &d;
  b.left_     = &a;
  b.right_    = &c;
  b.is_black_ = false;

  f.parent_   = &d;
  f.left_     = &e;
  f.right_    = &g;
  f.is_black_ = false;

  a.parent_   = &b;
  a.left_     = 0;
  a.right_    = 0;
  a.is_black_ = true;

  c.parent_   = &b;
  c.left_     = 0;
  c.right_    = 0;
  c.is_black_ = true;

  e.parent_   = &f;
  e.left_     = 0;
  e.right_    = 0;
  e.is_black_ = true;

  g.parent_   = &f;
  g.left_     = 0;
  g.right_    = &h;
  g.is_black_ = true;

  h.parent_   = &g;
  h.left_     = 0;
  h.right_    = 0;
  h.is_black_ = false;

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  mstd::detail::tree_remove(root.left_, &a);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &d);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(d.parent_, &root);
  EXPECT_EQ(d.left_, &b);
  EXPECT_EQ(d.right_, &f);
  EXPECT_EQ(d.is_black_, true);

  EXPECT_EQ(b.parent_, &d);
  EXPECT_EQ(b.left_, nullptr);
  EXPECT_EQ(b.right_, &c);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(f.parent_, &d);
  EXPECT_EQ(f.left_, &e);
  EXPECT_EQ(f.right_, &g);
  EXPECT_EQ(f.is_black_, false);

  EXPECT_EQ(c.parent_, &b);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
  EXPECT_EQ(c.is_black_, false);

  EXPECT_EQ(e.parent_, &f);
  EXPECT_EQ(e.left_, nullptr);
  EXPECT_EQ(e.right_, nullptr);
  EXPECT_EQ(e.is_black_, true);

  EXPECT_EQ(g.parent_, &f);
  EXPECT_EQ(g.left_, nullptr);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, false);

  mstd::detail::tree_remove(root.left_, &b);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &d);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(d.parent_, &root);
  EXPECT_EQ(d.left_, &c);
  EXPECT_EQ(d.right_, &f);
  EXPECT_EQ(d.is_black_, true);

  EXPECT_EQ(c.parent_, &d);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
  EXPECT_EQ(c.is_black_, true);

  EXPECT_EQ(f.parent_, &d);
  EXPECT_EQ(f.left_, &e);
  EXPECT_EQ(f.right_, &g);
  EXPECT_EQ(f.is_black_, false);

  EXPECT_EQ(e.parent_, &f);
  EXPECT_EQ(e.left_, nullptr);
  EXPECT_EQ(e.right_, nullptr);
  EXPECT_EQ(e.is_black_, true);

  EXPECT_EQ(g.parent_, &f);
  EXPECT_EQ(g.left_, nullptr);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, false);

  mstd::detail::tree_remove(root.left_, &c);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &f);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(f.parent_, &root);
  EXPECT_EQ(f.left_, &d);
  EXPECT_EQ(f.right_, &g);
  EXPECT_EQ(f.is_black_, true);

  EXPECT_EQ(d.parent_, &f);
  EXPECT_EQ(d.left_, nullptr);
  EXPECT_EQ(d.right_, &e);
  EXPECT_EQ(d.is_black_, true);

  EXPECT_EQ(g.parent_, &f);
  EXPECT_EQ(g.left_, nullptr);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(e.parent_, &d);
  EXPECT_EQ(e.left_, nullptr);
  EXPECT_EQ(e.right_, nullptr);
  EXPECT_EQ(e.is_black_, false);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, false);

  mstd::detail::tree_remove(root.left_, &d);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &f);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(f.parent_, &root);
  EXPECT_EQ(f.left_, &e);
  EXPECT_EQ(f.right_, &g);
  EXPECT_EQ(f.is_black_, true);

  EXPECT_EQ(e.parent_, &f);
  EXPECT_EQ(e.left_, nullptr);
  EXPECT_EQ(e.right_, nullptr);
  EXPECT_EQ(e.is_black_, true);

  EXPECT_EQ(g.parent_, &f);
  EXPECT_EQ(g.left_, nullptr);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, false);

  mstd::detail::tree_remove(root.left_, &e);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &g);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(g.parent_, &root);
  EXPECT_EQ(g.left_, &f);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(f.parent_, &g);
  EXPECT_EQ(f.left_, nullptr);
  EXPECT_EQ(f.right_, nullptr);
  EXPECT_EQ(f.is_black_, true);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, true);

  mstd::detail::tree_remove(root.left_, &f);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &g);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(g.parent_, &root);
  EXPECT_EQ(g.left_, nullptr);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, false);

  mstd::detail::tree_remove(root.left_, &g);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &h);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(h.parent_, &root);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, true);

  mstd::detail::tree_remove(root.left_, &h);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, nullptr);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);
}

TEST(TreeBalanceAfterInsert, Test1) {
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &b;
    c.right_    = &d;
    c.is_black_ = true;

    b.parent_   = &c;
    b.left_     = &a;
    b.right_    = 0;
    b.is_black_ = false;

    d.parent_   = &c;
    d.left_     = 0;
    d.right_    = 0;
    d.is_black_ = false;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    mstd::detail::tree_balance_after_insert(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &c);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &b);
    EXPECT_EQ(c.right_, &d);
    EXPECT_EQ(c.is_black_, true);

    EXPECT_EQ(b.parent_, &c);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &c);
    EXPECT_EQ(d.left_, nullptr);
    EXPECT_EQ(d.right_, nullptr);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &b;
    c.right_    = &d;
    c.is_black_ = true;

    b.parent_   = &c;
    b.left_     = 0;
    b.right_    = &a;
    b.is_black_ = false;

    d.parent_   = &c;
    d.left_     = 0;
    d.right_    = 0;
    d.is_black_ = false;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    mstd::detail::tree_balance_after_insert(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &c);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &b);
    EXPECT_EQ(c.right_, &d);
    EXPECT_EQ(c.is_black_, true);

    EXPECT_EQ(b.parent_, &c);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, &a);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &c);
    EXPECT_EQ(d.left_, nullptr);
    EXPECT_EQ(d.right_, nullptr);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &b;
    c.right_    = &d;
    c.is_black_ = true;

    b.parent_   = &c;
    b.left_     = 0;
    b.right_    = 0;
    b.is_black_ = false;

    d.parent_   = &c;
    d.left_     = &a;
    d.right_    = 0;
    d.is_black_ = false;

    a.parent_   = &d;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    mstd::detail::tree_balance_after_insert(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &c);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &b);
    EXPECT_EQ(c.right_, &d);
    EXPECT_EQ(c.is_black_, true);

    EXPECT_EQ(b.parent_, &c);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &c);
    EXPECT_EQ(d.left_, &a);
    EXPECT_EQ(d.right_, nullptr);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(a.parent_, &d);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;
    Node d;

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &b;
    c.right_    = &d;
    c.is_black_ = true;

    b.parent_   = &c;
    b.left_     = 0;
    b.right_    = 0;
    b.is_black_ = false;

    d.parent_   = &c;
    d.left_     = 0;
    d.right_    = &a;
    d.is_black_ = false;

    a.parent_   = &d;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    mstd::detail::tree_balance_after_insert(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &c);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &b);
    EXPECT_EQ(c.right_, &d);
    EXPECT_EQ(c.is_black_, true);

    EXPECT_EQ(b.parent_, &c);
    EXPECT_EQ(b.left_, nullptr);
    EXPECT_EQ(b.right_, nullptr);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &c);
    EXPECT_EQ(d.left_, nullptr);
    EXPECT_EQ(d.right_, &a);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(a.parent_, &d);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);
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

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &b;
    c.right_    = &d;
    c.is_black_ = true;

    b.parent_   = &c;
    b.left_     = &a;
    b.right_    = &g;
    b.is_black_ = false;

    d.parent_   = &c;
    d.left_     = &h;
    d.right_    = &i;
    d.is_black_ = false;

    a.parent_   = &b;
    a.left_     = &e;
    a.right_    = &f;
    a.is_black_ = false;

    e.parent_   = &a;
    e.is_black_ = true;

    f.parent_   = &a;
    f.is_black_ = true;

    g.parent_   = &b;
    g.is_black_ = true;

    h.parent_   = &d;
    h.is_black_ = true;

    i.parent_   = &d;
    i.is_black_ = true;

    mstd::detail::tree_balance_after_insert(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &c);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &b);
    EXPECT_EQ(c.right_, &d);
    EXPECT_EQ(c.is_black_, true);

    EXPECT_EQ(b.parent_, &c);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, &g);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &c);
    EXPECT_EQ(d.left_, &h);
    EXPECT_EQ(d.right_, &i);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, &e);
    EXPECT_EQ(a.right_, &f);
    EXPECT_EQ(a.is_black_, false);
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

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &b;
    c.right_    = &d;
    c.is_black_ = true;

    b.parent_   = &c;
    b.left_     = &g;
    b.right_    = &a;
    b.is_black_ = false;

    d.parent_   = &c;
    d.left_     = &h;
    d.right_    = &i;
    d.is_black_ = false;

    a.parent_   = &b;
    a.left_     = &e;
    a.right_    = &f;
    a.is_black_ = false;

    e.parent_   = &a;
    e.is_black_ = true;

    f.parent_   = &a;
    f.is_black_ = true;

    g.parent_   = &b;
    g.is_black_ = true;

    h.parent_   = &d;
    h.is_black_ = true;

    i.parent_   = &d;
    i.is_black_ = true;

    mstd::detail::tree_balance_after_insert(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &c);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &b);
    EXPECT_EQ(c.right_, &d);
    EXPECT_EQ(c.is_black_, true);

    EXPECT_EQ(b.parent_, &c);
    EXPECT_EQ(b.left_, &g);
    EXPECT_EQ(b.right_, &a);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &c);
    EXPECT_EQ(d.left_, &h);
    EXPECT_EQ(d.right_, &i);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, &e);
    EXPECT_EQ(a.right_, &f);
    EXPECT_EQ(a.is_black_, false);
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

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &b;
    c.right_    = &d;
    c.is_black_ = true;

    b.parent_   = &c;
    b.left_     = &g;
    b.right_    = &h;
    b.is_black_ = false;

    d.parent_   = &c;
    d.left_     = &a;
    d.right_    = &i;
    d.is_black_ = false;

    a.parent_   = &d;
    a.left_     = &e;
    a.right_    = &f;
    a.is_black_ = false;

    e.parent_   = &a;
    e.is_black_ = true;

    f.parent_   = &a;
    f.is_black_ = true;

    g.parent_   = &b;
    g.is_black_ = true;

    h.parent_   = &b;
    h.is_black_ = true;

    i.parent_   = &d;
    i.is_black_ = true;

    mstd::detail::tree_balance_after_insert(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &c);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &b);
    EXPECT_EQ(c.right_, &d);
    EXPECT_EQ(c.is_black_, true);

    EXPECT_EQ(b.parent_, &c);
    EXPECT_EQ(b.left_, &g);
    EXPECT_EQ(b.right_, &h);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &c);
    EXPECT_EQ(d.left_, &a);
    EXPECT_EQ(d.right_, &i);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(a.parent_, &d);
    EXPECT_EQ(a.left_, &e);
    EXPECT_EQ(a.right_, &f);
    EXPECT_EQ(a.is_black_, false);
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

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &b;
    c.right_    = &d;
    c.is_black_ = true;

    b.parent_   = &c;
    b.left_     = &g;
    b.right_    = &h;
    b.is_black_ = false;

    d.parent_   = &c;
    d.left_     = &i;
    d.right_    = &a;
    d.is_black_ = false;

    a.parent_   = &d;
    a.left_     = &e;
    a.right_    = &f;
    a.is_black_ = false;

    e.parent_   = &a;
    e.is_black_ = true;

    f.parent_   = &a;
    f.is_black_ = true;

    g.parent_   = &b;
    g.is_black_ = true;

    h.parent_   = &b;
    h.is_black_ = true;

    i.parent_   = &d;
    i.is_black_ = true;

    mstd::detail::tree_balance_after_insert(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &c);

    EXPECT_EQ(c.parent_, &root);
    EXPECT_EQ(c.left_, &b);
    EXPECT_EQ(c.right_, &d);
    EXPECT_EQ(c.is_black_, true);

    EXPECT_EQ(b.parent_, &c);
    EXPECT_EQ(b.left_, &g);
    EXPECT_EQ(b.right_, &h);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &c);
    EXPECT_EQ(d.left_, &i);
    EXPECT_EQ(d.right_, &a);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(a.parent_, &d);
    EXPECT_EQ(a.left_, &e);
    EXPECT_EQ(a.right_, &f);
    EXPECT_EQ(a.is_black_, false);
  }
}

TEST(TreeBalanceAfterInsert, Test2) {
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &a;
    c.right_    = 0;
    c.is_black_ = true;

    a.parent_   = &c;
    a.left_     = 0;
    a.right_    = &b;
    a.is_black_ = false;

    b.parent_   = &a;
    b.left_     = 0;
    b.right_    = 0;
    b.is_black_ = false;

    mstd::detail::tree_balance_after_insert(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &b);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, false);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &a;

    a.parent_   = &root;
    a.left_     = 0;
    a.right_    = &c;
    a.is_black_ = true;

    c.parent_   = &a;
    c.left_     = &b;
    c.right_    = 0;
    c.is_black_ = false;

    b.parent_   = &c;
    b.left_     = 0;
    b.right_    = 0;
    b.is_black_ = false;

    mstd::detail::tree_balance_after_insert(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &b);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);
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

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &a;
    c.right_    = &g;
    c.is_black_ = true;

    a.parent_   = &c;
    a.left_     = &d;
    a.right_    = &b;
    a.is_black_ = false;

    b.parent_   = &a;
    b.left_     = &e;
    b.right_    = &f;
    b.is_black_ = false;

    d.parent_   = &a;
    d.is_black_ = true;

    e.parent_   = &b;
    e.is_black_ = true;

    f.parent_   = &b;
    f.is_black_ = true;

    g.parent_   = &c;
    g.is_black_ = true;

    mstd::detail::tree_balance_after_insert(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &b);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, &f);
    EXPECT_EQ(c.right_, &g);
    EXPECT_EQ(c.is_black_, false);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, &d);
    EXPECT_EQ(a.right_, &e);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &a);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(e.parent_, &a);
    EXPECT_EQ(e.is_black_, true);

    EXPECT_EQ(f.parent_, &c);
    EXPECT_EQ(f.is_black_, true);

    EXPECT_EQ(g.parent_, &c);
    EXPECT_EQ(g.is_black_, true);
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

    root.left_ = &a;

    a.parent_   = &root;
    a.left_     = &d;
    a.right_    = &c;
    a.is_black_ = true;

    c.parent_   = &a;
    c.left_     = &b;
    c.right_    = &g;
    c.is_black_ = false;

    b.parent_   = &c;
    b.left_     = &e;
    b.right_    = &f;
    b.is_black_ = false;

    d.parent_   = &a;
    d.is_black_ = true;

    e.parent_   = &b;
    e.is_black_ = true;

    f.parent_   = &b;
    f.is_black_ = true;

    g.parent_   = &c;
    g.is_black_ = true;

    mstd::detail::tree_balance_after_insert(root.left_, &b);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &b);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, &f);
    EXPECT_EQ(c.right_, &g);
    EXPECT_EQ(c.is_black_, false);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, &d);
    EXPECT_EQ(a.right_, &e);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &a);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(e.parent_, &a);
    EXPECT_EQ(e.is_black_, true);

    EXPECT_EQ(f.parent_, &c);
    EXPECT_EQ(f.is_black_, true);

    EXPECT_EQ(g.parent_, &c);
    EXPECT_EQ(g.is_black_, true);
  }
}

TEST(TreeBalanceAfterInsert, Test3) {
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &b;
    c.right_    = 0;
    c.is_black_ = true;

    b.parent_   = &c;
    b.left_     = &a;
    b.right_    = 0;
    b.is_black_ = false;

    a.parent_   = &b;
    a.left_     = 0;
    a.right_    = 0;
    a.is_black_ = false;

    mstd::detail::tree_balance_after_insert(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &b);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, false);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);
  }
  {
    Node root;
    Node a;
    Node b;
    Node c;

    root.left_ = &a;

    a.parent_   = &root;
    a.left_     = 0;
    a.right_    = &b;
    a.is_black_ = true;

    b.parent_   = &a;
    b.left_     = 0;
    b.right_    = &c;
    b.is_black_ = false;

    c.parent_   = &b;
    c.left_     = 0;
    c.right_    = 0;
    c.is_black_ = false;

    mstd::detail::tree_balance_after_insert(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &b);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, nullptr);
    EXPECT_EQ(a.right_, nullptr);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, nullptr);
    EXPECT_EQ(c.right_, nullptr);
    EXPECT_EQ(c.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);
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

    root.left_ = &c;

    c.parent_   = &root;
    c.left_     = &b;
    c.right_    = &g;
    c.is_black_ = true;

    b.parent_   = &c;
    b.left_     = &a;
    b.right_    = &f;
    b.is_black_ = false;

    a.parent_   = &b;
    a.left_     = &d;
    a.right_    = &e;
    a.is_black_ = false;

    d.parent_   = &a;
    d.is_black_ = true;

    e.parent_   = &a;
    e.is_black_ = true;

    f.parent_   = &b;
    f.is_black_ = true;

    g.parent_   = &c;
    g.is_black_ = true;

    mstd::detail::tree_balance_after_insert(root.left_, &a);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &b);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, &f);
    EXPECT_EQ(c.right_, &g);
    EXPECT_EQ(c.is_black_, false);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, &d);
    EXPECT_EQ(a.right_, &e);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &a);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(e.parent_, &a);
    EXPECT_EQ(e.is_black_, true);

    EXPECT_EQ(f.parent_, &c);
    EXPECT_EQ(f.is_black_, true);

    EXPECT_EQ(g.parent_, &c);
    EXPECT_EQ(g.is_black_, true);
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

    root.left_ = &a;

    a.parent_   = &root;
    a.left_     = &d;
    a.right_    = &b;
    a.is_black_ = true;

    b.parent_   = &a;
    b.left_     = &e;
    b.right_    = &c;
    b.is_black_ = false;

    c.parent_   = &b;
    c.left_     = &f;
    c.right_    = &g;
    c.is_black_ = false;

    d.parent_   = &a;
    d.is_black_ = true;

    e.parent_   = &b;
    e.is_black_ = true;

    f.parent_   = &c;
    f.is_black_ = true;

    g.parent_   = &c;
    g.is_black_ = true;

    mstd::detail::tree_balance_after_insert(root.left_, &c);

    EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

    EXPECT_EQ(root.left_, &b);

    EXPECT_EQ(c.parent_, &b);
    EXPECT_EQ(c.left_, &f);
    EXPECT_EQ(c.right_, &g);
    EXPECT_EQ(c.is_black_, false);

    EXPECT_EQ(a.parent_, &b);
    EXPECT_EQ(a.left_, &d);
    EXPECT_EQ(a.right_, &e);
    EXPECT_EQ(a.is_black_, false);

    EXPECT_EQ(b.parent_, &root);
    EXPECT_EQ(b.left_, &a);
    EXPECT_EQ(b.right_, &c);
    EXPECT_EQ(b.is_black_, true);

    EXPECT_EQ(d.parent_, &a);
    EXPECT_EQ(d.is_black_, true);

    EXPECT_EQ(e.parent_, &a);
    EXPECT_EQ(e.is_black_, true);

    EXPECT_EQ(f.parent_, &c);
    EXPECT_EQ(f.is_black_, true);

    EXPECT_EQ(g.parent_, &c);
    EXPECT_EQ(g.is_black_, true);
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

  root.left_ = &a;
  a.parent_  = &root;

  mstd::detail::tree_balance_after_insert(root.left_, &a);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &a);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(a.parent_, &root);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, true);

  a.right_  = &b;
  b.parent_ = &a;

  mstd::detail::tree_balance_after_insert(root.left_, &b);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &a);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(a.parent_, &root);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, &b);
  EXPECT_EQ(a.is_black_, true);

  EXPECT_EQ(b.parent_, &a);
  EXPECT_EQ(b.left_, nullptr);
  EXPECT_EQ(b.right_, nullptr);
  EXPECT_EQ(b.is_black_, false);

  b.right_  = &c;
  c.parent_ = &b;

  mstd::detail::tree_balance_after_insert(root.left_, &c);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &b);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, false);

  EXPECT_EQ(b.parent_, &root);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, &c);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(c.parent_, &b);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
  EXPECT_EQ(c.is_black_, false);

  c.right_  = &d;
  d.parent_ = &c;

  mstd::detail::tree_balance_after_insert(root.left_, &d);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &b);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, true);

  EXPECT_EQ(b.parent_, &root);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, &c);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(c.parent_, &b);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, &d);
  EXPECT_EQ(c.is_black_, true);

  EXPECT_EQ(d.parent_, &c);
  EXPECT_EQ(d.left_, nullptr);
  EXPECT_EQ(d.right_, nullptr);
  EXPECT_EQ(d.is_black_, false);

  d.right_  = &e;
  e.parent_ = &d;

  mstd::detail::tree_balance_after_insert(root.left_, &e);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &b);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(b.parent_, &root);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, &d);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, true);

  EXPECT_EQ(d.parent_, &b);
  EXPECT_EQ(d.left_, &c);
  EXPECT_EQ(d.right_, &e);
  EXPECT_EQ(d.is_black_, true);

  EXPECT_EQ(c.parent_, &d);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
  EXPECT_EQ(c.is_black_, false);

  EXPECT_EQ(e.parent_, &d);
  EXPECT_EQ(e.left_, nullptr);
  EXPECT_EQ(e.right_, nullptr);
  EXPECT_EQ(e.is_black_, false);

  e.right_  = &f;
  f.parent_ = &e;

  mstd::detail::tree_balance_after_insert(root.left_, &f);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &b);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(b.parent_, &root);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, &d);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, true);

  EXPECT_EQ(d.parent_, &b);
  EXPECT_EQ(d.left_, &c);
  EXPECT_EQ(d.right_, &e);
  EXPECT_EQ(d.is_black_, false);

  EXPECT_EQ(c.parent_, &d);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
  EXPECT_EQ(c.is_black_, true);

  EXPECT_EQ(e.parent_, &d);
  EXPECT_EQ(e.left_, nullptr);
  EXPECT_EQ(e.right_, &f);
  EXPECT_EQ(e.is_black_, true);

  EXPECT_EQ(f.parent_, &e);
  EXPECT_EQ(f.left_, nullptr);
  EXPECT_EQ(f.right_, nullptr);
  EXPECT_EQ(f.is_black_, false);

  f.right_  = &g;
  g.parent_ = &f;

  mstd::detail::tree_balance_after_insert(root.left_, &g);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &b);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(b.parent_, &root);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, &d);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, true);

  EXPECT_EQ(d.parent_, &b);
  EXPECT_EQ(d.left_, &c);
  EXPECT_EQ(d.right_, &f);
  EXPECT_EQ(d.is_black_, false);

  EXPECT_EQ(c.parent_, &d);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
  EXPECT_EQ(c.is_black_, true);

  EXPECT_EQ(f.parent_, &d);
  EXPECT_EQ(f.left_, &e);
  EXPECT_EQ(f.right_, &g);
  EXPECT_EQ(f.is_black_, true);

  EXPECT_EQ(e.parent_, &f);
  EXPECT_EQ(e.left_, nullptr);
  EXPECT_EQ(e.right_, nullptr);
  EXPECT_EQ(e.is_black_, false);

  EXPECT_EQ(g.parent_, &f);
  EXPECT_EQ(g.left_, nullptr);
  EXPECT_EQ(g.right_, nullptr);
  EXPECT_EQ(g.is_black_, false);

  g.right_  = &h;
  h.parent_ = &g;

  mstd::detail::tree_balance_after_insert(root.left_, &h);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &d);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(d.parent_, &root);
  EXPECT_EQ(d.left_, &b);
  EXPECT_EQ(d.right_, &f);
  EXPECT_EQ(d.is_black_, true);

  EXPECT_EQ(b.parent_, &d);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, &c);
  EXPECT_EQ(b.is_black_, false);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, true);

  EXPECT_EQ(c.parent_, &b);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
  EXPECT_EQ(c.is_black_, true);

  EXPECT_EQ(f.parent_, &d);
  EXPECT_EQ(f.left_, &e);
  EXPECT_EQ(f.right_, &g);
  EXPECT_EQ(f.is_black_, false);

  EXPECT_EQ(e.parent_, &f);
  EXPECT_EQ(e.left_, nullptr);
  EXPECT_EQ(e.right_, nullptr);
  EXPECT_EQ(e.is_black_, true);

  EXPECT_EQ(g.parent_, &f);
  EXPECT_EQ(g.left_, nullptr);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, false);
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

  root.left_ = &h;
  h.parent_  = &root;

  mstd::detail::tree_balance_after_insert(root.left_, &h);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &h);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(h.parent_, &root);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, true);

  h.left_   = &g;
  g.parent_ = &h;

  mstd::detail::tree_balance_after_insert(root.left_, &g);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &h);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(h.parent_, &root);
  EXPECT_EQ(h.left_, &g);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, true);

  EXPECT_EQ(g.parent_, &h);
  EXPECT_EQ(g.left_, nullptr);
  EXPECT_EQ(g.right_, nullptr);
  EXPECT_EQ(g.is_black_, false);

  g.left_   = &f;
  f.parent_ = &g;

  mstd::detail::tree_balance_after_insert(root.left_, &f);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &g);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(g.parent_, &root);
  EXPECT_EQ(g.left_, &f);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(f.parent_, &g);
  EXPECT_EQ(f.left_, nullptr);
  EXPECT_EQ(f.right_, nullptr);
  EXPECT_EQ(f.is_black_, false);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, false);

  f.left_   = &e;
  e.parent_ = &f;

  mstd::detail::tree_balance_after_insert(root.left_, &e);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &g);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(g.parent_, &root);
  EXPECT_EQ(g.left_, &f);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(f.parent_, &g);
  EXPECT_EQ(f.left_, &e);
  EXPECT_EQ(f.right_, nullptr);
  EXPECT_EQ(f.is_black_, true);

  EXPECT_EQ(e.parent_, &f);
  EXPECT_EQ(e.left_, nullptr);
  EXPECT_EQ(e.right_, nullptr);
  EXPECT_EQ(e.is_black_, false);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, true);

  e.left_   = &d;
  d.parent_ = &e;

  mstd::detail::tree_balance_after_insert(root.left_, &d);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &g);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(g.parent_, &root);
  EXPECT_EQ(g.left_, &e);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(e.parent_, &g);
  EXPECT_EQ(e.left_, &d);
  EXPECT_EQ(e.right_, &f);
  EXPECT_EQ(e.is_black_, true);

  EXPECT_EQ(d.parent_, &e);
  EXPECT_EQ(d.left_, nullptr);
  EXPECT_EQ(d.right_, nullptr);
  EXPECT_EQ(d.is_black_, false);

  EXPECT_EQ(f.parent_, &e);
  EXPECT_EQ(f.left_, nullptr);
  EXPECT_EQ(f.right_, nullptr);
  EXPECT_EQ(f.is_black_, false);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, true);

  d.left_   = &c;
  c.parent_ = &d;

  mstd::detail::tree_balance_after_insert(root.left_, &c);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &g);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(g.parent_, &root);
  EXPECT_EQ(g.left_, &e);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(e.parent_, &g);
  EXPECT_EQ(e.left_, &d);
  EXPECT_EQ(e.right_, &f);
  EXPECT_EQ(e.is_black_, false);

  EXPECT_EQ(d.parent_, &e);
  EXPECT_EQ(d.left_, &c);
  EXPECT_EQ(d.right_, nullptr);
  EXPECT_EQ(d.is_black_, true);

  EXPECT_EQ(c.parent_, &d);
  EXPECT_EQ(c.left_, nullptr);
  EXPECT_EQ(c.right_, nullptr);
  EXPECT_EQ(c.is_black_, false);

  EXPECT_EQ(f.parent_, &e);
  EXPECT_EQ(f.left_, nullptr);
  EXPECT_EQ(f.right_, nullptr);
  EXPECT_EQ(f.is_black_, true);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, true);

  c.left_   = &b;
  b.parent_ = &c;

  mstd::detail::tree_balance_after_insert(root.left_, &b);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &g);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(g.parent_, &root);
  EXPECT_EQ(g.left_, &e);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, true);

  EXPECT_EQ(e.parent_, &g);
  EXPECT_EQ(e.left_, &c);
  EXPECT_EQ(e.right_, &f);
  EXPECT_EQ(e.is_black_, false);

  EXPECT_EQ(c.parent_, &e);
  EXPECT_EQ(c.left_, &b);
  EXPECT_EQ(c.right_, &d);
  EXPECT_EQ(c.is_black_, true);

  EXPECT_EQ(b.parent_, &c);
  EXPECT_EQ(b.left_, nullptr);
  EXPECT_EQ(b.right_, nullptr);
  EXPECT_EQ(b.is_black_, false);

  EXPECT_EQ(d.parent_, &c);
  EXPECT_EQ(d.left_, nullptr);
  EXPECT_EQ(d.right_, nullptr);
  EXPECT_EQ(d.is_black_, false);

  EXPECT_EQ(f.parent_, &e);
  EXPECT_EQ(f.left_, nullptr);
  EXPECT_EQ(f.right_, nullptr);
  EXPECT_EQ(f.is_black_, true);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, true);

  b.left_   = &a;
  a.parent_ = &b;

  mstd::detail::tree_balance_after_insert(root.left_, &a);

  EXPECT_TRUE(mstd::detail::tree_invariant(root.left_));

  EXPECT_EQ(root.parent_, nullptr);
  EXPECT_EQ(root.left_, &e);
  EXPECT_EQ(root.right_, nullptr);
  EXPECT_EQ(root.is_black_, false);

  EXPECT_EQ(e.parent_, &root);
  EXPECT_EQ(e.left_, &c);
  EXPECT_EQ(e.right_, &g);
  EXPECT_EQ(e.is_black_, true);

  EXPECT_EQ(c.parent_, &e);
  EXPECT_EQ(c.left_, &b);
  EXPECT_EQ(c.right_, &d);
  EXPECT_EQ(c.is_black_, false);

  EXPECT_EQ(b.parent_, &c);
  EXPECT_EQ(b.left_, &a);
  EXPECT_EQ(b.right_, nullptr);
  EXPECT_EQ(b.is_black_, true);

  EXPECT_EQ(a.parent_, &b);
  EXPECT_EQ(a.left_, nullptr);
  EXPECT_EQ(a.right_, nullptr);
  EXPECT_EQ(a.is_black_, false);

  EXPECT_EQ(d.parent_, &c);
  EXPECT_EQ(d.left_, nullptr);
  EXPECT_EQ(d.right_, nullptr);
  EXPECT_EQ(d.is_black_, true);

  EXPECT_EQ(g.parent_, &e);
  EXPECT_EQ(g.left_, &f);
  EXPECT_EQ(g.right_, &h);
  EXPECT_EQ(g.is_black_, false);

  EXPECT_EQ(f.parent_, &g);
  EXPECT_EQ(f.left_, nullptr);
  EXPECT_EQ(f.right_, nullptr);
  EXPECT_EQ(f.is_black_, true);

  EXPECT_EQ(h.parent_, &g);
  EXPECT_EQ(h.left_, nullptr);
  EXPECT_EQ(h.right_, nullptr);
  EXPECT_EQ(h.is_black_, true);
}
