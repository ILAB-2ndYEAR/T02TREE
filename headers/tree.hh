
#include <utility>
#include <functional>
#include <ostream>

#ifndef TREE_HH_INCL
#define TREE_HH_INCL

namespace tree
{

// RB Tree that provides statistical collection methods.
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
        size_t leftSubTreeSize_ = 0;

        // Debug field
        static constexpr bool PASS_TOGGLE_INIT_VAL = false;
        bool passToggle_ = PASS_TOGGLE_INIT_VAL;

        // Checks if that node were toggled with taken treePassFlag
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

    // Debug field. Used for nodes toggling.
    bool passToggle_ = Node::PASS_TOGGLE_INIT_VAL;

public:
    class Iterator
    {
        Node* ptr_ = nullptr;

    public:
        Data getData() const;

        bool operator==( const Iterator& sd ) const noexcept
            { return ptr_ == sd.ptr_; }
    };

    static Iterator end()
        { return Iterator {nullptr}; }

    size_t size() const noexcept
        { return size_; }

   ~StatTree();
    StatTree( const StatTree& );
    StatTree operator=( const StatTree& );
    StatTree operator=( StatTree&& sd );
    StatTree( StatTree&& sd ) : size_ (sd.size), root_ (sd.root_)
        { sd.root_ = nullptr; }

    // TODO [TheRedHotHabanero]:
    Iterator insert( const Data& );
    Iterator find( const Data& ) const;
    bool erase( const Data& );

    size_t countLesser( const Data& ) const;
    Data lesserOfOrderK( size_t k ) const;

    bool verify() const;

    // Bypasses all tree and performs checks / dumps it.
    // Returns 'true' in case of success check (dump) and 'false' otherwise.
    // TODO [TheRedHotHabanero]:
    using Tester = std::function<bool( const Node& )>;
    bool bypass( Tester tester ) const;

    // Callables for bypass
    static bool checkTreeStruct( const Node& );
    // TODO [TheRedHotHabanero]:
    static bool checkSortedOrder( const Node& );
    // TODO [TheRedHotHabanero]:
    static bool checkColors( const Node& );
    static bool checkSubTreesSizes( const Node& );

    // TODO [TheRedHotHabanero]:
    struct dumpCallable {/* ... use ostream? ...*/};
};

} // namespace tree

#endif // #ifndef TREE_HH_INCL
