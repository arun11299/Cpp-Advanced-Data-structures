#ifndef ARRAY_HASH_HPP
#define ARRAY_HASH_HPP

#if __cplusplus < 201103L
  #error This header needs atleast a C++11 compliant compiler.
#endif

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstddef>
#include <memory>
#include <iterator>
#include <string>
#include <vector>
#include <type_traits>
#include "hash.hpp"


#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

namespace ds {

//FWd decl iterator class - needed for frienship
template <typename> class ArrayHashIterator;

//TODO: should be replaced by string_view
template <typename KeyT>
struct KeyHolder
{
  KeyHolder(KeyT k, size_t l): key_ptr(k)
                             , key_len(l)
  {}
  KeyT key_ptr = nullptr;
  size_t key_len = 0;
};

namespace detail {

/*
 * @class Buffer
 * RAII based class to hold a chunk of raw memory
 * Exposed API's:
 * 1. data() - Returns the start(mutable) of the buffer.
 * 2. resize(new_size) - Extends the memory using realloc
 * Exactly for this reason (realloc), I am not using vector<char>
 */

// Deletor for unique_ptr
struct free_deletor {
  void operator()(void* ptr) {
    if (unlikely(!ptr)) return;
    free(ptr);
  }
};

class Buffer
{
public:
  Buffer() = default;
  ~Buffer() = default;
  Buffer(const Buffer&) = delete;
  void operator=(const Buffer&) = delete;

public:
  char* data() const noexcept {
    return memory_.get();
  }

  // Returns nullptr on failure
  // users of this class are strictly expected
  // to check
  const char* resize(size_t new_size) {
    auto new_buf = static_cast<char*>(realloc(memory_.release(), new_size));
    memory_.reset(new_buf);
    return new_buf;
  }
private:
  std::unique_ptr<char, free_deletor> memory_ = nullptr;
};

//==============================================================================

//TODO: Object ownership for `value` ?For now its assumed to be
// purely on copy semantics

/*
 * @class RawMemoryMapImpl
 * Implements the storage as a contiguous memory layout
 * by mapping Key and value one after the other.
 * TODO: More details please
 *
 * Exposed API's:
 * 1. find()
 * 2. add()
 * 3. remove()
 */

template <typename KeyType, typename ValueType>
class RawMemoryMapImpl: private Buffer
{
public:
  RawMemoryMapImpl();
  RawMemoryMapImpl(const RawMemoryMapImpl& other) = delete;
  void operator=(const RawMemoryMapImpl& other) = delete;

public:
  using key_type = KeyType;
  using value_type = ValueType;

public:

  ValueType* find(const KeyType key, size_t key_len) const;

  bool add(const KeyType key, size_t key_len, const ValueType& value);

  bool remove(const KeyType key, size_t key_len);

  /* Returns the size of the total key-value pairs.
   * The calculated size does not include the size of the 
   * buffer (uint32_t) holding the value of size
   */
  size_t size() const noexcept {
    auto data = Buffer::data();
    return data ? *reinterpret_cast<uint32_t*>(data) : 0;
  }

private:
  /* Returns nullptr on failure
   * users of this class are strictly expected
   * to check
   */
  const char* resize(size_t siz) {
    if (siz < size()) return Buffer::data();
    return Buffer::resize(siz);
  }

  size_t basic_checks_size(const KeyType key, size_t key_len) const noexcept 
  {
    if (unlikely(!key || key_len == 0)) return 0;
    if (!Buffer::data()) return 0;

    size_t total_len = size();
    return total_len;
  }

  void update_size(uint32_t new_size) noexcept {
    auto data = Buffer::data();
    if (unlikely(!data)) return;
    *reinterpret_cast<uint32_t*>(data) = new_size;
  }

private: //For iterator class only
  template <typename U>
  friend class ds::ArrayHashIterator;

  char* first() const noexcept;
  std::pair<KeyHolder<KeyType>, ValueType*> item(char* ptr) const noexcept;
  char* next(char* prev) const noexcept;
};


//==============================================================================

/*
 * @class ListImpl
 */

template <typename KeyType, typename ValueType>
class ListMapImpl
{
public:
  ListMapImpl();
  ListMapImpl(const ListMapImpl&) = delete;
  void operator=(const ListMapImpl&) = delete;

public:
  using key_type = KeyType;
  using value_type = ValueType;

  template <typename U>
  friend class ds::ArrayHashIterator;

public:

  ValueType* find(const KeyType key, size_t key_len) const;

  // Adds new key to the front of the list
  bool add(const KeyType key, size_t key_len, const ValueType& value);

  bool remove(const KeyType key, size_t key_len);

  size_t size() const noexcept { return size_; }

private:
  class ListNode;

  struct ListNodeDeleter {
    void operator()(ListNode* node) {
      delete [] (reinterpret_cast<char*>(node));
    }
  };

  class ListNode
  {
  public:
    using NodeKey = typename std::remove_const<KeyType>::type;

    ListNode(NodeKey k, size_t l, const ValueType& v, 
    	std::unique_ptr<ListNode, ListNodeDeleter> nxt):
      key_(k),
      key_len_(l),
      value_(v),
      next_(std::move(nxt))
    {}

    ListNode(const ListNode&) = delete;
    void operator=(const ListNode&) = delete;
    ~ListNode() = default;

  public:
    bool compare(const KeyType key, size_t klen) {
      return (klen == key_len_) && 
	     (memcmp(key, key_, klen) == 0);
    }

  public:
    // Key is allocated right after the ListNode to improve
    // cache hit. This design results in wierd allocation and deallocation
    // of ListNode
    NodeKey key_ = nullptr;
    size_t key_len_ = 0;
    ValueType value_;
    std::unique_ptr<ListNode, ListNodeDeleter> next_ = nullptr;
  };

  uint32_t size_  = 0;
  std::unique_ptr<ListNode, ListNodeDeleter> head_ = nullptr;

private: //For iterator class only
  char* first() const noexcept;
  std::pair<KeyHolder<KeyType>, ValueType*> item(char* ptr) const noexcept;
  char* next(char* prev) const noexcept;
};

}// END OF NAMESPACE DETAIL

//=================================================================================
using KeyType = const char*;
//=================================================================================

template <typename KVStore>
class ArrayHashIterator
{
public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = std::pair<KeyHolder<typename KVStore::key_type>, 
				      typename KVStore::value_type*>;
  using pointer           = typename std::add_pointer<value_type>::type;
  using reference         = typename std::add_lvalue_reference<value_type>::type;
  using difference_type   = ptrdiff_t; // ?
  using self_type         = ArrayHashIterator<KVStore>;

public:
  ArrayHashIterator(const std::vector<KVStore>& kvs, size_t slot = 0):
    cont_(kvs),
    cont_slot_(slot)
  {
    if (cont_slot_ == cont_.size()) return;

    const KVStore& kv = cont_[cont_slot_];
    impl_pointer_ = kv.first();
    if (!impl_pointer_) {
      impl_pointer_ = find_next_valid_slot();
    }
  }

  value_type operator*() const
  {
    const KVStore& kv = cont_[cont_slot_];
    return impl_pointer_ ?
      kv.item(impl_pointer_) : 
      value_type{KeyHolder<typename KVStore::key_type>(nullptr, 0), nullptr};
  }

  self_type& operator++()
  {
    const KVStore& kv = cont_[cont_slot_];
    impl_pointer_ = kv.next(impl_pointer_);
    if (!impl_pointer_) {
      impl_pointer_ = find_next_valid_slot();
    }
    return *this;
  }

  bool operator==(const self_type& other) const noexcept
  {
    return cont_slot_ == other.cont_slot_ && 
      impl_pointer_ == other.impl_pointer_;
  }

  bool operator!=(const self_type& other) const noexcept
  {
    return !(*this == other);
  }

private:
  char* find_next_valid_slot() noexcept
  {
    while (!impl_pointer_) {
      cont_slot_++;
      if (cont_slot_ == cont_.size()) break;
      auto& kv_store = cont_[cont_slot_];
      impl_pointer_ = kv_store.first();
    }
    return impl_pointer_;
  }

private:
  const std::vector<KVStore>& cont_;
  // Pointer to the underlying storage type `KVStore`
  char* impl_pointer_ = nullptr;
  size_t cont_slot_ = 0;
};


//==================================================================================

template <// Type of Value stored against the Key
	  typename ValueType, 
	  // Hashing used internally
	  typename Hasher = typename hash::MurmurHash3,
	  // Type of implementation used to store key-value
	  typename KVStore = typename detail::RawMemoryMapImpl<KeyType, ValueType>
	  >
class ArrayHash
{
public:
  ArrayHash(size_t initial_capacity): total_slots_(initial_capacity)
				    , hash_slots_(total_slots_)
  {}
  ArrayHash(): total_slots_(initial_capacity_)
            , hash_slots_(total_slots_)
  {}

  ArrayHash(const ArrayHash&) = delete;
  void operator=(const ArrayHash&) = delete;
public:
  using iterator = ArrayHashIterator<KVStore>;
  using const_iterator = const iterator;

  iterator begin() { return iterator(hash_slots_); }
  iterator end()   { return iterator(hash_slots_, total_slots_); }
  const_iterator cbegin();
  const_iterator cend();

public:
  bool add(KeyType key, size_t key_len, const ValueType& value)
  {
    assert (key && key_len);
    auto hloc = Hasher()(key, key_len) % total_slots_;
    auto& kvs = hash_slots_[hloc];
    return kvs.add(key, key_len, value);
  }

  bool add(const std::string& key, const ValueType& value)
  {
    return add(key.c_str(), key.length(), value);
  }

  ValueType* find(KeyType key, size_t key_len) const
  {
    assert (key && key_len);
    auto hloc = Hasher()(key, key_len) % total_slots_;
    auto& kvs = hash_slots_[hloc];
    return kvs.find(key, key_len);
  }

  ValueType* find(const std::string& key) const
  {
    return find(key.c_str(), key.length());
  }

  bool remove(KeyType key, size_t key_len)
  {
    assert (key && key_len);
    auto hloc = Hasher()(key, key_len) % total_slots_;
    auto& kvs = hash_slots_[hloc];
    return kvs.remove(key, key_len);
  }

  bool remove(const std::string& key)
  {
    return remove(key.c_str(), key.length());
  }

private:
  // Constant parameters
  const double load_factor_      = 100000.0;
  const size_t initial_capacity_ = 4096;

  // Runtime parameters
  size_t total_slots_            = 0;
  size_t used_slots_             = 0;

  // Storage Container
  // TODO: Need a configurable allocator
  std::vector<KVStore> hash_slots_;
};


// Useful typedefs for lesser finger smashing.
template <typename ValueT, 
	 typename Hasher = typename hash::MurmurHash3>
using ArrayHashBlob = ArrayHash<ValueT, Hasher, 
                                typename detail::RawMemoryMapImpl<KeyType, ValueT>>;

template<typename ValueT,
	 typename Hasher = typename hash::MurmurHash3>
using ArrayHashList = ArrayHash<ValueT, Hasher,
				typename detail::ListMapImpl<KeyType, ValueT>>;	


}

#endif
