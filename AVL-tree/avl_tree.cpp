#include "avl_tree.hpp"
using namespace ds;

template <typename K, typename V, typename Comp>
bool AVLTree_Base<K, V, Comp>::insert(const value_type& value)
{
  if (head_ == nullptr) {
    head_ = new NodeType(value);
    return true;
  }
  auto tnode = do_insert(head_, value);
  if (!tnode) {
    return false;
  }
  if (tnode != head_) head_ = tnode;
  return true;
}

template <typename K, typename V, typename Comp>
auto AVLTree_Base<K, V, Comp>::
do_insert(NodeTypePtr node, const value_type& value)
  -> NodeTypePtr 
{
  assert (node);
  if (node->key() == value.first) return nullptr;
  NodeTypePtr ret_node = nullptr;

  if (compare_(value.second, node->key())) {
    if (!node->left()) {
      node->left(new NodeType(value));
      node->adjust_height();
      return node;
    }
    ret_node = do_insert(node->left(), value);
    if (ret_node != node->left()) node->left(ret_node);
  } else {
    if (!node->right()) {
      node->right(new NodeType(value));
      node->adjust_height();
      return node;
    }
    ret_node = do_insert(node->right(), value);
    if (ret_node != node->right()) node->right(ret_node);
  }
  // Adjust the height
  node->adjust_height();

  if (ret_node) {
    auto ht = node->subtree_heights();
    if (ht.first > ht.second && (ht.first - ht.second) > 1) {
      return do_left_rotate(node);
    } else if (ht.second > ht.first && (ht.second - ht.first) > 1) {
      return do_right_rotate(node);
    }
  } else {
    return nullptr;
  }
  return node;
}

template <typename K, typename V, typename Comp>
auto AVLTree_Base<K, V, Comp>::do_left_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto left_child = node->left();
  assert (left_child);
  auto ht = left_child->subtree_heights();
  if (ht.first > ht.second) {
    return do_left_left_rotate(node);
  } else {
    return do_left_right_rotate(node);
  }
  assert (0);
  return node;
}

template <typename K, typename V, typename Comp>
auto AVLTree_Base<K, V, Comp>::do_right_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto right_child = node->right();
  assert (right_child);
  auto ht = right_child->subtree_heights();
  if (ht.second > ht.first) {
    return do_right_right_rotate(node);
  } else {
    return do_right_left_rotate(node);
  }

  assert (0);
  return node;
}

template <typename K, typename V, typename Comp>
auto AVLTree_Base<K, V, Comp>::do_left_left_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto left_child = node->left();
  assert (left_child);

  // Make `node` right child of left_child
  node->left(left_child->right());
  left_child->right(node);

  // Adjust heights
  node->adjust_height();
  left_child->adjust_height();

  return left_child;
}

template <typename K, typename V, typename Comp>
auto AVLTree_Base<K, V, Comp>::do_right_right_rotate(NodeTypePtr node)
   -> NodeTypePtr
{
  assert (node);
  auto right_child = node->right();
  assert (right_child);

  //Make node left child of right_child
  node->as_leaf();
  percolate_down_left(right_child, node);
  right_child->adjust_height();

  return right_child;
}

template <typename K, typename V, typename Comp>
auto AVLTree_Base<K, V, Comp>::do_left_right_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto left_child = node->left();
  assert (left_child);

  //promote right child of the left_child
  auto rl_child = left_child->right();
  assert (rl_child);

  // Make it the leaf node
  left_child->as_leaf();
  percolate_down_left(rl_child, left_child);

  //adjust height
  rl_child->adjust_height();
  node->left(rl_child);
  node->adjust_height();

  return do_left_left_rotate(node);
}

template <typename K, typename V, typename Comp>
auto AVLTree_Base<K, V, Comp>::do_right_left_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto right_child = node->right();
  assert (right_child);

  auto lr_child = right_child->left();
  assert (lr_child);

  right_child->left(lr_child->right());
  lr_child->right(right_child);
  node->right(lr_child);

  right_child->adjust_height();
  lr_child->adjust_height();
  node->adjust_height();

  return do_right_right_rotate(node);
}

template <typename K, typename V, typename Comp>
void AVLTree_Base<K, V, Comp>::
percolate_down_left(NodeTypePtr top, NodeTypePtr leaf)
{
  assert (top && leaf);
  if (!top->left()) {
    top->left(leaf);
    top->adjust_height();
    return;
  }
  percolate_down_left(top->left(), leaf);
  top->adjust_height();
}
