
#include <iostream>

#include "tree.hh"

int main()
{
  tree::StatTree<int> tree{};
  int n = 0;
  std::cin >> n;
  for (int i = 0; i < n; ++i)
  {
    int num = 0;
    std::cin >> num;
    tree.insert(num);
  }

  tree.dump();
  
  return 0;
}