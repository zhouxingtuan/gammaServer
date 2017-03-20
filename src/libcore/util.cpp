//
// Created by IntelliJ IDEA.
// User: AppleTree
// Date: 16/7/23
// Time: 下午2:02
// To change this template use File | Settings | File Templates.
//

#include "util.h"

NS_HIVE_BEGIN
// 32-bit hash
uint32_t binary_murmur_hash32( const void * key, int len, uint32_t seed )
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value

    uint32_t h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4)
    {
        //每次循环都将4个字节的 字符 转成一个int类型
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array
    //处理结尾不足4个字节的数据，通过移位操作将其转换为一个int型数据
    switch(len)
    {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
            h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

// 64-bit hash for 64-bit platforms
uint64_t binary_murmur_hash64A ( const void * key, int len, unsigned int seed )
{
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    const uint64_t * data = (const uint64_t *)key;
    const uint64_t * end = data + (len/8);

    while(data != end)
    {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char * data2 = (const unsigned char*)data;

    switch(len & 7)
    {
        case 7: h ^= uint64_t(data2[6]) << 48;
        case 6: h ^= uint64_t(data2[5]) << 40;
        case 5: h ^= uint64_t(data2[4]) << 32;
        case 4: h ^= uint64_t(data2[3]) << 24;
        case 3: h ^= uint64_t(data2[2]) << 16;
        case 2: h ^= uint64_t(data2[1]) << 8;
        case 1: h ^= uint64_t(data2[0]);
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

// 64-bit hash for 32-bit platforms
uint64_t binary_murmur_hash64B ( const void * key, int len, unsigned int seed )
{
    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    unsigned int h1 = seed ^ len;
    unsigned int h2 = 0;

    const unsigned int * data = (const unsigned int *)key;

    while(len >= 8)
    {
        unsigned int k1 = *data++;
        k1 *= m; k1 ^= k1 >> r; k1 *= m;
        h1 *= m; h1 ^= k1;
        len -= 4;

        unsigned int k2 = *data++;
        k2 *= m; k2 ^= k2 >> r; k2 *= m;
        h2 *= m; h2 ^= k2;
        len -= 4;
    }

    if(len >= 4)
    {
        unsigned int k1 = *data++;
        k1 *= m; k1 ^= k1 >> r; k1 *= m;
        h1 *= m; h1 ^= k1;
        len -= 4;
    }

    switch(len)
    {
        case 3: h2 ^= ((unsigned char*)data)[2] << 16;
        case 2: h2 ^= ((unsigned char*)data)[1] << 8;
        case 1: h2 ^= ((unsigned char*)data)[0];
            h2 *= m;
    };

    h1 ^= h2 >> 18; h1 *= m;
    h2 ^= h1 >> 22; h2 *= m;
    h1 ^= h2 >> 17; h1 *= m;
    h2 ^= h1 >> 19; h2 *= m;

    uint64_t h = h1;

    h = (h << 32) | h2;

    return h;
}
uint32_t binary_djb_hash(const char* cstr, unsigned int length){
    char* str = const_cast<char*>(cstr);
    uint32_t hash = 5381;
    while (length){
        hash += (hash << 5) + (*str++);
        --length;
    }
    return (hash & 0x7FFFFFFF);
}
// tiny加密
void tiny_encrypt(uint32_t* v, uint32_t* k, uint32_t round){
	uint32_t v0=v[0], v1=v[1], sum=0, i;           /* set up */
	uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
	uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
	for (i=0; i < round; i++) {                       /* basic cycle start */
		sum += delta;
		v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
		v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
	}                                              /* end cycle */
	v[0]=v0; v[1]=v1;
}
// tiny解密
void tiny_decrypt(uint32_t* v, uint32_t* k, uint32_t round, uint32_t move){
	uint32_t v0=v[0], v1=v[1], sum/*=0xC6EF3720*/, i;  /* set up */
	uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
	sum = delta << move;
	uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
	for (i=0; i<round; i++) {                         /* basic cycle start */
		v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
		v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
		sum -= delta;
	}                                              /* end cycle */
	v[0]=v0; v[1]=v1;
}

// 如果当前要加密的长度大于8，那么多出来的字节不再加密；如果长度小于8，不进行加密操作
// 快速加密
void binary_encrypt_quick(char* ptr, unsigned int length, const char* key){
    char* stop_ptr = ptr + (length-length%8);
    uint32_t* u_key = (uint32_t*)key;
    while(ptr < stop_ptr){
		tiny_encrypt( (uint32_t*)ptr, u_key, 16 );
    	ptr += 8;
    };
}
void binary_decrypt_quick(char* ptr, unsigned int length, const char* key){
    char* stop_ptr = ptr + (length-length%8);
    uint32_t* u_key = (uint32_t*)key;
    while(ptr < stop_ptr){
		tiny_decrypt( (uint32_t*)ptr, u_key, 16, 4 );
    	ptr += 8;
    };
}
// 普通加密，速度和加密程度居中
void binary_encrypt(char* ptr, unsigned int length, const char* key){
    char* stop_ptr = ptr + (length-length%8);
    uint32_t* u_key = (uint32_t*)key;
    while(ptr < stop_ptr){
		tiny_encrypt( (uint32_t*)ptr, u_key, 32 );
    	ptr += 8;
    };
}
void binary_decrypt(char* ptr, unsigned int length, const char* key){
    char* stop_ptr = ptr + (length-length%8);
    uint32_t* u_key = (uint32_t*)key;
    while(ptr < stop_ptr){
		tiny_decrypt( (uint32_t*)ptr, u_key, 32, 5 );
    	ptr += 8;
    };
}
// 强度较高的加密
void binary_encrypt_hard(char* ptr, unsigned int length, const char* key){
    char* stop_ptr = ptr + (length-length%8);
    uint32_t* u_key = (uint32_t*)key;
    while(ptr < stop_ptr){
		tiny_encrypt( (uint32_t*)ptr, u_key, 64 );
    	ptr += 8;
    };
}
void binary_decrypt_hard(char* ptr, unsigned int length, const char* key){
    char* stop_ptr = ptr + (length-length%8);
    uint32_t* u_key = (uint32_t*)key;
    while(ptr < stop_ptr){
		tiny_decrypt( (uint32_t*)ptr, u_key, 64, 6 );
    	ptr += 8;
    };
}

NS_HIVE_END
