
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
    class Iterator
    {
        Node* ptr_ = nullptr;

        Data getData() const;

        static Iterator end()
            { return Iterator {nullptr}; }
    };

    size_t size() const { return size_; }

   ~StatTree();
    StatTree( const StatTree& );
    StatTree operator=( const StatTree& );
    StatTree operator=( StatTree&& sd );
    StatTree( StatTree&& sd ) : size_ (sd.size)
        { std::swap (root_, sd.root_); }

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
    bool bypass( std::function<bool( const Node& )> tester ) const;

    // Callables for bypass
    static checkTreeStruct( const Node& );
    // TODO [TheRedHotHabanero]:
    static checkSortedOrder( const Node& );
    // TODO [TheRedHotHabanero]:
    static checkColors( const Node& );
    static checkSubTreesSizes( const Node& );

    // TODO [TheRedHotHabanero]:
    struct dumpCallable {/* ... use ostream? ...*/};
};
