#include <iostream>
#include "disjoint_set.hpp"

using namespace ds;

void test_1()
{
  DisjointSet<int> djs;
  for (int i = 0; i < 10 ; i++) {
    djs.make_set(i);
  }

  std::cout << djs.sets() << std::endl;
  djs.union_set(1,9);
  std::cout << djs.sets() << std::endl;
}

int main() {
  test_1();
  return 0;
}
