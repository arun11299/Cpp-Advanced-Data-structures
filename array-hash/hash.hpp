#ifndef ARRAY_HASH_HASH
#define ARRAY_HASH_HASH

namespace ds {
namespace hash {

#if defined(_MSC_VER)

#define FORCE_INLINE	__forceinline
#include <cstdlib>

#define ROTL32(x,y)	_rotl(x,y)
#define ROTL64(x,y)	_rotl64(x,y)
#define BIG_CONSTANT(x) (x)

#else	// defined(_MSC_VER)

#define	FORCE_INLINE inline __attribute__((always_inline))

inline uint32_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)


class MurmurHash3
{
private:
  FORCE_INLINE uint32_t fmix32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
  }

  //----------

  FORCE_INLINE uint64_t fmix64(uint64_t k) {
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;

    return k;
  }

  //-----------------------------------------------------------------------------
  // Block read - if your platform needs to do endian-swapping or can only
  // handle aligned reads, do the conversion here

  FORCE_INLINE uint32_t getblock32(const uint32_t* p, int i) {
    return p[i];
  }

  FORCE_INLINE uint64_t getblock64(const uint64_t* p, int i) {
    return p[i];
  }

public:
  inline uint32_t operator()(const void * key, int len, uint32_t seed = 0x911C9DC5)
  {
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 4;
    uint32_t h1 = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // body
    const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

    for(int i = -nblocks; i; i++)
    {
      uint32_t k1 = getblock32(blocks,i);
      k1 *= c1;
      k1 = ROTL32(k1,15);
      k1 *= c2;
    
      h1 ^= k1;
      h1 = ROTL32(h1,13); 
      h1 = h1*5+0xe6546b64;
    } 

    // tail
    const uint8_t * tail = (const uint8_t*)(data + nblocks*4);
    uint32_t k1 = 0;

    switch(len & 3)
    {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1: k1 ^= tail[0];
	    k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };

    // finalization
    h1 ^= len;
    h1 = fmix32(h1);

    return h1;
  } 
};


//===================================================================
//
//Fowler-Noll-Vo Hash (FNV1a)

class FNVHash
{
private:
  static const uint32_t Prime = 0x01000193; //   16777619
  static const uint32_t Seed  = 0x811C9DC5; // 2166136261

  FORCE_INLINE uint32_t fnv1a(unsigned char oneByte, uint32_t hash = Seed)
  {
      return (oneByte ^ hash) * Prime;
  }
public:

  inline uint32_t operator()(const void* data, size_t numBytes, uint32_t hash = Seed)
  {
    const unsigned char* ptr = (const unsigned char*)data;
    while (numBytes--)
      hash = fnv1a(*ptr++, hash);
    return hash;
  }

};

} // end hash
} // end ds

#endif
