
#include "tree.hh"
#include <iostream>

#ifndef TREE_TESTER_HH_INCL
#define TREE_TESTER_HH_INCL

namespace tree
{

struct TreeTester
{
  // Node data with test stuff
  struct TestData
  {
    size_t value_ = 0;
    bool passToggle_ = false;

    TestData(size_t value) : value_{value}
    {}

    // Checks if that node were toggled with taken treePassToggle
    // value last time and toggles passToggle_ to do such checks in future.
    // Used in verify passes to avoid looping.
    bool passToggle(bool treePassToggle) noexcept
    {
      bool curToggleVal = passToggle_;
      return curToggleVal == (passToggle_ = treePassToggle);
    }

    std::strong_ordering operator<=>(const TestData &) const = default;
  };

  using TestTree = StatTree<TestData>;
  using TestNode = TestTree::Node;

  // Runs all needed tests.
  static bool verify(const TestTree &tree);

  // Checks that StatTree have binary tree structure.
  struct StructTester
  {
    const size_t treeSize_;
    const bool passToggle_;
    mutable size_t passedNum_ = 0;

    StructTester(const TestTree &tree)
      : treeSize_{tree.size()}, passToggle_{tree.root_ != nullptr && tree.root_->data_.passToggle_}
    {}

    bool operator()(TestNode *) const noexcept;
  };

  // Checks that all sizes are counted correctly.
  struct SizesTester
  {
    mutable bool rootPassed_ = false;
    const size_t treeSize_;

    SizesTester(const TestTree &tree) : treeSize_{tree.size()}
    {}

    bool operator()(const TestNode *node) const noexcept;
  };

// Checks that tree is ordered correctly.
#if 0
  struct SortedOrderTester
  {
    mutable bool sorted_order = false;

    SortedOrderTester(const TestTree &tree)
    {}

    bool operator()(const TestNode *node) const noexcept;
  };
#endif

  // Checks that RB-tree invariants are met.
  struct ColorsTester
  {
    TestNode *root_;
    mutable std::unordered_map<TestNode *, size_t> blackHeights_{};

    ColorsTester(const TestTree &tree) : root_{tree.root_}
    {}

    bool operator()(TestNode *node) const;
  };
};

inline std::ostream &operator<<(std::ostream &os, const TreeTester::TestData &data)
{
  os << data.value_;
  return os;
}

} // namespace tree

#endif // #ifndef TREE_TESTER_HH_INCL
