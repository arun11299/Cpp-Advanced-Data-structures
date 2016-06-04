#include "suffix_array.hpp"

using namespace ds;

int main() {
  std::string s("abcabc");
  std::cout << s << std::endl;
  auto c = qsufsort(s.begin(), s.end());

  return 0;
}
