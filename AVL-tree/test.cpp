#include "avl_tree.hpp"
#include "avl_tree.cpp"
#include <iostream>

using namespace ds;

void test_simple()
{
  std::cout << "Start test_simple=====" << std::endl;
  AVLTree_Base<int, int> map;
  for (int i = 0; i < 10; i++) {
    map.insert(std::make_pair(i, i));
  }
  auto tmp = map.head();
  auto ht = tmp->subtree_heights();
  std::cout << "Left ht: " << ht.first << std::endl;
  std::cout << "Right ht: " << ht.second << std::endl;
  assert (tmp->height() == 4);
  map.insert(std::make_pair(10, 10));
  tmp = map.head();

  std::cout << "=====End test_simple" << std::endl;
}

int main() {
  test_simple();
  return 0;
}
