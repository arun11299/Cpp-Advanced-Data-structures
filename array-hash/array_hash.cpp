
using namespace ds;
using namespace ds::detail;

static inline size_t offset_pointer_to_key(char*& data_ptr)
{
  size_t siz = 0;
  if (0x01 & *data_ptr) {
    siz = static_cast<size_t>(*((uint16_t*) data_ptr) >> 1);
    data_ptr += sizeof(uint16_t);
  } else {
    siz = static_cast<size_t>(*data_ptr >> 1);
    data_ptr += sizeof(uint8_t);
  }

  return siz;
}

template <typename KeyType, typename ValueType>
RawMemoryMapImpl<KeyType, ValueType>::RawMemoryMapImpl()
{
  static_assert(std::is_pod<ValueType>::value,
       "RawMemoryMapImpl supports only POD value types.");

  static_assert(std::is_pointer<KeyType>::value,
       "KeyType is expected to be pointer type");
}

template <typename KeyType, typename ValueType>
ValueType* 
RawMemoryMapImpl<KeyType, ValueType>::
find(const KeyType key, size_t key_len)
{
  size_t total_len = basic_checks_size(key, key_len);
  if (unlikely(total_len == 0)) return nullptr;

  auto data_ptr = data();
  auto start = data_ptr;

  data_ptr += sizeof(uint32_t); // offset the length
  auto value_siz = sizeof(ValueType);

  while ((size_t)(data_ptr - start) < total_len) {
    auto embd_ksiz = offset_pointer_to_key(data_ptr);

    if (embd_ksiz != key_len) {
      data_ptr += (embd_ksiz + value_siz);
      continue;
    }
    if (memcmp(data_ptr, key, key_len) == 0) {
      data_ptr += embd_ksiz;
      return reinterpret_cast<ValueType*>(data_ptr);
    } else {
      data_ptr += (embd_ksiz + value_siz);
      continue;
    }
  }
  return nullptr;
}

template <typename KeyType, typename ValueType>
bool 
RawMemoryMapImpl<KeyType, ValueType>::
add(KeyType key, size_t key_len, const ValueType& value)
{
  auto* fvalue = find(key, key_len);
  if (fvalue) {
    // Found value, replace it
    *fvalue = value;
    return true;
  }
  // Key does not exist already
  bool res = false;
  auto old_siz = size();
  auto new_siz = (old_siz == 0 ? sizeof(uint32_t) : 0) + // Buffer for holding size information
                 (key_len < 128 ? 1 : 2) +  // Extra byte(s) for storing length encoding
                 key_len +                  // Buffer for holding key
                 sizeof(ValueType);         // Buffer for holding value

  // Increase the size of memory buffer to 
  // accomodate one more key value
  res = resize(new_siz);
  if (!res) {
    return false;
  }
  // Update the size at the head of the buffer
  update_size(new_siz - (old_siz == 0 ? sizeof(uint32_t) : 0));

  auto data_ptr = data() + (old_siz == 0 ? sizeof(uint32_t) : old_siz);

  // Encode length information
  if (key_len < 128) {
    *data_ptr = (key_len << 1);
    data_ptr += sizeof(uint8_t);
  } else {
    *reinterpret_cast<uint16_t*>(data_ptr) = ((uint16_t)key_len << 1) | 0x1;
    data_ptr += sizeof(uint16_t);
  }

  // Copy the key value
  memcpy(data_ptr, key, key_len);
  data_ptr += key_len;

  // Copy the Value
  *reinterpret_cast<ValueType*>(data_ptr) = value;
  return true;
}

template <typename KeyType, typename ValueType>
bool
RawMemoryMapImpl<KeyType, ValueType>::
remove(const KeyType key, size_t key_len)
{
  auto* val = find(key, key_len);
  if (!val) { // Key not present
    return false;
  }
  auto data_ptr = reinterpret_cast<char*>(val);
  auto next_key_ptr = data_ptr + sizeof(ValueType);
  auto curr_size = size();

  auto rem_size = curr_size - (next_key_ptr - (data() + sizeof(uint32_t)));
  // Move data_ptr back to the start of the key
  data_ptr -= key_len + (key_len < 128 ? 1 : 2);
  auto elem_size = next_key_ptr - data_ptr;

  memmove(data_ptr, next_key_ptr, rem_size);
  update_size(curr_size - elem_size);

  return true;
}


//====================================================================================

template <typename KeyType, typename ValueType>
ListMapImpl<KeyType, ValueType>::ListMapImpl()
{
  static_assert(std::is_pod<ValueType>::value,
	      "RawMemoryMapImpl supports only POD value types.");

  static_assert(std::is_pointer<KeyType>::value,
	      "KeyType is expected to be pointer type");
}


template <typename KeyType, typename ValueType>
ValueType*
ListMapImpl<KeyType, ValueType>::
find(const KeyType key, size_t key_len)
{
  if (head_ == nullptr) return nullptr;
  auto iter = head_.get();

  while (iter) {
    if (iter->key_len_ != key_len) {
      iter = (iter->next_).get();
      continue;
    }
    if (memcmp(key, iter->key_, key_len) == 0) {
      return &iter->value_;
    }
    iter = (iter->next_).get();
  }

  return nullptr;
}


template <typename KeyType, typename ValueType>
bool
ListMapImpl<KeyType, ValueType>::
add(const KeyType key, size_t key_len, const ValueType& value)
{
  // Check if already exists
  auto val = find(key, key_len);
  if (val) {
    *val = value;
    return true;
  }

  // Add it to the front of the list
  // Make storage of key cache efficient
  std::unique_ptr<char[]> blob(new char[sizeof(ListNode) + 
			sizeof(typename std::remove_pointer<KeyType>::type)*key_len]);

  auto node = reinterpret_cast<ListNode*>(blob.get());
  auto str_ptr = blob.get() + sizeof(ListNode);
  blob.release();

  memcpy(str_ptr, key, key_len);
  node->key_ = str_ptr;
  node->key_len_ = key_len;
  node->value_ = value;
  node->next_.reset(head_.get());

  head_.release();
  head_.reset(node);

  size_++;

  return true;
}


template <typename KeyType, typename ValueType>
bool 
ListMapImpl<KeyType, ValueType>::
remove(const KeyType key, size_t key_len)
{
  if (head_ == nullptr) return false;
  auto iter = head_.get();
  auto iter_next = (iter->next_).get();

  // Check if head is what we need to remove
  if (iter->compare(key, key_len)) {
    head_.reset(iter->next_.release());
    size_--;
    return true;
  }

  while (iter && iter_next) {
    if (iter_next->compare(key, key_len)) {
      iter->next_.reset(iter_next->next_.release());
      size_--;
      return true;
    }
    iter = iter_next;
    iter_next = iter->next_.get();
  }

  return false;
}
