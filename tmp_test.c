#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<stdint.h>
#include<sys/stat.h> 
#include<fcntl.h>
#include"fs_common.h"
#include"fs_superblock.h"
#include"fs_datablock.h"
#include"fs_inode.h"
#include"fs_bitmap.h"
#include"fs_rbtree.h"
#include"fs_file.h"
#include"fs_hash.h"
#define G_FILE 1024*1024*100
int main()
{   
    fs_init();
    fs_superblock_init(&sb);
    char* txt = (char*)malloc(G_FILE);
    char* res = (char*)malloc(G_FILE+1);
    for(int i = 0;i< G_FILE;i++) {
        txt[i] = i%26+48; 
    }
    fs_file_create("huge",FS_NORMAL_FILE);
    if(G_FILE != fs_file_write_data("huge", txt, G_FILE))
    {
        printf("error\n");
    }
  /*  if(G_FILE != fs_file_read_data("huge", res))
    {
        printf("error\n");
    }   
    */
    fs_superblock_write(&sb);
    return 0;
}