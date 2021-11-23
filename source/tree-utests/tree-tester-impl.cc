
#include "tree-tester.hh"

namespace tree
{

bool TreeTester::verify(const TestTree &tree)
{
  if (!tree.DFS<StructTester>(StructTester{tree}))
    return false;
  if (!tree.DFS<ColorsTester>(ColorsTester{tree}))
    return false;

  return true;
}

bool TreeTester::StructTester::operator()(TestNode *node) const noexcept
{
  if (node->data_.passToggle(passToggle_))
    return false;

  TestNode *left = node->left_;
  TestNode *right = node->right_;

  if (left != nullptr)
    if (left->parent_ != node)
      return false;

  if (right != nullptr)
    if (right->parent_ != node)
      return false;

  return ++passedNum_ <= treeSize_;
}

bool TreeTester::SizesTester::operator()(const TestNode *node) const noexcept
{
  const TestNode *l = node->left_;
  const TestNode *r = node->right_;
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

bool TreeTester::ColorsTester::operator()(TestNode *node) const
{
  if (node == root_ && TestNode::getColor(node) != TestTree::Color::BLACK)
    return false;

  if (TestNode::getColor(node) == TestTree::Color::RED && TestNode::getColor(node->parent_) == TestTree::Color::RED)
    return false;

  // Black heights check.
  size_t blackHeight = 0;
  if (node->left_ == nullptr && node->right_ == nullptr)
  {
    while (node != nullptr)
    {
      if (TestNode::getColor(node) == TestTree::Color::BLACK)
        ++blackHeight;

      auto hIt = blackHeights_.find(node);
      if (hIt == blackHeights_.end())
        blackHeights_.emplace(node, blackHeight);
      else if (hIt->second != blackHeight)
        return false;

      node = node->parent_;
    }
  }

  return true;
}

} // namespace tree
