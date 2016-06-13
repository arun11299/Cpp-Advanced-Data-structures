#include <iostream>
#include <cassert>
#include <sstream>
#include <unordered_map>
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

void test_blob_iterator_simple()
{
  //std::cout << "Starting test_iterator_simple =====" << std::endl;

  ArrayHashBlob<int> hmap;
  std::string key;
  key.reserve(16);
  for (int i = 0; i < 1000000; i++) {
    key = "key-" + std::to_string(i);
    hmap.add(key, i);
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
  //std::cout << found << std::endl;
  //assert (found == 1000);

  //std::cout << "===== Finished test_iterator_simple" << std::endl;
}

void test_list_iterator_simple()
{
  //std::cout << "Starting test_list_iterator_simple =====" << std::endl;

  ArrayHashList<int> hmap;
  std::string key;
  key.reserve(16);
  for (int i = 0; i < 1000000; i++) {
    key = "key-" + std::to_string(i);
    hmap.add(key, i);
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
  //std::cout << found << std::endl;
  //assert (found == 1000);

  //std::cout << "===== Finished test_list_iterator_simple" << std::endl;
}

void test_add_and_find_raw()
{
  ArrayHashBlob<int> hmap;
  std::string key; key.reserve(16);

  for (int i = 0; i < 1000000; i++) {
    key = "key-" + std::to_string(i);
    hmap.add(key, i);
  }

  for (int i = 0; i < 1000000; i++) {
    key = "key-" + std::to_string(i);
    assert (hmap.find(key));
  }
}

void test_add_and_find_list()
{
  ArrayHashList<int> hmap;
  std::string key; key.reserve(16);
  
  for (int i = 0; i < 1000000; i++) {
    key = "key-" + std::to_string(i);
    hmap.add(key, i);
  }
  
  for (int i = 0; i < 1000000; i++) {
    key = "key-" + std::to_string(i);
    hmap.find(key);
  }
}

void test_add_and_find_map()
{
  std::unordered_map<std::string, int> hmap(1056323);
  std::string key; key.reserve(16);

  for (int i = 0; i < 1000000; i++) {
    key = "key-" + std::to_string(i);
    hmap.insert({key, i});
  }
  for (int i = 0; i < 1000000; i++) {
    key = "key-" + std::to_string(i);
    hmap.find(key);
  }
}


int main() {
  //test_simple_blob();
  //test_blob_iterator_simple();
  //test_list_iterator_simple();
  test_add_and_find_raw();
  //test_add_and_find_list();
  //test_add_and_find_map();
  return 0;
}
