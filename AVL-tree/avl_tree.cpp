#include "avl_tree.hpp"
using namespace ds;

template <typename T, typename Comp>
bool AVLTree_Base<T, Comp>::insert(const value_type& value)
{
  if (head_ == nullptr) {
    head_ = new NodeType(value);
    return true;
  }

  return do_insert(head_, value);
}

template <typename T, typename Comp>
auto AVLTree_Base<T, Comp>::
do_insert(NodeTypePtr node, const value_type& value)
  -> NodeTypePtr 
{
  assert (node);
  // Do not insert if the value already exists
  if (node->value() == value) return nullptr;

  NodeTypePtr tmp = nullptr;

  if (compare_(value, node->value())) {
    if (!node->left()) {
      return new NodeType(value);
    }
    tmp = do_insert(node->left(), value);
    if (tmp) node->left(tmp);

  } else {
    if (!node->right()) {
      return new NodeType(value);
    }
    tmp = do_insert(node->right(), value);
    if (tmp) node->right(tmp);
  }

  // Update the node height
  node->height(node->height()  +1);

  if (tmp) {
    uint32_t left = node->left_subtree_ht();
    uint32_t right = node->right_subtree_ht();

    if ((left > right) && ((left - right) > 1)) {
      return do_left_rotate(node);
    } else if ((right > left) && (right - left) > 1) {
      return do_right_rotate(node);
    }
  }

  return tmp;
}

template <typename T, typename Comp>
auto AVLTree_Base<T, Comp>::do_left_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto lchild = node->left();
  if (lchild->left_subtree_ht() >= lchild->right_subtree_ht()) {
    return do_left_left_rotate(node);
  } else {
    return do_left_right_rotate(node);
  }

  assert (0);
  return nullptr;
}

template <typename T, typename Comp>
auto AVLTree_Base<T, Comp>::do_right_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto rchild = node->right();
  if (rchild->left_subtree_ht() >= rchild->right_subtree_ht()) {
    return do_right_left_rotate(node);
  } else {
    return do_right_right_rotate(node);
  }

  assert (0);
  return nullptr;
}

template <typename T, typename Comp>
auto AVLTree_Base<T, Comp>::do_left_left_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto lchild = node->left();
  assert (lchild->right() == nullptr);
  relocate_to_rightmost(lchild, node);
  return lchild;
}

template <typename T, typename Comp>
auto AVLTree_Base<T, Comp>::do_left_right_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto tmp = node->left();
  relocate_to_leftmost(tmp->right(), tmp);
  return do_left_left_rotate(node);
}

template <typename T, typename Comp>
auto AVLTree_Base<T, Comp>::do_right_left_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto tmp = node->right();
  relocate_to_rightmost(tmp->left(), tmp);
  return do_right_right_rotate(node);
}

template <typename T, typename Comp>
auto AVLTree_Base<T, Comp>::do_right_right_rotate(NodeTypePtr node)
  -> NodeTypePtr
{
  assert (node);
  auto rchild = node->right();
  relocate_to_leftmost(rchild, node);
  return rchild;
}

template <typename T, typename Comp>
void AVLTree_Base<T, Comp>::
relocate_to_rightmost(NodeTypePtr from, NodeTypePtr node)
{
  assert (from && node);
  auto tmp = from->right();
  while (tmp) {
    from = tmp;
    tmp = tmp->right();
  }
  from->right(node);
}

template <typename T, typename Comp>
void AVLTree_Base<T, Comp>::
relocate_to_leftmost(NodeTypePtr from, NodeTypePtr node)
{
  assert (from && node);
  auto tmp = from->left();
  while (tmp) {
    from = tmp;
    tmp = tmp->left();
  }
  from->left(node);
}
