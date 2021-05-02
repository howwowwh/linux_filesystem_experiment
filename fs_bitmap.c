#include"fs_bitmap.h"
struct fs_bitmap* gBlockBitmap;
struct fs_bitmap* gInodeBitmap;
int fs_bitmap_block_gflag = 0;
struct fs_bitmap* fs_bitmap_init(int count)
{
    int size = 0;
    struct fs_bitmap *res;
    if(count%8 == 0) {
        size = count/8;
    } else {
        size = count/8+1;
    }
    res = (struct fs_bitmap*)malloc(sizeof(struct fs_bitmap));
    memset(res, 0, sizeof(struct fs_bitmap));
    res->total = count;
    res->map = (uint8_t*)malloc(sizeof(char)*size);
    memset(res->map, 0 ,sizeof(char)*size);
    return res;
}

int fs_bitmap_set_bit(struct fs_bitmap* map, int num)
{
    uint8_t mask = 128;
    int remainder = 0;
    int quotient = 0;

    if(num >= map->total) {
        return NOK;
    }
    remainder = num%8;
    quotient = num/8;
    mask >>= remainder;
    if( 1 == (map->map[quotient]&mask)) {
        return NOK;
    }else {
        map->map[quotient] |= mask;
    }
    return OK;
}

int fs_bitmap_set(struct fs_bitmap* map, int* setList, int size)
{
    int i = 0;
    for(i = 0;i < size; i++) {
        if( NOK == fs_bitmap_set_bit(map, setList[i]) ) {
            return NOK;
        }
    }
    return OK;
}

int fs_bitmap_set_bit_zero(struct fs_bitmap* map, int num)
{
    uint8_t mask = 128;
    int remainder = 0;
    int quotient = 0;

    if(num >= map->total) {
        return NOK;
    }
    remainder = num%8;
    quotient = num/8;
    mask >>= remainder;
    if( 0 == (map->map[quotient]&mask)) {
        return OK;
    }else {
        map->map[quotient] &= (~mask);
    }
    return OK;
}

int fs_bitmap_get_free_num(struct fs_bitmap* map)
{
    int i = 0;
    int j = 0;
    uint8_t mask = 128;
    for(i = 0; i < map->total/8 + 1; i++) {
        mask = 128;
        for(j = 0;j < 8; j++) {   
            if(0 == (map->map[i]&mask) ) {
                return i*8+j;
            }
            mask >>= 1;
        }
    }
    return NOK;
}


void fs_print_bitmap(struct fs_bitmap* map)
{
    int i = 0;
    int j = 0;
    int res = -1;
    uint8_t mask = 128;
    for(i =0;i<map->total/8+1;i++) {
        mask = 128;
        for(j = 0; j < 8; j++) {
            res = map->map[i]&mask;
            printf("%d",res>>(8-j-1));
            mask >>= 1;
        }
        printf(" ");
    }
    printf("\n");
}
