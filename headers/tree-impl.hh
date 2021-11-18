
#include "tree.hh"

#ifndef TREE_IMPL_HH_INCL
#define TREE_IMPL_HH_INCL

namespace tree
{

template <class Data, class Compare>
typename StatTree<Data, Compare>::Iterator StatTree<Data, Compare>::find(const Data &toFind) const
{
    Node *curNode = root_;

    while (curNode != nullptr)
    {
        const Data &curData = curNode->data_;

        if (toFind > curData)
            curNode = curNode->right_;
        else if (toFind < curData)
            curNode = curNode->left_;
        else
            break;
    }

    if (curNode == nullptr)
        return end();

    return Iterator(curNode);
}

template <class Data, class Compare> void StatTree<Data, Compare>::transplant(Node *old, Node *replacing)
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

// Rotate a node x to the right
//        x                y
//       / \              / \
//      y   c    --->    a   x
//     / \                  / \
//    a   b                b   c
template <class Data, class Compare> void StatTree<Data, Compare>::rRotation(Node *x)
{
    Node* y = x->left_;
    assert(y != 0);

    Node* parent = x->parent_;
    y->parent_ = parent;

    if (x == parent->left_)
    {
        // x is the left son of its parent
        parent->left_ = y;
    }
    else
    {
        // x is the right son of its parent
        parent->right_ = y;
    }

    x->left_ = y->right_;

    if (y->right_ != 0)
        y->right_->parent_ = x;
    y->right_ = x;
    x->parent_ = y;
}

// Rotate a node x to the left
//        x                y
//       / \              / \
//      a   y    --->    x   c
//         / \          / \
//        b   c        a   b
template <class Data, class Compare> void StatTree<Data, Compare>::lRotation(Node *x)
{
    Node* y = x->right_;
    assert(y != 0);

    Node* parent = x->parent_;
    y->parent_ = parent;
    if (x == parent->left_) {
        // x is the left son of its parent
        parent->left_ = y;
    } else {
        // x is the right son of its parent
        parent->right_ = y;
    }
    x->right_ = y->left_;
    if (y->left_ != 0)
        y->left_->parent_ = x;
    y->left_ = x;
    x->parent_ = y;
}

template <class Data, class Compare> void StatTree<Data, Compare>::erase(Iterator delIt)
{
    if (delIt == end())
        return;

    Node *del = delIt.ptr_;

    // Stuff for eraseFixup.
    typename Node::Color toCheck = del->color;
    Node *toFix = nullptr;
    Node *toFixParent = del;

    if (del->left_ == nullptr)
    {
        toFix = del->right_;
        transplant(del, toFix);
    }
    else if (del->right_ == nullptr)
    {
        toFix = del->left_;
        transplant(del, toFix);
    }
    else
    {
        // Next node after del.
        Node *next = del->right_;
        while (next->left_ != nullptr)
            next = next->left_;
        toCheck = next->color_;

        toFixParent = next;
        toFix = next->right_;

        if (next->parent_ != del)
        {
            toFixParent = next->parent_;
            transplant(next, next->right_);
            next->right_ = del->right_;
            next->right_->parent_ = next;
        }

        transplant(del, next);
        next->left_ = del->left_;
        next->left_->parent_ = next;
        next->color_ = del->color_;
    }

    if (toCheck == Node::Color::BLACK)
        eraseFixup(toFix, toFixParent);

    delete del;
}

template <class Data, class Compare> void StatTree<Data, Compare>::eraseFixup(Node *toFix, Node *toFixParent)
{
    while (toFix != root_ && (toFix == nullptr || toFix->color_ == Node::Color::BLACK))
    {
        if (toFix == toFixParent->left_)
        {
            // Not nil.
            Node *brother = toFixParent->right_;
            if (brother->color_ == Node::Color::RED)
            {
                brother->color_ = Node::Color::BLACK;
                toFixParent->color_ = Node::Color::RED;
                lRotation(toFixParent);
                brother = toFixParent->right_;
            }
            if (brother->left_->color_ == Node::Color::BLACK && brother->right_->color_ == Node::Color::BLACK)
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
                    rRotation(brother);
                    brother = toFixParent->right_;
                }

                brother->color_ = toFixParent->color_;
                toFixParent->color_ = Node::Color::BLACK;
                brother->right_->color_ = Node::Color::BLACK;
                lRotation(toFixParent);
                toFix = root_;
            }
        }
        else
        {
        }
    }

    toFix->color_ = Node::Color::BLACK;
}

template <class Data, class Compare> void StatTree<Data, Compare>::insertFixup(Node *x)
{
    using type_color = typename tree::StatTree<Data, Compare>::Node::Color;

    while (x != root_ && x->parent_->color_ == type_color::RED)
    {
        if (x->parent_ == x->parent_->parent_->left_)
        {
            Node *y = x->parent_->parent_->right_;
            if (y->color_ == type_color::RED)
            {
                x->parent_->color_ = type_color::BLACK;
                y->color_ = type_color::BLACK;
                x->parent_->parent_->color_ = type_color::RED;
                x = x->parent_->parent_;
            }
            else
            {
                if (x == x->parent_->right_)
                {
                    x = x->parent_;
                    rRotation(x);
                }

                x->parent_->color_ = type_color::BLACK;
                x->parent_->parent_->color_ = type_color::RED;
                rRotation(x->parent_->parent_);
            }
        }
        else
        {
            Node* y = x->parent_->parent_->left_;
            if (y->color_ == type_color::RED)
            {
                x->parent_->color_ = type_color::BLACK;
                y->color_ = type_color::BLACK;
                x->parent_->parent_->color_ = type_color::RED;
                x = x->parent_->parent_;
            }
            else
            {
                if (x == x->parent_->left_)
                {
                    x = x->parent_;
                    rRotation(x);
                }
                x->parent_->color_ = type_color::BLACK;
                x->parent_->parent_->color_ = type_color::RED;
                lRotation(x->parent_->parent_);
            }
        }
    }
    root_->color_ = type_color::BLACK;
}

template <class Data, class Compare>
typename tree::StatTree<Data, Compare>::Node tree::StatTree<Data, Compare>::insert(const Data &new_data)
{
    Node *current = root_;
    Node *parent = 0;
    Node *x = 0;

    using type_color = typename tree::StatTree<Data, Compare>::Node::Color;

    // find where node belongs
    while (current != nullptr)
    {
        if (new_data == current->data_)
        {
            return (*current);
        }
        parent = current;
        current = (new_data < current->data_) ? current->left_ : current->right_;
    }

    x->data_ = new_data;
    x->parent_ = parent;

    // insert node in tree
    if (parent)
    {
        if (new_data < parent->data_)
        {
            parent->left_ = x;
        }
        else
        {
            parent->right_ = x;
        }
    }
    else
    {
        root_ = x;
    }

    insertFixup(x);
    return *x;
}

template <class Data, class Compare> bool StatTree<Data, Compare>::verify() const
{
    if (!DFS<StatTree<Data, Compare>::StructTester>())
        return false;
    if (!DFS<StatTree<Data, Compare>::SizesTester>())
        return false;

    return true;
}

template <class Data, class Compare> template <class Tester> bool StatTree<Data, Compare>::DFS() const
{
    Tester tester{*this};

    Node *curNode = root_;
    std::vector<bool> rightChildPassed(size_, false);
    size_t depth = 0;

    for (; curNode != nullptr;)
    {
        if (!tester(curNode))
            return false;

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

template <class Data, class Compare> bool StatTree<Data, Compare>::StructTester::operator()(Node *node) noexcept
{
    if (node->passToggle(passToggle_))
        return false;

    Node *left = node->left_;
    Node *right = node->right_;

    if (left != nullptr)
        if (left->parent_ != node)
            return false;

    if (right != nullptr)
        if (right->parent_ != node)
            return false;

    return ++passedNum_ <= treeSize_;
}

template <class Data, class Compare> bool StatTree<Data, Compare>::SizesTester::operator()(const Node *node) noexcept
{
    const Node *l = node->left_;
    const Node *r = node->right_;
    size_t rSz = node->rightSize_;
    size_t lSz = node->leftSize_;

    if (!rootPassed_)
    {
        rootPassed_ = true;
        if (treeSize_ != lSz + rSz + 1)
            return false;
    }

    if (l == nullptr)
    {
        if (lSz != 0)
            return false;
    }
    else
    {
        if (lSz != l->rightSize_ + l->leftSize_ + 1)
            return false;
    }

    if (r == nullptr)
    {
        if (rSz != 0)
            return false;
    }
    else
    {
        if (rSz != r->rightSize_ + r->leftSize_ + 1)
            return false;
    }

    return true;
}

} // namespace tree

#endif
