
#include "tree.hh"

#ifndef TREE_IMPL_HH_INCL
#define TREE_IMPL_HH_INCL

namespace tree
{

template <class Data, class Compare>
typename StatTree<Data, Compare>::Iterator StatTree<Data, Compare>::find(const Data &toFind) const
{
  Node *curNode = root_;

  while (curNode != nullptr)
  {
    const Data &curData = curNode->data_;

    if (toFind > curData)
      curNode = curNode->right_;
    else if (toFind < curData)
      curNode = curNode->left_;
    else
      break;
  }

  if (curNode == nullptr)
    return end();

  return Iterator{curNode};
}

template <class Data, class Compare>
void StatTree<Data, Compare>::transplant(Node *old, Node *replacing)
{
  if (old->parent_ == nullptr)
    root_ = replacing;
  else if (old->parent_->left_ == old)
    old->parent_->left_ = replacing;
  else
    old->parent_->right_ = replacing;
  if (replacing != nullptr)
    replacing->parent_ = old->parent_;
}

// Rotate a node x to the right
//        x                y
//       / |              / |
//      y   c    --->    a   x
//     / |                  / |
//    a   b                b   c
template <class Data, class Compare>
void StatTree<Data, Compare>::rRotation(Node *x)
{
  Node *y = x->left_;
  assert(y != 0);

  Node *parent = x->parent_;
  y->parent_ = parent;

  if (x == parent->left_)
  {
    // x is the left son of its parent
    parent->left_ = y;
  }
  else
  {
    // x is the right son of its parent
    parent->right_ = y;
  }

  x->left_ = y->right_;

  if (y->right_ != 0)
    y->right_->parent_ = x;
  y->right_ = x;
  x->parent_ = y;
}

// Rotate a node x to the left
//        x                y
//       / |              / |
//      a   y    --->    x   c
//         / |          / |
//        b   c        a   b
template <class Data, class Compare>
void StatTree<Data, Compare>::lRotation(Node *x)
{
  Node *y = x->right_;
  assert(y != 0);

  x->right_ = y->left_;

  if (y->left_ != 0)
    y->left_->parent_ = x;

  y->parent_ = x->parent_;
  if (x->parent_ == nullptr)
    root_ = y;
  else if (x == x->parent_->left_)
    x->parent_->left_ = y;
  else
    x->parent_->right_ = y;

  y->left_ = x;
  x->parent_ = y;
}

template <class Data, class Compare>
void StatTree<Data, Compare>::erase(Iterator delIt)
{
  if (delIt == end())
    return;

  Node *del = delIt.ptr_;

  // Stuff for eraseFixup.
  auto toCheck = Node::getColor(del);
  Node *toFix = nullptr;
  Node *toFixParent = del;

  if (del->left_ == nullptr)
  {
    toFix = del->right_;
    transplant(del, toFix);
  }
  else if (del->right_ == nullptr)
  {
    toFix = del->left_;
    transplant(del, toFix);
  }
  else
  {
    // Next node after del.
    Node *next = del->right_;
    while (next->left_ != nullptr)
      next = next->left_;
    toCheck = Node::getColor(next);

    toFixParent = next;
    toFix = next->right_;

    if (next->parent_ != del)
    {
      toFixParent = next->parent_;
      transplant(next, next->right_);
      next->right_ = del->right_;
      next->right_->parent_ = next;
    }

    transplant(del, next);
    next->left_ = del->left_;
    if (next->left_ != nullptr)
      next->left_->parent_ = next;
    next->color_ = del->color_;
  }

  if (toCheck == Color::BLACK)
    eraseFixup(toFix, toFixParent);

  delete del;
}

template <class Data, class Compare>
void StatTree<Data, Compare>::eraseFixup(Node *toFix, Node *toFixParent)
{
  while (toFix != root_ && Node::getColor(toFix) == Color::BLACK)
  {
    // To run through all variants without copypaste (just simple renaming in right child case).
    auto left = toFix == toFixParent->left_ ? Side::LEFT : Side::RIGHT;
    auto right = left == Side::LEFT ? Side::RIGHT : Side::LEFT;

    // Not nil because of black depth invariant compliance.
    Node *brother = Node::getChild(toFixParent, right);
    if (brother->color_ == Color::RED)
    {
      brother->color_ = Color::BLACK;
      toFixParent->color_ = Color::RED;
      rotation(toFixParent, left);

      brother = Node::getChild(toFixParent, right);
    }
    if (Node::getColor(brother->left_) == Color::BLACK && Node::getColor(brother->right_) == Color::BLACK)
    {
      brother->color_ = Color::RED;
      toFix = toFixParent;
      toFixParent = toFix->parent_;
    }
    else
    {
      if (Node::getColor(Node::getChild(brother, right)) == Color::BLACK)
      {
        Node::getChild(brother, left)->color_ = Color::BLACK;
        brother->color_ = Color::RED;
        rotation(brother, right);

        brother = Node::getChild(toFixParent, right);
      }

      brother->color_ = toFixParent->color_;
      toFixParent->color_ = Color::BLACK;
      brother->right_->color_ = Color::BLACK;
      rotation(toFixParent, left);

      toFix = root_;
    }
  }

  toFix->color_ = Color::BLACK;
}

template <class Data, class Compare>
void StatTree<Data, Compare>::insertFixup(Node *z)
{
  while (z != root_ && Node::getColor(z->parent_) == Color::RED)
  {
    if (z->parent_ == z->parent_->parent_->left_)
    {
      Node *y = z->parent_->parent_->right_;
      if (Node::getColor(y) == Color::RED)
      {
        z->parent_->color_ = Color::BLACK;
        z->parent_->color_ = Color::BLACK;
        z->parent_->parent_->color_ = Color::RED;
        z = z->parent_->parent_;
      }
      else
      {
        if (z == z->parent_->right_)
        {
          z = z->parent_;
          lRotation(z);
        }
        z->parent_->color_ = Color::BLACK;
        z->parent_->parent_->color_ = Color::RED;
        rRotation(z->parent_->parent_);
      }
    }
    else
    {
      Node *y = z->parent_->parent_->left_;
      if (Node::getColor(y) == Color::RED)
      {
        z->parent_->color_ = Color::BLACK;
        y->color_ = Color::BLACK;
        z->parent_->parent_->color_ = Color::RED;
        z = z->parent_->parent_;
      }
      else
      {
        if (z == z->parent_->left_)
        {
          z = z->parent_;
          rRotation(z);
        }
        z->parent_->color_ = Color::BLACK;
        z->parent_->parent_->color_ = Color::RED;
        lRotation(z->parent_->parent_);
      }
    }
  }
  root_->color_ = Color::BLACK;
}

template <class Data, class Compare>
typename tree::StatTree<Data, Compare>::Node tree::StatTree<Data, Compare>::insert(const Data &new_data)
{
  Node *current = root_;
  Node *parent = 0;
  Node *x = new Node;

  // find where node belongs
  while (current != nullptr)
  {
    if (new_data == current->data_)
    {
      return (*current);
    }
    parent = current;
    current = (new_data < current->data_) ? current->left_ : current->right_;
  }

  x->data_ = new_data;
  x->parent_ = parent;
  x->left_ = nullptr;
  x->right_ = nullptr;
  x->color_ = Color::RED;

  // insert node in tree
  if (parent)
  {
    if (new_data < parent->data_)
    {
      parent->left_ = x;
    }
    else
    {
      parent->right_ = x;
    }
  }
  else
  {
    root_ = x;
  }

  insertFixup(x);
  return *x;
}

template <class Data, class Compare>
bool StatTree<Data, Compare>::verify() const
{
  if (!DFS<StatTree<Data, Compare>::StructTester>())
    return false;
  if (!DFS<StatTree<Data, Compare>::SizesTester>())
    return false;

  return true;
}

template <class Data, class Compare>
template <class Tester>
bool StatTree<Data, Compare>::DFS() const
{
  Tester tester{*this};

  Node *curNode = root_;
  std::vector<bool> rightChildPassed(size_, false);
  size_t depth = 0;

  while (curNode != nullptr)
  {
    if (!tester(curNode))
      return false;

    if (curNode->left_ != nullptr)
    {
      rightChildPassed[depth++] = false;
      curNode = curNode->left_;
      continue;
    }

    if (curNode->right_ != nullptr)
    {
      rightChildPassed[depth++] = true;
      curNode = curNode->right_;
      continue;
    }

    for (curNode = curNode->parent_, --depth; curNode != nullptr;)
    {
      if (!rightChildPassed[depth] && curNode->right_ != nullptr)
      {
        rightChildPassed[depth++] = true;
        curNode = curNode->right_;
        break;
      }

      curNode = curNode->parent_;
    }
  }

  return true;
}

template <class Data, class Compare>
bool StatTree<Data, Compare>::StructTester::operator()(Node *node) noexcept
{
  if (node->passToggle(passToggle_))
    return false;

  Node *left = node->left_;
  Node *right = node->right_;

  if (left != nullptr)
    if (left->parent_ != node)
      return false;

  if (right != nullptr)
    if (right->parent_ != node)
      return false;

  return ++passedNum_ <= treeSize_;
}

template <class Data, class Compare>
bool StatTree<Data, Compare>::SizesTester::operator()(const Node *node) noexcept
{
  const Node *l = node->left_;
  const Node *r = node->right_;
  size_t rSz = node->rightSize_;
  size_t lSz = node->leftSize_;

  if (!rootPassed_)
  {
    rootPassed_ = true;
    if (treeSize_ != lSz + rSz + 1)
      return false;
  }

  if (l == nullptr)
  {
    if (lSz != 0)
      return false;
  }
  else
  {
    if (lSz != l->rightSize_ + l->leftSize_ + 1)
      return false;
  }

  if (r == nullptr)
  {
    if (rSz != 0)
      return false;
  }
  else
  {
    if (rSz != r->rightSize_ + r->leftSize_ + 1)
      return false;
  }

  return true;
}

template <class Data, class Compare>
bool StatTree<Data, Compare>::Dumper::operator()(const Node *node) noexcept
{
  if (node == nullptr)
    return true;

  std::cout << "\"" << node << "\" -> "
            << " \"" << node->right_ << "\"" << std::endl;
  std::cout << "\"" << node << "\" -> "
            << " \"" << node->left_ << "\"" << std::endl;
  return true;
}

template <class Data, class Compare>
bool StatTree<Data, Compare>::dump() const
{
  return DFS<tree::StatTree<int>::Dumper>();
}

template <class Data, class Compare>
bool StatTree<Data, Compare>::ColorsTester::operator()(const Node *node)
{
  if (node == root_ && Node::getColor(node) != Color::BLACK)
    return false;

  if (Node::getColor(node) == Color::RED && Node::getColor(node->parent_) == Color::RED)
    return false;

  // Black heights check.
  size_t blackHeight = 0;
  if (node->left_ == nullptr && node->right_ == nullptr)
  {
    while (node != nullptr)
    {
      if (Node::getColor(node) == Color::BLACK)
        ++blackHeight;

      auto hIt = blackHeights_.find(node);
      if (hIt == blackHeights_.end())
        blackHeights_.emplace(node, blackHeight);
      else if (hIt.second != blackHeight)
        return false;

      node = node->parent_;
    }
  }

  return true;
}

} // namespace tree

#endif
