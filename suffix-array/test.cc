#include "suffix_array.hpp"

using namespace ds;

int main() {
  std::string s("ababab");
  qsufsort(s.begin(), s.end());
  return 0;
}
