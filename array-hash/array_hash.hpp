#ifndef ARRAY_HASH_HPP
#define ARRAY_HASH_HPP

#if __cplusplus < 201103L
  #error This header needs atleast a C++11 compliant compiler.
#endif

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <iterator>
#include <string>
#include <vector>
#include <type_traits>


#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

namespace ds {

namespace detail {

class Buffer
{
public:
  Buffer() = default;
  ~Buffer() = default;
  Buffer(const Buffer&) = delete;
  void operator=(const Buffer&) = delete;

public:
  char* data() noexcept {
    return memory_.get();
  }

  // Returns nullptr on failure
  // users of this class are strictly expected
  // to check
  const char* resize(size_t new_size);
private:
  std::unique_ptr<char[]> memory_ = nullptr;
};

template <typename KeyType, typename ValueType>
class RawMemoryMapImpl: private Buffer
{
public:
  RawMemoryMapImpl() {
    static_assert(std::is_pod<ValueType>::value, 
    	"RawMemoryMapImpl supports only POD value types.");

    static_assert(std::is_pointer<KeyType>::value,
    	"KeyType is expected to be pointer type");
  }

  using Buffer::data;

  // Returns nullptr on failure
  // users of this class are strictly expected
  // to check
  const char* resize(size_t siz) {
    if (siz < size()) return Buffer::data();
    return Buffer::resize(siz);
  }

  bool find(const KeyType key, ValueType& value);

  bool append(const KeyType key, const ValueType& value);

  size_t size() const noexcept {
    auto data = Buffer::data();
    if (unlikely(!data)) return 0;
    // Strong invariant that needs to be maintained
    return *static_cast<uint32_t*>(data);
  }

  void update_size(uint32_t new_size) noexcept {
    auto data = Buffer::data();
    if (unlikely(!data)) return;

    auto& size = *static_cast<uint32_t*>(data);
    size = new_size;
    return;
  }

};

class ListImpl
{
};

}


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

}

#endif
