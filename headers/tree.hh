
#include <cassert>
#include <functional>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <utility>
#include <fstream>

#ifndef TREE_HH_INCL
#define TREE_HH_INCL

namespace tree
{

// RB Tree that provides stat calc methods with log (n) computational
// complexity.
template <class Data, class Compare = std::less<Data>>
class StatTree
{
  enum class Side
  {
    LEFT,
    RIGHT
  };
  enum class Color
  {
    RED,
    BLACK
  };

  struct Node
  {
    Data data_;

    Node *left_ = nullptr;
    Node *right_ = nullptr;
    Node *parent_ = nullptr;

    Color color_ = Color::RED;
    // Sub trees sizes.
    size_t leftSize_ = 0;
    size_t rightSize_ = 0;

    static Color getColor(const Node *node)
    {
      if (node == nullptr)
        return Color::BLACK;
      return node->color_;
    }

    static Node *getChild(const Node *node, Side side)
    {
      if (side == Side::LEFT)
        return node->left_;
      return node->right_;
    }

    // Debug fields & methods:
    bool passToggle_ = false;

    // Checks if that node were toggled with taken treePassToggle
    // value last time and toggles passToggle_ to do such checks in future.
    // Used in verify passes to avoid looping.
    bool passToggle(bool treePassToggle) noexcept
    {
      bool curToggleVal = passToggle_;
      return curToggleVal == (passToggle_ = treePassToggle);
    }
  };

  Node *root_ = nullptr;
  size_t size_ = 0;

public:
  struct Iterator
  {
    Node *ptr_ = nullptr;

    Data getData() const;

    bool operator==(const Iterator &sd) const noexcept
    {
      return ptr_ == sd.ptr_;
    }
  };

  static Iterator end()
  {
    return Iterator{nullptr};
  }

  size_t size() const noexcept
  {
    return size_;
  }

  StatTree() = default;

  // Will implement later:
  ~StatTree() = default;

  StatTree(const StatTree &) = delete;
  StatTree operator=(const StatTree &) = delete;
  StatTree operator=(StatTree &&sd) = delete;
  StatTree(StatTree &&sd) = delete;

  Iterator find(const Data &) const;
  // TODO [TheRedHotHabanero]:
  Node insert(const Data &new_data);
  void erase(Iterator delIt);

private:
  void transplant(Node *old, Node *replacing);
  void rRotation(Node *node);
  void lRotation(Node *node);
  void rotation(Node *node, Side side)
  {
    if (side == Side::LEFT)
      lRotation(node);
    else
      rRotation(node);
  }

  void swipeColors(Node *node);

  void insertFixup(Node *node);
  void eraseFixup(Node *check, Node *checkReplace);

public:
  // Methods from the KV task
  size_t countLesser(const Data &) const;
  Data lesserOfOrderK(size_t k) const;

  // Calles bypass for all types of checks.
  bool verify() const;
  bool dump() const;

private:
  // Bypasses all tree and calls tester (curNode) for all nodes.
  // Returns 'true' in case of success check (dump) and 'false' otherwise.
  template <class Tester>
  bool DFS() const;

  // Used by StructTester to avoid looping.
  bool toggleValue() const noexcept
  {
    if (root_ != nullptr)
      return root_->passToggle_;
    return false;
  }

  // Callables for bypass
  // Checks that StatTree have binary tree structure.
  struct StructTester
  {
    const size_t treeSize_;
    const bool passToggle_;
    size_t passedNum_ = 0;

    StructTester(const StatTree &tree) : treeSize_{tree.size()}, passToggle_{tree.toggleValue()}
    {}

    bool operator()(Node *) noexcept;
  };

  // Checks that all sizes are counted correctly.
  struct SizesTester
  {
    bool rootPassed_ = false;
    const size_t treeSize_;

    SizesTester(const StatTree &tree) : treeSize_{tree.size()}
    {}

    bool operator()(const Node *node) noexcept;
  };

  // Checks that tree is ordered correctly.
  struct SortedOrderTester
  {
    bool sorted_order = false;

    SortedOrderTester(const StatTree &tree) : sorted_order{tree.bypass()}
    {}

    bool operator()(const Node *node) noexcept;
  };
  // TODO [TheRedHotHabanero]:
  struct ColorsTester
  {
    std::unordered_map<Node *, size_t> blackHeights_{};

    bool operator()(const Node *node);
  };
  // TODO [TheRedHotHabanero]:
  struct Dumper
  {
    unsigned long long int cout_nils;
    Dumper(const StatTree &tree)
    {
      cout_nils = 0;

      std::ofstream out;
      out.open("../tree.txt");
      if (out.is_open())
        out << "digraph DG {" << std::endl;
    };
    ~Dumper()
    {
      std::ofstream out("../tree.txt", std::ios::app);
      if (out.is_open())
        out << "}" << std::endl;
    }
    bool operator()(const Node *node) noexcept;
  };
};

} // namespace tree

#include "tree-impl.hh"

#endif // #ifndef TREE_HH_INCL
