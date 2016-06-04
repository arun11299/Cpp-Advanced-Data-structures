#ifndef SUFFIX_ARRAY_HPP
#define SUFFIX_ARRAY_HPP

#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>

namespace ds {

using sa_group_vec_t = std::vector<int>;
using sa_inv_vec_t = std::vector<int>;

namespace meta {

  namespace detail {
    template <typename... T> struct param_pack {};

    template <typename F, typename S> struct is_on_of_impl;

    template <typename T>
    struct is_on_of_impl<T, param_pack<>> {
      static const bool value = false;
    };

    template <typename T, typename F, typename... RArgs>
    struct is_on_of_impl<T, param_pack<F, RArgs...>> {
      static const bool value = is_on_of_impl<T, param_pack<RArgs...>>::value;
    };

    template <typename T, typename... RArgs>
    struct is_on_of_impl<T, param_pack<T, RArgs...>> {
      static const bool value = true;
    };
  } // end namespace detail

  template <typename T, typename... ETypes> 
  struct is_one_of : detail::is_on_of_impl<T, detail::param_pack<ETypes...>> {};


  template <typename Tag, typename Iter>
  using is_same = std::is_same<Tag, typename std::iterator_traits<Iter>::iterator_category>;

  template <typename Iter>
  using iter_value_type = typename std::iterator_traits<Iter>::value_type;

} // end meta namespace

//-------------------------------------------
/// Suffix Array Implementation starts here
//-------------------------------------------

template <typename Cont>
void print_container(Cont& c)
{
  for (auto& e : c) {
    std::cout << e << " ";
  }
  std::cout << std::endl;
}

template <typename T, size_t N>
size_t arr_size(T(&arr)[N]) { return N; }

template <typename Iter>
sa_group_vec_t sort_by_first_byte(Iter first, Iter last)
{
  sa_group_vec_t sa(std::distance(first, last));
  int count[256] = {0,};

  std::for_each(first, last, [&count](meta::iter_value_type<Iter> v){ count[v]++; });

  // Store the index of the symbol in count array
  uint64_t sum = 0;
  for (int i = 0; i < arr_size(count); i++) {
    auto tmp = sum;
    sum += count[i];
    count[i] = tmp;
  }

  auto tmp = first;
  while (tmp != last) {
    sa[count[*tmp]] = std::distance(first, tmp);
    count[*tmp]++;
    ++tmp;
  }

  return sa;
}

template <typename Iter>
sa_inv_vec_t init_sa_groups(Iter first, Iter last, sa_group_vec_t& sa)
{
  sa_inv_vec_t inv(std::distance(first, last));
  auto prev_group = inv.size() - 1;
  auto prev_byte = *std::next(first, sa[prev_group]);

  for (int i = sa.size() - 1; i >= 0; i--) {
    auto byte = *std::next(first, sa[i]);
    if (byte < prev_byte) {
      if (prev_group == i + 1) {
      	sa[i+1] = -1;
      }
      prev_byte = byte;
      prev_group = i;

      if (prev_group == 0) {
      	sa[0] = 0;
      }
    }
    inv[sa[i]] = prev_group;
  }

  // Separate out the final suffix to the start of its group.
  // This is necessary to ensure the suffix "a" is before "aba"
  // when using a potentially unstable sort.
  // TODO

  return inv;
}

template <typename Iter, 
	 typename = typename std::enable_if<
		      meta::is_same<std::forward_iterator_tag, Iter>::value       |
		      meta::is_same<std::bidirectional_iterator_tag, Iter>::value |
		      meta::is_same<std::random_access_iterator_tag, Iter>::value
		       >::type,
	 typename = typename std::enable_if<
		      meta::is_one_of<typename std::iterator_traits<Iter>::value_type, uint8_t, char>::value>>
sa_group_vec_t qsufsort(Iter first, Iter last)
{
  sa_group_vec_t sa = sort_by_first_byte(first, last);
  sa_inv_vec_t inv = init_sa_groups(first, last, sa);

  print_container(sa);
  print_container(inv);

  return sa;
}

}

#endif
