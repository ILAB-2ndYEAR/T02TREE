
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
