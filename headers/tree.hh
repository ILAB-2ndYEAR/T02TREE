
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <utility>

#ifndef TREE_HH_INCL
#define TREE_HH_INCL

namespace tree
{

// RB Tree that provides stat calc methods with log (n) computational
// complexity.
template <class Data, class Compare = std::less<Data>>
class StatTree
{
  friend class TreeTester;

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
  };

  Node *root_ = nullptr;
  size_t size_ = 0;

public:
  class Iterator
  {
    friend StatTree;

    Node *ptr_ = nullptr;
    bool isEnd_ = false;

    Iterator(Node *ptr, bool isEnd) : ptr_{ptr}, isEnd_{isEnd}
    {}

  public:
    const Data &operator*() const noexcept
    {
      return ptr_->data_;
    }

    bool operator==(const Iterator &sd) const noexcept
    {
      if (isEnd_ == sd.isEnd_)
      {
        if (isEnd_)
          return true;
        return ptr_ == sd.ptr_;
      }
      return false;
    }
  };

  Iterator end() const
  {
    Node *end = root_;
    if (end == nullptr)
      return Iterator{nullptr, true};
    while (end->right_ != nullptr)
      end = end->right_;
    return Iterator{end, true};
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
  size_t countLesser(const Node *node, const size_t m) const;
  Data lesserOfOrderK(size_t k) const;

  // Calles bypass for all types of checks.
  bool dump() const;

private:
  // Passes all tree and calls callable (curNode) for all nodes.
  // Returns 'true' in case of success pass and 'false' otherwise.
  template <class Callable>
  bool DFS(const Callable &callable) const;

  struct Dumper
  {
    mutable size_t cout_nils;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    Dumper(const StatTree &tree)
    {
      cout_nils = 0;

      std::ofstream out;
      out.open("tree.txt");
      if (out.is_open())
        out << "digraph DG {" << std::endl;
    };
#pragma GCC diagnostic pop
    ~Dumper()
    {
      std::ofstream out("tree.txt", std::ios::app);
      if (out.is_open())
        out << "}" << std::endl;
    }
    bool operator()(const Node *node) const noexcept;
  };
};

} // namespace tree

#include "tree-impl.hh"

#endif // #ifndef TREE_HH_INCL
