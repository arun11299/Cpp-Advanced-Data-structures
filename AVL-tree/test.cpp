#include "avl_tree.hpp"
#include "avl_tree.cpp"
#include <iostream>

using namespace ds;

void test_simple()
{
  std::cout << "Start test_simple=====" << std::endl;
  AVLTree_Base<int> map;
  map.insert(1);
  std::cout << "=====End test_simple" << std::endl;
}

int main() {
  test_simple();
  return 0;
}
