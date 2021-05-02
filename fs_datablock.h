#ifndef __FS_DATABLOCK_H
#define __FS_DATABLOCK_H
#include "fs_common.h"
#include "fs_bitmap.h"

typedef enum
{
    NDIR = 0,
    IND,
    DIND,
    TIND
} block_type;

int fs_block_table[FS_MAX_BLOCKS];
void fs_delete_datablock_IND(int block_num);
void fs_datablock_init(void);
int fs_read_datablock(uint8_t *buf, int block_num);
int fs_read_datablock_NDIR(uint8_t *buf, int block_num);
int fs_write_datablock(uint8_t *buf, int block_num, int count);
int fs_write_datablock_NDIR(uint8_t *buf, int block_num, int count);
void fs_delete_datablock_NDIR(int block_num);
int fs_read_index(int *buf, int block_num);
int fs_read_datablock_IND(uint8_t *buf, int block_num);
int fs_write_index(int *buf, int block_num, int count);
int fs_write_datablock_IND(uint8_t *buf, int block_num, int count);
void fs_delete_datablock_IND(int block_num);
void fs_datablock_table_print(int *list);
int fs_write_datablock_DIND(uint8_t *buf, int block_num, int count);
int fs_read_datablock_DIND(uint8_t *buf, int block_num);
void fs_delete_datablock_DIND(int block_num);
void fs_delete_datablock_TIND(int block_num);
int fs_write_datablock_TIND(uint8_t *buf, int block_num, int count);
int fs_read_datablock_TIND(uint8_t *buf, int block_num);
#endif