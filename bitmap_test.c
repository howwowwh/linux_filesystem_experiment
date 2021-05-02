#include<stdio.h>
#include<stdlib.h>
#include"fs_bitmap.h"
int main()
{
    struct fs_bitmap* test;
    int fnum=0;
    test=fs_bitmap_init(128);
    for(int i = 0;i < 5;i++ ) {
        fnum = fs_bitmap_get_free_num(test);
        printf("fnum = %3d ", fnum);
        if(NOK == fs_bitmap_set_bit(test, fnum)) {
            printf("error %d\n",i);
        } else {
            fs_print_bitmap(test);
        }
    }
    
    if(OK == fs_bitmap_set_bit_zero(test,0))
        fs_print_bitmap(test);

    return 0;
}