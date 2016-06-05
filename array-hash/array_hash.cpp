
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
append(KeyType key, size_t key_len, const ValueType& value)
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
  auto new_siz = old_siz + (key_len < 128 ? 1 : 2) + key_len + sizeof(ValueType);

  // Increase the size of memory buffer to 
  // accomodate one more key value
  res = resize(new_siz);
  if (!res) {
    return false;
  }
  // Update the size at the head of the buffer
  update_size(new_siz);

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
