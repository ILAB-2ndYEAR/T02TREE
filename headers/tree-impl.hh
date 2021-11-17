
#include "tree.hh"

#ifndef TREE_IMPL_HH_INCL
#define TREE_IMPL_HH_INCL

namespace tree
{

template<class Data, class Compare>
StatTree<Data, Compare>::Iterator StatTree<Data, Compare>::find( const Data& toFind ) const
{
    Node* curNode = root_;

    while (curNode != nullptr)
    {
        const Data& curData = curNode->data_;

        if (toFind > curData)
            curNode = curNode->right_;
        else if (toFind < curData)
            curNode = curNode->left_;
        else break;
    }

    if (curNode == nullptr)
        return end ();

    return Iterator (curNode);
}

template<class Data, class Compare>
void StatTree<Data, Compare>::transplant( Node* old, Node* replacing )
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

template<class Data, class Compare>
void StatTree<Data, Compare>::erase( Iterator delIt )
{
    if (delIt == end ()) return;

    Node* del = delIt.ptr_;

    // Stuff for eraseFixup.
    Node::Color toCheck = del->color;
    Node* toFix = nullptr;
    Node* toFixParent = del;

    if (del->left_ == nullptr)
    {
        toFix = del->right_;
        transplant (del, toFix);
    }
    else if (del->right_ == nullptr)
    {
        toFix = del->left_;
        transplant (del, toFix);
    }
    else
    {
        // Next node after del.
        Node* next = del->right_;
        while (next->left_ != nullptr)
            next = next->left_;
        toCheck = next->color_;

        toFixParent = next;
        toFix = next->right_;

        if (next->parent_ != del)
        {
            toFixParent = next->parent_;
            transplant (next, next->right_);
            next->right_ = del->right_;
            next->right_->parent_ = next;
        }

        transplant (del, next);
        next->left_ = del->left_;
        next->left_->parent_ = next;
        next->color_ = del->color_;
    }

    if (toCheck == Node::Color::BLACK)
        eraseFixup (toFix, toFixParent);

    delete del;
}

template<class Data, class Compare>
void StatTree<Data, Compare>::eraseFixup( Node* toFix, Node* toFixParent )
{
    while (toFix != root_ && (toFix == nullptr || toFix->color_ == Node::Color::BLACK))
    {
        if (toFix == toFixParent->left_)
        {
            // Not nil.
            Node* brother = toFixParent->right_;
            if (brother->color_ == Node::Color::RED)
            {
                brother->color_ = Node::Color::BLACK;
                toFixParent->color_ = Node::Color::RED;
                left_rotation (toFixParent);
                brother = toFixParent->right_;
            }
            if (brother->left_->color_ == Node::Color::BLACK &&
                brother->right_->color_ == Node::Color::BLACK)
            {
                brother->color_ = Node::Color::RED;
                toFix = toFixParent;
                toFixParent = toFix->parent_;
            }
            else
            {
                if (brother->right_->color_ == Node::Color::BLACK)
                {
                    brother->left_->color_ = Node::Color::BLACK;
                    brother->color_ = Node::Color::RED;
                    right_rotation (brother);
                    brother = toFixParent->right_;
                }

                brother->color_ = toFixParent->color_;
                toFixParent->color_ = Node::Color::BLACK;
                brother->right_->color_ = Node::Color::BLACK;
                left_rotation (toFixParent);
                toFix = root_;
            }
        }
        else
        {

        }
    }

    toFix->color_ = Node::Color::BLACK;
}

template<class Data, class Compare>
template<class Tester>
bool StatTree<Data, Compare>::bypass() const
{
    Tester tester {*this};

    Node* curNode = root_;
    std::vector<bool> rightChildPassed (size_, false);
    size_t depth = 0;

    for (; curNode != nullptr;)
    {
        if (!tester (curNode)) return false;

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

template<class Data, class Compare>
bool StatTree<Data, Compare>::StructTester::operator()( Node* node ) noexcept
{
    if (node->passToggle (passToggle_))
        return false;

    Node* left = node->left_;
    Node* right = node->right_;

    if (left != nullptr)
    if (left->parent_ != node || left->side_ != Node::Side::LEFT)
        return false;

    if (right != nullptr)
    if (right->parent_ != node || right->side_ != Node::Side::RIGHT)
        return false;

    return ++passedNum_ <= treeSize_;
}

template<class Data, class Compare>
bool StatTree<Data, Compare>::SizesTester::operator()( const Node* node ) noexcept
{
    const Node* l = node->left_;
    const Node* r = node->right_;
    size_t rSz = node->rightSize_;
    size_t lSz = node->leftSize_;

    if (!rootPassed_)
    {
        rootPassed_ = true;
        if (treeSize_ != lSz + rSz + 1)
            return false;
    }

    if (l == nullptr) { if (lSz != 0) return false; }
    else
    {
        if (lSz != l->rightSize_ + l->leftSize_ + 1)
            return false;
    }

    if (r == nullptr) { if (rSz != 0) return false; }
    else
    {
        if (rSz != r->rightSize_ + r->leftSize_ + 1)
            return false;
    }

    return true;
}
template<class Data, class Compare>
void StatTree<Data, Compare>::right_rotation( const Node& x )
{
    Node *y = x->left_;

    x.left_ = y.right_;
    if (y.right_ != nullptr)
    { y.right_->parent_ = x; }

    if (y != nullptr)
    { y.parent_ = x.parent_; }

    if (x.parent_)
    {
        if (x == x.parent_->right_)
        { x.parent_->right_ = y; }
        else
        { x.parent_->left_ = y; }
    }
    else
    { root_ = y; }

    y.right_ = x;
    if (x != nullptr)
    { x.parent_ = y; }
}

template<class Data, class Compare>
void StatTree<Data, Compare>::left_rotation( const Node& x )
{
    Node *y = x.left_;
    x.left_ = y.right_;

    if (y.right_ != nullptr)
    { y.right_->parent_ = x; }

    if (y != nullptr)
    { y.parent_ = x.parent_; }

    if (x.parent_)
    {
        if (x == x.parent_->right_)
        { x.parent_->right_ = y; }
        else
        { x.parent_->left_ = y; }
    }
    else
    { root_ = y; }

    y.right_ = x;
    if (x != nullptr)
    { x.parent_ = y; }
}

template<class Data, class Compare>
void StatTree<Data, Compare>::swipe_colors( const Node& node )
{
    node.right_->color_ = Color::BLACK;
    node.left_->color_   = Color::BLACK;
    node.parent_->color  = Color::RED;
}

template<class Data, class Compare>
void StatTree<Data, Compare>::balance( const StatTree& tree, const Node& node )
{
    Node right_node     = node.right;
    Node left_node      = node.left;
    Node parent_node    = node.parent_;

    if (right_node.color_ == Color::RED && left_node.color_ == Color::BLACK)
    { tree.left_rotation(node); }

    else if (left_node.color_ == Color::RED && right_node->left_.color_ == Color::RED )
    { tree.right_rotation(node); }

    else if (left_node.color_ == Color::RED && right_node.color_ == Color::RED)
    { tree.swipe_colors(node); }
}

template<class Data, class Compare>
bool StatTree<Data, Compare>::verify() const
{
    if (!bypass<StatTree<Data, Compare>::StructTester> ())
        return false;
    if (!bypass<StatTree<Data, Compare>::SizesTester> ())
        return false;

    return true;
}

} // namespace tree

#endif
