#ifndef SUFFIX_ARRAY_HPP
#define SUFFIX_ARRAY_HPP

#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>

namespace ds {

using sa_group_t = std::vector<int>;
using sa_inv_t = std::vector<int>;

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

} // end meta namespace

template <typename Iter, 
	 typename = typename std::enable_if<
		      meta::is_same<std::forward_iterator_tag, Iter>::value |
		      meta::is_same<std::bidirectional_iterator_tag, Iter>::value |
		      meta::is_same<std::random_access_iterator_tag, Iter>::value
		       >::type,
	 typename = typename std::enable_if<
		      meta::is_one_of<typename std::iterator_traits<Iter>::value_type, uint8_t, char>::value>>
sa_group_t qsufsort(Iter first, Iter last)
{
  sa_group_t sa(std::distance(first, last));
  return sa;
}

}

#endif
