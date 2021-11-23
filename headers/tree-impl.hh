
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

  return Iterator{curNode, false};
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
void StatTree<Data, Compare>::rRotation(Node *node)
{
  Node *left_node = node->left_;
  assert(left_node != nullptr);

  Node *parent = node->parent_;
  left_node->parent_ = parent;

  if (left_node->right_ != nullptr)
    left_node->right_->parent_ = node;

  if (parent == nullptr)
    root_ = left_node;
  else if (node == parent->left_)
    parent->left_ = left_node;
  else
    parent->right_ = left_node;

  node->left_ = left_node->right_;

  left_node->right_ = node;
  node->parent_ = left_node;
}

// Rotate a node x to the left
//        x                y
//       / |              / |
//      a   y    --->    x   c
//         / |          / |
//        b   c        a   b
template <class Data, class Compare>
void StatTree<Data, Compare>::lRotation(Node *node)
{
  Node *right_node = node->right_;
  assert(right_node != nullptr);

  node->right_ = right_node->left_;

  if (right_node->left_ != nullptr)
    right_node->left_->parent_ = node;

  right_node->parent_ = node->parent_;
  if (node->parent_ == nullptr)
    root_ = right_node;
  else if (node == node->parent_->left_)
    node->parent_->left_ = right_node;
  else
    node->parent_->right_ = right_node;

  right_node->left_ = node;
  node->parent_ = right_node;
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
    toFixParent = del->parent_;
    transplant(del, toFix);
  }
  else if (del->right_ == nullptr)
  {
    toFix = del->left_;
    toFixParent = del->parent_;
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
typename StatTree<Data, Compare>::Node StatTree<Data, Compare>::insert(const Data &new_data)
{
  Node *cur_root = nullptr;
  Node *root = root_;
  Node *new_node = new Node{new_data};

  while (root != nullptr)
  {
    cur_root = root;
    if (new_node->data_ < root->data_)
      root = root->left_;
    else
      root = root->right_;
  }

  new_node->parent_ = cur_root;

  if (cur_root == nullptr)
    root_ = new_node;
  else if (new_node->data_ < cur_root->data_)
    cur_root->left_ = new_node;
  else
    cur_root->right_ = new_node;

  new_node->left_ = nullptr;
  new_node->right_ = nullptr;
  new_node->color_ = Color::RED;

  insertFixup(new_node);
  return (*new_node);
}

template <class Data, class Compare>
void StatTree<Data, Compare>::insertFixup(Node *node)
{
  while (Node::getColor(node->parent_) == Color::RED)
  {
    if (node->parent_ == node->parent_->parent_->left_)
    {
      Node *ppleft_node = node->parent_->parent_->right_;
      if (Node::getColor(ppleft_node) == Color::RED)
      {
        node->parent_->color_ = Color::BLACK;
        ppleft_node->color_ = Color::BLACK;
        node->parent_->parent_->color_ = Color::RED;
        node = node->parent_->parent_;
      }
      else
      {
        if (node == node->parent_->right_)
        {
          node = node->parent_;
          lRotation(node);
        }
        node->parent_->color_ = Color::BLACK;
        node->parent_->parent_->color_ = Color::RED;
        rRotation(node->parent_->parent_);
      }
    }
    else
    {
      Node *ppleft_node = node->parent_->parent_->left_;
      if (Node::getColor(ppleft_node) == Color::RED)
      {
        node->parent_->color_ = Color::BLACK;
        ppleft_node->color_ = Color::BLACK;
        node->parent_->parent_->color_ = Color::RED;
        node = node->parent_->parent_;
      }
      else
      {
        if (node == node->parent_->left_)
        {
          node = node->parent_;
          rRotation(node);
        }
        node->parent_->color_ = Color::BLACK;
        node->parent_->parent_->color_ = Color::RED;
        lRotation(node->parent_->parent_);
      }
    }
  }
  root_->color_ = Color::BLACK;
}

template <class Data, class Compare>
size_t StatTree<Data, Compare>::countLesser(const Node *node, const size_t m) const
{
  if (m <= node->Node::leftSize_)
    return countLesser(node->left_, m);
  else if (m > Node::leftSize_)
  {
    m = m - (Node::leftSize_ + 1);
    if (Node::leftSize_ + 1 == m)
      return node->data_;
    else if (Node::leftSize_ + 1 < m)
      return countLesser(node->right_, m);
  }
}

template <class Data, class Compare>
template <class Callable>
bool StatTree<Data, Compare>::DFS(const Callable &callable) const
{
  Node *curNode = root_;
  std::vector<bool> rightChildPassed{};
  size_t depth = 0;

  while (curNode != nullptr)
  {
    if (!callable(curNode))
      return false;

    if (depth == rightChildPassed.size())
      rightChildPassed.push_back(false);

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

    for (curNode = curNode->parent_, --depth; curNode != nullptr; --depth)
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
bool StatTree<Data, Compare>::dump() const
{
  return DFS<tree::StatTree<Data, Compare>::Dumper>(Dumper{*this});
}

template <class Data, class Compare>
bool StatTree<Data, Compare>::Dumper::operator()(const Node *node) const noexcept
{
  std::ofstream out("tree.txt", std::ios::app);
  if (out.is_open())
  {
    if (node == nullptr)
    {
      return true;
    }

    if (node->left_ != nullptr)
      out << "\"" << node->data_ << "\" -> "
          << " \"" << node->left_->data_ << "\""
          << ";" << std::endl;
    else
    {
      ++cout_nils;
      out << "\"" << node->data_ << "\" -> "
          << " \""
          << "nil" << cout_nils << "\""
          << ";" << std::endl;
      out << "\""
          << "nil" << cout_nils << "\""
          << "[style=\"filled\",fontcolor=\"white\",fillcolor="
          << "\""
          << "BLACK"
          << "\"];" << std::endl;
    }

    if (node->right_ != nullptr)
      out << "\"" << node->data_ << "\" -> "
          << " \"" << node->right_->data_ << "\""
          << "[style="
          << "\""
          << "dotted"
          << "\""
          << "];" << std::endl;
    else
    {
      ++cout_nils;
      out << "\"" << node->data_ << "\" -> "
          << " \""
          << "nil" << cout_nils << "\""
          << "[style="
          << "\""
          << "dotted"
          << "\""
          << "];" << std::endl;
      out << "\""
          << "nil" << cout_nils << "\""
          << "[style=\"filled\",fontcolor=\"white\",fillcolor="
          << "\""
          << "BLACK"
          << "\"];" << std::endl;
    }

    if (node != nullptr && node->color_ == Color::RED)
      out << "\"" << node->data_ << "\""
          << "[style=\"filled\",fontcolor=\"white\",fillcolor="
          << "\""
          << "RED"
          << "\"];" << std::endl;
    else if (node == nullptr || node->color_ == Color::BLACK)
      out << "\"" << node->data_ << "\""
          << "[style=\"filled\",fontcolor=\"white\",fillcolor="
          << "\""
          << "BLACK"
          << "\"];" << std::endl;
  }
  out.close();
  return true;
}

} // namespace tree

#endif
