
#include <utility>
#include <functional>
#include <ostream>
#include <iostream>

#ifndef TREE_HH_INCL
#define TREE_HH_INCL

namespace tree
{

// RB Tree that provides stat calc methods with log (n) computational complexity.
template<class Data, class Compare = std::less<Data>>
class StatTree
{
    struct Node
    {
        // This node attachement side.
        enum class Side { LEFT, RIGHT };
        enum class Color { RED, BLACK };

        Data data_ {};

        Node* left_ = nullptr;
        Node* right_ = nullptr;
        Node* parent_ = nullptr;

        Side side_ = Side::LEFT;

        Color color_ = Color::BLACK;
        // Left sub tree size.
        size_t leftSize_ = 0;

        // Debug fields & methods:
        bool passToggle_ = false;
        size_t rightSize_ = 0;

        // Checks if that node were toggled with taken treePassToggle
        // value last time and toggles passToggle_ to do such checks in future.
        // Used in verify passes to avoid looping.
        bool passToggle( bool treePassToggle ) noexcept
        {
            bool curToggleVal = passToggle_;
            return curToggleVal == (passToggle_ = treePassToggle);
        }
    };

    Node* root_ = nullptr;
    size_t size_ = 0;

public:
    struct Iterator
    {
        Node* ptr_ = nullptr;

        Data getData() const;

        bool operator==( const Iterator& sd ) const noexcept
            { return ptr_ == sd.ptr_; }
    };

    static Iterator end()
        { return Iterator {nullptr}; }

    size_t size() const noexcept
        { return size_; }

    StatTree() = default;

    // Will implement later:
   ~StatTree() = default;

    StatTree( const StatTree& ) = delete;
    StatTree operator=( const StatTree& ) = delete;
    StatTree operator=( StatTree&& sd ) = delete;
    StatTree( StatTree&& sd ) = delete;

    // TODO [TheRedHotHabanero]:
    //bool insert(subset_node** sn, int k, subset_node* parent = nullptr)
    void right_rotation( Node* node );
    void left_rotation( Node* node );
    void swipe_colors (Node& node );
    void balance( StatTree& tree, Node& node );
    Node insert( StatTree& tree, const Data& new_data );
    Iterator find( const Data& ) const;

    void erase( Iterator delIt );

private:
    void transplant( Node* old, Node* replacing );
    void eraseFixup( Node* check, Node* checkReplace );

public:
    // Methods from the KV task
    size_t countLesser( const Data& ) const;
    Data lesserOfOrderK( size_t k ) const;

    // Calles bypass for all types of checks.
    bool verify() const;

private:
    // Bypasses all tree and calls tester (curNode) for all nodes.
    // Returns 'true' in case of success check (dump) and 'false' otherwise.
    template<class Tester>
    bool bypass() const;

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

        StructTester( const StatTree& tree ) :
            treeSize_ {tree.size ()}, passToggle_ {tree.toggleValue ()} {}

        bool operator ()( Node* ) noexcept;
    };

    // Checks that all sizes are counted correctly.
    struct SizesTester
    {
        bool rootPassed_ = false;
        const size_t treeSize_;

        SizesTester( const StatTree& tree ) :
            treeSize_ {tree.size ()}
        {}

        bool operator()( const Node* node ) noexcept;
    };

    // Checks that tree is ordered correctly.
    struct SortedOrderTester
    {
        bool sorted_order = false;

        SortedOrderTester( const StatTree& tree ):
            sorted_order { tree.bypass() }
        {}

        bool operator()( const Node* node ) noexcept;
    };
    // TODO [TheRedHotHabanero]:
    struct ColorsTester;
    // TODO [TheRedHotHabanero]:
    struct Dumper;
};

} // namespace tree

#include "tree-impl.hh"

#endif // #ifndef TREE_HH_INCL
