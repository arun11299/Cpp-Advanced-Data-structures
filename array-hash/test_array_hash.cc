#include <iostream>
#include <cassert>
#include <sstream>
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

void test_iterator_simple()
{
  std::cout << "Starting test_iterator_simple =====" << std::endl;

  ArrayHashBlob<int> hmap;
  for (int i = 0; i < 1000; i++) {
    std::ostringstream oss;
    oss << "Key-" << i;
    hmap.add(oss.str(), i);
  }

  auto it = hmap.begin();
  size_t found = 0;
  while (it != hmap.end()) {
    auto kv = *it;
    auto& kh = kv.first;
    assert (kh.key_ptr && kh.key_len);
    found++;
    ++it;
  }
  std::cout << found << std::endl;
  assert (found == 1000);

  std::cout << "===== Finished test_iterator_simple" << std::endl;
}

int main() {
  test_simple_blob();
  test_iterator_simple();
  return 0;
}
