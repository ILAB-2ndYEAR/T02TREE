
#include "tree.hh"

namespace tree
{

// self - left - right
template<class Data, class Compare>
bool StatTree<Data, Compare>::bypass( Tester tester ) const
{
    Node* curNode = root_;
    if (curNode == nullptr)
        return true;

    std::vector<bool> rightChildPassed (size_, false);
    size_t depth = 0;

    passToggle_ = !passToggle_;
    size_t passedNum = 1;

    for (; !curNode->passToggle (); ++passedNum)
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

        for (curNode = curNode->parent_, depth--; curNode != nullptr;)
        {
            if (!rightChildPassed[depth] && curNode->right_ != nullptr)
            {
                rightChildPassed[depth++] = true;
                curNode = curNode->right_;
                break;
            }

            curNode = curNode->parent_
        }

        // All nodes are passed.
        if (curNode == nullptr) break;
    }

    return passedNum == size_;
}

} // namespace tree