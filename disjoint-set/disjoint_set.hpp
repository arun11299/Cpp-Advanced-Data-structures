#ifndef DISJOINT_SET_HPP
#define DISJOINT_SET_HPP

#if __cplusplus < 201103L
#error This header needs atleast a C++11 compliant compiler.
#endif

#include <algorithm>
#include <unordered_map>

namespace ds {
namespace detail {

  struct ListNode {
    int id_ = 0;
    int children_ = 0;
    ListNode* parent_ = nullptr;
    ListNode* next_ = nullptr;
  };

}

template <typename ValueType>
class DisjointSet
{
public:
  DisjointSet() = default;
  DisjointSet(const DisjointSet&) = delete;
  void operator=(const DisjointSet&) = delete;
  ~DisjointSet() {
  }

public:
  void make_set(ValueType& val) {
    auto tmp = new detail::ListNode;
    if (!tail_) {
      tail_= tmp;
      head_ = tail_;
    } else {
      tail_->next_ = tmp;
      tail_ = tail_->next_;
    }
    tail_->parent_ = tmp;
    tail_->id_ = sets_;
    tracker_.emplace({val, tmp});
    sets_++;
  }

  void union_set(ValueType& val1, ValueType& val2) {
    auto it1 = tracker_.find(val1);
    assert (it1 != tracker_.end());

    auto it2 = tracker_.find(val2);
    assert (it2 != tracker_.end());

    auto node1 = it1->second;
    auto node2 = it2->second;
    ListNode* master, child;

    if (node1->parent_->children_ <= node2->parent_->children_) {
      master = node2->parent_; 
      child = node1->parent_;
    } else {
      master = node1->parent_;
      child = node2->parent_;
    }

    ListNode* second = child;
    while (second) {
      auto nxt = second->next_;
      second->next_ = master->next_;
      master->next_ = second;
      master->children_++;
      second->parent_ = master;
      second = nxt;
    }
  }

  int find_set(ValueType& val) {
    auto it = tracker_.find(val);
    assert (it != tracker_.end());

    return it->second->parent_->id_;
  }

private:
  std::unordered_map<ValueType, ListNode*> tracker_;
  size_t sets_ = 0;
  detail::ListNode* head_ = nullptr;
  detail::ListNode* tail_ = nullptr;
};

}
#endif
