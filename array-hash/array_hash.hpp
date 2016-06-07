#ifndef ARRAY_HASH_HPP
#define ARRAY_HASH_HPP

#if __cplusplus < 201103L
  #error This header needs atleast a C++11 compliant compiler.
#endif

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <memory>
#include <iterator>
#include <string>
#include <vector>
#include <type_traits>


#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

namespace ds {

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

  ValueType* find(const KeyType key, size_t key_len);

  bool add(const KeyType key, size_t key_len, const ValueType& value);

  bool remove(const KeyType key, size_t key_len);

  /* Returns the size of the total key-value pairs.
   * The calculated size does not include the size of the 
   * buffer (uint32_t) holding the value of size
   */
  size_t size() const noexcept {
    auto data = Buffer::data();
    if (unlikely(!data)) return 0;
    return *reinterpret_cast<uint32_t*>(data);
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

  size_t basic_checks_size(const KeyType key, size_t key_len) {
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

  ValueType* find(const KeyType key, size_t key_len);

  // Adds new key to the front of the list
  bool add(const KeyType key, size_t key_len, const ValueType& value);

  bool remove(const KeyType key, size_t key_len);

  size_t size() const noexcept { return size_; }

private:
  struct ListNode;

  struct ListNodeDeleter {
    void operator()(ListNode* node) {
      delete [] (reinterpret_cast<char*>(node));
    }
  };

  struct ListNode {
    using NodeKey = typename std::remove_const<KeyType>::type;

    bool compare(const KeyType key, size_t klen) {
      return (klen == key_len_) && 
	     (memcmp(key, key_, klen) == 0);
    }

    NodeKey key_ = nullptr;
    size_t key_len_ = 0;
    ValueType value_;
    std::unique_ptr<ListNode, ListNodeDeleter> next_ = nullptr;
  };

  uint32_t size_  = 0;
  std::unique_ptr<ListNode, ListNodeDeleter> head_ = nullptr;
};

}

/*
using KeyType = char*;

template <typename ValueType, 
		  // Type of Value stored against the Key
	  typename Hasher = typename hash::Murmur, 
			    // Hashing used internally
	  typename KVStore = typename detail::RawMemoryMapImpl<KeyType, ValueType>, 
			    // Type of implementation used to store key-value
	  >
class ArrayHash
{
public:
  ArrayHash(): ArrayHash(initial_capacity_) {}
  ArrayHash(size_t initial_capacity);

  bool insert(const KeyType key, ValueType& value);
  ValueType& operator[](const KeyType key);

  // TODO:: This interface needs to be removed and
  // replaced with iterator based
  ValueType get(const KeyType key) const;
  ValueType operator[](const ValueType key) const;

private:
  // Constant parameters
  const double load_factor_      = 100000.0;
  const size_t initial_capacity_ = 4096;

  // Runtime parameters
  size_t total_slots_;
  size_t used_slots_             = 0;

  // Hasher
  Hasher hash_;

  // Storage Container
  std::vector<KVStore> hash_slots_;
};

*/

}

#endif
