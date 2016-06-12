#include <iostream>
#include <cassert>
#include <sstream>
#include "array_hash.hpp"
#include "array_hash.cpp"

void simple_test()
{
  bool res = false;
  ListMapImpl<const char*, int> hmap;

  std::cout << "Init size = " << hmap.size() << std::endl;
  res = hmap.add("Test-1", 6, 42);
  assert (res == true);

  auto siz = hmap.size();
  std::cout << siz << std::endl;
  assert (siz == 1);

  auto* val = hmap.find("Test-1", 6);
  assert (val != nullptr);
  std::cout << "Value for key(Test-1) = " << *val << std::endl;

  res = hmap.add("Test-1", 6, 43);
  assert (res == true);

  auto new_siz = hmap.size();
  assert (siz == 1);

  val = hmap.find("Test-1", 6);
  std::cout << "Updated value for key(Test-1) = " << *val << std::endl;
  assert (val != nullptr);
  assert (*val == 43);
}

void simple_delete_test()
{
  bool res = false;
  ListMapImpl<const char*, int> hmap;
  res = hmap.add("Test-1", 6, 42);
  assert (res == true);

  res = hmap.remove("Test-1", 6);
  assert (res == true);

  std::cout << "Size after remove = " << hmap.size() << std::endl;
  auto* val = hmap.find("Test-1", 6);
  assert (val == nullptr);

  res = hmap.add("Test-1", 6, 42);
  assert (res == true);

  auto size = hmap.size();
  assert (size == 1);
}

void bulk_add_test()
{
  ListMapImpl<const char*, int> hmap;
  for (int i = 0; i < 5; i++) {
    std::ostringstream oss;
    oss << "key-" << i;
    hmap.add(oss.str().c_str(), oss.str().length(), i);
  }
}

int main() {
  simple_test();
  simple_delete_test();
  bulk_add_test();
  return 0;
}
