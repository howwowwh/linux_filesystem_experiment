#ifndef __FS_SUPERBLOCK_H
#define __FS_SUPERBLOCK_H
#include"fs_common.h"
#include"fs_rbtree.h"
#include"fs_inode.h"
#include"fs_datablock.h"

struct fs_superblock{
    int sb_root_dir; //flag of root dirent
    int sb_inode_num; //used inode count
    int sb_block_table[FS_MAX_BLOCKS];
    uint8_t sb_inode_bitmap[FS_MAX_INODES/8+1];
    uint8_t sb_datablock_bitmap[FS_MAX_BLOCKS/8+1];
    uint64_t fs_all_file[FS_MAX_INODES];
    struct fs_inode sb_inode_table[FS_MAX_INODES];
};

struct fs_superblock sb;
void fs_superblock_init(struct fs_superblock* sb);
int fs_superblock_read(struct fs_superblock* sb);
int fs_superblock_write(struct fs_superblock* sb);
void fs_init();
#endif