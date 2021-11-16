
#include "tree.hh"

#ifndef TREE_IMPL_HH_INCL
#define TREE_IMPL_HH_INCL

namespace tree
{

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
void StatTree<Data, Compare>::right_rotation( Node* x )
{
    Node* y = x->left_;

    x->left_ = y->right_;
    if (y->right_ != nullptr)
    { y->right_->parent_ = x; }

    if (y != nullptr)
    { y->parent_ = x->parent_; }

    if (x->parent_)
    {
        if (x == x->parent_->right_)
        { x->parent_->right_ = y; }
        else
        { x->parent_->left_ = y; }
    }
    else
    { root_ = y; }

    y->right_ = x;
    if (x != nullptr)
    { x->parent_ = y; }
}

template<class Data, class Compare>
void StatTree<Data, Compare>::left_rotation( Node* x )
{
    Node* y = x->left_;
    x->right_ = y->right_;

    if (y->left_ != nullptr)
    { y->right_->parent_ = x; }

    if (y != nullptr)
    { y->parent_ = x->parent_; }

    if (x->parent_)
    {
        if (x == x->parent_->right_)
        { x->parent_->right_ = y; }
        else
        { x->parent_->left_ = y; }
    }
    else
    { root_ = y; }

    y->right_ = x;
    if (x != nullptr)
    { x->parent_ = y; }
}

template<class Data, class Compare>
void StatTree<Data, Compare>::swipe_colors( Node& node )
{
    using type_color = typename tree::StatTree<Data, Compare>::Node::Color;
    node.right_->color_  = type_color::BLACK;
    node.left_->color_   = type_color::BLACK;
    node.parent_->color_  = type_color::RED;
}

template<class Data, class Compare>
void StatTree<Data, Compare>::balance( StatTree& tree, Node& node )
{
    using type_color = typename tree::StatTree<Data, Compare>::Node::Color;

    if (node.right_->color_ == type_color::RED && node.left_->color_ == type_color::BLACK)
    { tree.left_rotation(&node); }

    else if (node.left_->color_ == type_color::RED && node.right_->left_->color_ == type_color::RED )
    { tree.right_rotation(&node); }

    else if (node.left_->color_ == type_color::RED && node.right_->color_ == type_color::RED)
    { tree.swipe_colors(node); }
}

template<class Data, class Compare>
typename tree::StatTree<Data, Compare>::Node tree::StatTree<Data, Compare>::insert( StatTree& tree, const Data& new_data )
{
    Node* current;
    Node* parent;
    Node* x;

    using type_color = typename tree::StatTree<Data, Compare>::Node::Color;

    // find where node belongs
    current = root_;
    parent = 0;
    while (current != nullptr)
    {
        if (new_data == current->data_)
        { return (*current); }
        parent = current;
        current = (new_data < current->data_) ? current->left_ : current->right_;
    }

    x->data_    = new_data;
    x->parent_  = parent;
    x->left_    = nullptr;
    x->right_   = nullptr;
    x->color_   = type_color::RED;

    // insert node in tree
    if(parent)
    {
        if(new_data < parent->data_)
        { parent->left_ = x; }
        else
        { parent->right_ = x; }
    }
    else
    { root_ = x; }

    tree.balance(tree, *x);
    return(*x);
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
