#ifndef __FS_BITMAP_H
#define __FS_BITMAP_H

#include "fs_common.h"

struct fs_bitmap
{
    int total;
    uint8_t* map;
};
extern struct fs_bitmap* gBlockBitmap;
extern struct fs_bitmap* gInodeBitmap;
extern int fs_bitmap_block_gflag;
struct fs_bitmap* fs_bitmap_init(int count);
int fs_bitmap_set_bit(struct fs_bitmap* map, int num);
int fs_bitmap_set(struct fs_bitmap* map, int* setList, int size);
int fs_bitmap_set_bit_zero(struct fs_bitmap* map, int num);
int fs_bitmap_get_free_num(struct fs_bitmap* map);
void fs_print_bitmap(struct fs_bitmap* map);

#endif