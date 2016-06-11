#include <iostream>
#include <cassert>
#include "array_hash.hpp"
#include "array_hash.cpp"

void test_simple_blob()
{
  std::cout << "Starting test_simple_blob =====" << std::endl;
  ArrayHashBlob<int> hmap;
  bool ret = hmap.add("key-1", 1);
  assert (ret == true);

  auto* val = hmap.find("key-1");
  assert (val && *val == 1);

  val = hmap.find("key-2");
  assert (val == nullptr);
  ret = hmap.add("key-2", 2);
  assert (ret);

  ret = hmap.remove("key-1");
  assert (ret);
  val = hmap.find("key-1");
  assert (val == nullptr);

  std::cout << "===== Finished test_simple_blob" << std::endl;
}

int main() {
  test_simple_blob();
  return 0;
}
