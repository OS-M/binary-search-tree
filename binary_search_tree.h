//
// Created by Matvey on 16.12.2020.
//

#ifndef BINARY_SEARCH_TREE_H_
#define BINARY_SEARCH_TREE_H_

#include <queue>
#include "vector"

template<typename T>
class BinarySearchTree {
  struct TreeNode;
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;
  using difference_type = std::ptrdiff_t;

  struct ConstIterator : std::iterator<std::bidirectional_iterator_tag, T> {
   public:
    const T& operator*() const {
      return node_->value;
    }
    const T* operator->() const {
      return &node_->value;
    }
    ConstIterator& operator++() {
      if (node_->right) {
        node_ = node_->right;
        while (node_->left) {
          node_ = node_->left;
        }
      } else {
        auto now = node_;
        while (now && !(node_->value < now->value)) {
          now = now->parent;
        }
        node_ = now;
      }
      return *this;
    }
    ConstIterator& operator--() {
      if (!node_) {
        node_ = tree_->root_;
        while (node_ && node_->right) {
          node_ = node_->right;
        }
      } else {
        if (node_->left) {
          node_ = node_->left;
          while (node_->right) {
            node_ = node_->right;
          }
        } else {
          auto now = node_;
          while (now && !(now->value < node_->value)) {
            now = now->parent;
          }
          node_ = now;
        }
      }
      return *this;
    }
    const ConstIterator operator++(int) {
      ConstIterator result = *this;
      ++(*this);
      return result;
    }
    const ConstIterator operator--(int) {
      ConstIterator result = *this;
      --(*this);
      return result;
    }
    friend bool operator==(const ConstIterator& lhs, const ConstIterator& rhs) {
      return lhs.node_ == rhs.node_;
    }
    friend bool operator!=(const ConstIterator& lhs, const ConstIterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    friend class BinarySearchTree<T>;

    explicit ConstIterator(const TreeNode* tree_node,
                           const BinarySearchTree<T>* tree)
        : node_{tree_node}, tree_{tree} {}
    const TreeNode* node_{nullptr};
    const BinarySearchTree<T>* tree_{nullptr};
  };

  BinarySearchTree() = default;
  BinarySearchTree(const std::initializer_list<T>& initializer_list);
  BinarySearchTree(const BinarySearchTree<T>& other);
  BinarySearchTree(BinarySearchTree<T>&& other) noexcept;
  ~BinarySearchTree();

  int size() const;
  bool empty() const;
  bool contains(const T& value) const;
  int count(const T& value) const;
  template<typename U>
  void insert(U&& value);
  template<class... Args>
  void emplace(Args&& ... args);
  std::vector<T> to_vector() const;
  void erase(const T& value);
  void erase(const ConstIterator& iter);
  ConstIterator find(const T& value) const;
  ConstIterator begin() const;
  ConstIterator end() const;
  void clear();

  BinarySearchTree<T>& operator=(const BinarySearchTree<T>& other);
  BinarySearchTree<T>& operator=(BinarySearchTree<T>&& other);
  template<class U>
  friend bool operator==(const BinarySearchTree<U>& rhs,
                         const BinarySearchTree<U>& lhs);
  template<class U>
  friend bool operator!=(const BinarySearchTree<U>& rhs,
                         const BinarySearchTree<U>& lhs);
  template<class U>
  friend void Swap(BinarySearchTree<U>& lhs, BinarySearchTree<U>& rhs);

 private:
  struct TreeNode {
    explicit TreeNode(T&& value_);
    template<class... Args>
    explicit TreeNode(Args ...args);
    T value;
    TreeNode* left{nullptr};
    TreeNode* right{nullptr};
    TreeNode* parent{nullptr};
  };

  template<class U>
  static bool Equal(const U& lhs, const U& rhs);
  static void Bind(TreeNode* parent, TreeNode* child);
  void ClearSubtree(TreeNode* node);
  TreeNode* Detach(BinarySearchTree<T>::TreeNode* node);
  void InsertToSubtree(TreeNode* root, TreeNode* node);
  TreeNode* root_{nullptr};
  size_t size_{0};
};

template<typename T>
BinarySearchTree<T>::TreeNode::TreeNode(T&& value_)
    : value{std::move(value_)} {}
template<typename T>

template<class... Args>
BinarySearchTree<T>::TreeNode::TreeNode(Args... args)
    : value(std::forward<Args>(args)...) {}

template<typename T>
BinarySearchTree<T>::BinarySearchTree(
    const std::initializer_list<T>& initializer_list) : BinarySearchTree() {
  for (const auto& element : initializer_list) {
    insert(element);
  }
}
template<typename T>
BinarySearchTree<T>::BinarySearchTree(const BinarySearchTree<T>& other) :
    BinarySearchTree() {
  if (other.empty()) {
    return;
  }
  std::queue<const TreeNode*> queue;
  insert(other.root_->value);
  queue.push(other.root_);
  while (!queue.empty()) {
    const TreeNode* now = queue.front();
    queue.pop();
    if (now->left) {
      insert(now->left->value);
      queue.push(now->left);
    }
    if (now->right) {
      insert(now->right->value);
      queue.push(now->right);
    }
  }
}
template<typename T>
BinarySearchTree<T>::BinarySearchTree(BinarySearchTree<T>&& other) noexcept
    : BinarySearchTree() {
  if (root_ != other.root_) {
    clear();
    Swap(*this, other);
  }
}
template<typename T>
BinarySearchTree<T>::~BinarySearchTree() {
  clear();
}
template<typename T>
int BinarySearchTree<T>::size() const {
  return size_;
}
template<typename T>
bool BinarySearchTree<T>::empty() const {
  return size_ == 0;
}
template<typename T>
bool BinarySearchTree<T>::contains(const T& value) const {
  return find(value) != end();
}
template<typename T>
int BinarySearchTree<T>::count(const T& value) const {
  BinarySearchTree<T>::ConstIterator it = find(value);
  int result = 0;
  while (it.node_ && !(value < it.node_->value)) {
    ++result;
    ++it;
  }
  return result;
}
template<typename T>
template<typename U>
void BinarySearchTree<T>::insert(U&& value) {
  InsertToSubtree(root_, new TreeNode(std::forward<U>(value)));
  ++size_;
}
template<typename T>
template<class... Args>
void BinarySearchTree<T>::emplace(Args&& ... args) {
  InsertToSubtree(root_, new TreeNode(std::forward<Args>(args)...));
  ++size_;
}
template<typename T>
void BinarySearchTree<T>::erase(const T& value) {
  erase(find(value));
}
template<typename T>
std::vector<T> BinarySearchTree<T>::to_vector() const {
  std::vector<T> result;
  for (const auto& element : *this) {
    result.push_back(element);
  }
  return result;
}
template<typename T>
typename BinarySearchTree<T>::ConstIterator BinarySearchTree<T>::begin() const {
  auto result = root_;
  if (!result) {
    return ConstIterator(nullptr, this);
  }
  while (result->left) {
    result = result->left;
  }
  return ConstIterator(result, this);
}
template<typename T>
typename BinarySearchTree<T>::ConstIterator BinarySearchTree<T>::end() const {
  return ConstIterator(nullptr, this);
}
template<typename T>
BinarySearchTree<T>& BinarySearchTree<T>::operator=(
    const BinarySearchTree<T>& other) {
  *this = std::move(BinarySearchTree(other));
  return *this;
}
template<typename T>
BinarySearchTree<T>& BinarySearchTree<T>::operator=(
    BinarySearchTree<T>&& other) {
  if (root_ != other.root_) {
    clear();
    Swap(*this, other);
  }
  return *this;
}
template<class U>
bool operator==(const BinarySearchTree<U>& lhs,
                const BinarySearchTree<U>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  auto iter = rhs.begin();
  for (const auto& element : lhs) {
    if (!(BinarySearchTree<U>::Equal(*iter, element))) {
      return false;
    }
    ++iter;
  }
  return true;
}
template<class U>
bool operator!=(const BinarySearchTree<U>& lhs,
                const BinarySearchTree<U>& rhs) {
  return !(lhs == rhs);
}
template<class U>
void Swap(BinarySearchTree<U>& lhs, BinarySearchTree<U>& rhs) {
  std::swap(lhs.size_, rhs.size_);
  std::swap(lhs.root_, rhs.root_);
}
template<typename T>
void BinarySearchTree<T>::erase(const BinarySearchTree::ConstIterator& iter) {
  if (iter.node_) {
    delete Detach(const_cast<TreeNode*>(iter.node_));
    --size_;
  }
}
template<typename T>
typename BinarySearchTree<T>::ConstIterator BinarySearchTree<T>::find(
    const T& value) const {
  auto now = root_;
  while (now && !(Equal(now->value, value))) {
    if (value < now->value) {
      now = now->left;
    } else {
      now = now->right;
    }
  }
  if (now && Equal(now->value, value)) {
    return ConstIterator(now, this);
  } else {
    return end();
  }
}
template<typename T>
typename BinarySearchTree<T>::TreeNode* BinarySearchTree<T>::Detach(
    BinarySearchTree<T>::TreeNode* node) {
  if (!node) {
    return nullptr;
  }

  if (node->parent) {
    if (node->parent->left == node) {
      node->parent->left = nullptr;
    } else {
      node->parent->right = nullptr;
    }
  }

  if (!node->left && !node->right) {
    if (node == root_) {
      root_ = nullptr;
    }
    return node;
  }

  auto child = (node->left ? node->left : node->right);
  if (node == root_) {
    root_ = child;
    root_->parent = nullptr;
  } else {
    Bind(node->parent, child);
  }
  if (node->left && node->right) {
    InsertToSubtree(node->left, node->right);
  }

  return node;
}
template<typename T>
void BinarySearchTree<T>::InsertToSubtree(BinarySearchTree::TreeNode* root,
                                          BinarySearchTree::TreeNode* node) {
  if (empty()) {
    root_ = node;
  } else {
    auto now = root;
    auto prev = root;
    while (now) {
      prev = now;
      if (node->value < now->value) {
        now = now->left;
      } else {
        now = now->right;
      }
    }
    Bind(prev, node);
  }
}
template<typename T>
void BinarySearchTree<T>::Bind(BinarySearchTree::TreeNode* parent,
                               BinarySearchTree::TreeNode* child) {
  if (child->value < parent->value) {
    parent->left = child;
  } else {
    parent->right = child;
  }
  child->parent = parent;
}
template<typename T>
template<class U>
bool BinarySearchTree<T>::Equal(const U& lhs, const U& rhs) {
  return !(lhs < rhs) && !(rhs < lhs);
}

template<typename T>
void BinarySearchTree<T>::ClearSubtree(BinarySearchTree::TreeNode* node) {
  if (!node) {
    return;
  }
  ClearSubtree(node->left);
  ClearSubtree(node->right);
  delete node;
}

template<typename T>
void BinarySearchTree<T>::clear() {
  ClearSubtree(root_);
  size_ = 0;
}

#endif  // BINARY_SEARCH_TREE_H_
