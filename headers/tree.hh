
#include <utility>
#include <functional>
#include <ostream>

// RB Tree that provides statistical collection methods.
template<class Data, class Compare = std::less<Data>>
class StatTree
{
    struct Node
    {
        enum class Color : char
        {
            Red,
            Black
        };

        Data data_ {};

        Node* left_ = nullptr;
        Node* right_ = nullptr;
        Node* parent_ = nullptr;

        Color color_ = Color::Black;
        size_t leftSize_ = 0;
    };

    Node* root_ = nullptr;
    size_t size_ = 0;

public:
    size_t size() const { return size_; }

   ~StatTree();
    StatTree( const StatTree& );
    StatTree operator=( const StatTree& );
    StatTree operator=( StatTree&& sd );
    StatTree( StatTree&& sd ) : size_ (sd.size)
        { std::swap (root_, sd.root_); }

  // Basic methods:
    bool insert( const Data& );
    bool contains( const Data& ) const;
    bool erase( const Data& );

  // Stat methods:
    size_t countLesser( const Data& ) const;
    Data getLesserOfOrderK( size_t k ) const;

  // Verification stuff:
    bool verify() const;

    // Bypasses all tree and performs checks / dumps it.
    // Returns 'true' in case of success check (dump) and 'false' otherwise.
    template<class Callable>
    bool bypass() const;

    // Callables for bypass
    struct checkTreeStruct { bool operator()( const Node& ); };
    struct checkSortedOrder { bool operator()( const Node& ); };
    struct checkColors { bool operator()( const Node& ); };
    struct checkSubTreesSizes { bool operator()( const Node& ); };
    struct dumpCallable {/* ... use ostream? ...*/};
};
