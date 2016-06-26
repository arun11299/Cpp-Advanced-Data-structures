#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <cassert>
#include <functional>
#include <iterator>
#include <type_traits>

namespace ds {

template <typename T>
class AVLNode {
public:
  AVLNode(const T& value):
    value_(value)
  {}
  AVLNode(const AVLNode&) = delete;
  void operator=(const AVLNode&) = delete;

public:
  const AVLNode* left() const noexcept { return left_; }
  AVLNode* left() noexcept { return left_; }
  void left(AVLNode* node) { left_ = node; }

  const AVLNode* right() const noexcept { return right_; } 
  AVLNode* right() noexcept { return right_; } 
  void right(AVLNode* node) { right_ = node; }

  T& value() noexcept { return value_; }
  const T& value() const noexcept { return value_; }

  void height(uint32_t h) noexcept  { height_ = h; }
  uint32_t height() const noexcept  { return height_; }

  uint32_t left_subtree_ht() const noexcept {
    return (left() ? left()->height() : 0);
  }
  uint32_t right_subtree_ht() const noexcept {
    return (right() ? right()->height() : 0);
  }
  
private:
  T value_;
  uint32_t height_ = 0;
  AVLNode* left_   = nullptr;
  AVLNode* right_  = nullptr;
};

template <typename ValueT, typename Comparator = std::less<ValueT>>
class AVLTree_Base
{
public:
  using value_type = ValueT;
  using NodeType = AVLNode<value_type>;
  using NodeTypePtr = NodeType*;

public:
  AVLTree_Base() = default;
  AVLTree_Base(const AVLTree_Base&) = delete;
  void operator=(const AVLTree_Base&) = delete;

  //TODO: change access specifiers 
public:
  bool insert(const value_type& val);

private:
  NodeTypePtr do_insert(NodeTypePtr node,
		        const value_type& value);

  NodeTypePtr do_right_rotate(NodeTypePtr node);
  NodeTypePtr do_left_rotate(NodeTypePtr node);
  NodeTypePtr do_left_left_rotate(NodeTypePtr node);
  NodeTypePtr do_left_right_rotate(NodeTypePtr node);

  void relocate_to_rightmost(NodeTypePtr from,
			     NodeTypePtr node);
  void relocate_to_leftmost(NodeTypePtr from,
			    NodeTypePtr node);

private:
  NodeTypePtr head_ = nullptr;
  Comparator compare_;
};



}// end namespace ds

#endif
