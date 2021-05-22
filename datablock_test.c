#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<stdint.h>
#include<sys/stat.h> 
#include<fcntl.h>
#include"fs_datablock.h"

int main()
{
    
    char test[3025];
    char res[2050] = {0};
    fs_bitmap_init();
    fs_datablock_init();
    int block = fs_bitmap_get_free_num(&gBlockBitmap);
    for(int i=0;i< 2049;i++) {
        test[i] = i%26+48;
    }
    fs_bitmap_set_bit(&gBlockBitmap,block);
    if( 2049 == fs_write_datablock_TIND(test,block, 2049)) {
        printf("OK\n");
    }else {
        printf("error\n");
    }
    if( 2049 == fs_read_datablock_TIND(res,block)) {
        printf("OK\n");
    }else {
        printf("error\n");
    }
    printf("%s\n",res);
    fs_datablock_table_print(sb.sb_block_table);
    fs_delete_datablock_TIND(block);
    fs_datablock_table_print(sb.sb_block_table);
}