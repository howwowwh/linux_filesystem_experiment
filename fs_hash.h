#ifndef __FS_HASH_H
#define __FS_HASH_H
/* jhash portting from linux kernel */
#include"fs_common.h"

static uint32_t jhash(const void *key, unsigned int length, unsigned int initval);

#define __jhash_mix(a, b, c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8);  \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12); \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5);  \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}
 
#define JHASH_GOLDEN_RATIO    0x9e3779b9
 
static uint32_t jhash(const void *key, unsigned int length, unsigned int initval)
{
    unsigned int a, b, c, len;
    const unsigned char *k = (unsigned char*)key;
    len = length;
    a = b = JHASH_GOLDEN_RATIO;
    c = initval;
 
    while (len >= 12) {
        a += (k[0] +((unsigned int)k[1]<<8) +((unsigned int)k[2]<<16) +((unsigned int)k[3]<<24));
        b += (k[4] +((unsigned int)k[5]<<8) +((unsigned int)k[6]<<16) +((unsigned int)k[7]<<24));
        c += (k[8] +((unsigned int)k[9]<<8) +((unsigned int)k[10]<<16)+((unsigned int)k[11]<<24));
 
        __jhash_mix(a,b,c);
 
        k += 12;
        len -= 12;
    }
 
    c += length;
    switch (len) {
        case 11: c += ((unsigned int)k[10]<<24);
        case 10: c += ((unsigned int)k[9]<<16);
        case 9 : c += ((unsigned int)k[8]<<8);
        case 8 : b += ((unsigned int)k[7]<<24);
        case 7 : b += ((unsigned int)k[6]<<16);
        case 6 : b += ((unsigned int)k[5]<<8);
        case 5 : b += k[4];
        case 4 : a += ((unsigned int)k[3]<<24);
        case 3 : a += ((unsigned int)k[2]<<16);
        case 2 : a += ((unsigned int)k[1]<<8);
        case 1 : a += k[0];
        default: break;
    };
 
    __jhash_mix(a,b,c);
    return c;
}

#endif