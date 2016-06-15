#ifndef DISJOINT_SET_HPP
#define DISJOINT_SET_HPP

#if __cplusplus < 201103L
#error This header needs atleast a C++11 compliant compiler.
#endif

#include <cassert>
#include <memory>
#include <unordered_map>

namespace ds {

struct ListNode {
  ListNode(): parent_(this) {}
  int id_ = 0;
  int children_ = 0;
  ListNode* parent_ = nullptr;
  ListNode* next_ = nullptr;
};

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
  size_t sets() const noexcept {
    return sets_;
  }

  void make_set(const ValueType& val) {
    std::unique_ptr<ListNode> tmp(new ListNode());
    tmp->id_ = sets_;
    tracker_.emplace(val, std::move(tmp));
    sets_++;
  }

  void union_set(const ValueType& val1, const ValueType& val2) {
    auto it1 = tracker_.find(val1);
    assert (it1 != tracker_.end());

    auto it2 = tracker_.find(val2);
    assert (it2 != tracker_.end());

    auto node1 = it1->second.get();
    auto node2 = it2->second.get();
    ListNode* master, *child;

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
    sets_--;
  }

  int find_set(const ValueType& val) {
    auto it = tracker_.find(val);
    assert (it != tracker_.end());

    return it->second->parent_->id_;
  }

  void print_sets(std::ostream& os) {
    for (auto& e : tracker_) {
      os << e.first << " = " << e.second->parent_->id_ << "\n";
    }
  }

private:
  std::unordered_map<ValueType, std::unique_ptr<ListNode>> tracker_;
  size_t sets_ = 0;
};

}
#endif
