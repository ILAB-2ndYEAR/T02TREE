
#include <iostream>

#include "tree.hh"

int main()
{
    tree::StatTree<int> tree{};
    int n;
    std::cin >> n;
    for (int i = 0; i < n; ++i)
    {
        int num;
        std::cin >> num;
        tree.insert(num);
    }

    return 0;
}