#include <iostream>
#include "disjoint_set.hpp"
#include <string>

using namespace ds;

void test_1()
{
  DisjointSet<int> djs;
  for (int i = 0; i < 10 ; i++) {
    djs.make_set(i);
  }

  assert (djs.sets() == 10);

  djs.union_set(1,9);
  djs.union_set(2,8);
  djs.union_set(3,7);

  assert (djs.sets() == 7);

  djs.print_sets(std::cout);
  std::cout << "-------" << std::endl;
  djs.union_set(1,3);
  djs.print_sets(std::cout);

  assert (djs.sets() == 6);

  assert (djs.find_set(1) == djs.find_set(3));
}

void test_2()
{
  DisjointSet<std::string> djs;
  djs.make_set("Arun");
  djs.make_set("Arun-1");
  djs.make_set("Arun-2");

  assert (djs.sets() == 3);
  djs.union_set("Arun-1", "Arun-2");
  assert (djs.find_set("Arun-1") == djs.find_set("Arun-2"));

  djs.print_sets(std::cout);
}

int main() {
  test_1();
  test_2();
  return 0;
}
