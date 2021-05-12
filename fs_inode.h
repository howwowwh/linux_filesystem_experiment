#ifndef __FS_INODE_H
#define __FS_INODE_H

#include<stdint.h>
#include "fs_common.h"

/*Specital indoes numbers*/
#define FS_BAD_INO  1 /*Bad blocks inode*/
#define FS_ROOT_INO 2 /*Root inode*/
#define FS_BOOT_LOADER_INo 5 /*Boot loader inode*/
#define FS_UNDEL_DIR_INO 6 /*Undelete directory inode*/
#define FS_RESIZE_INO    7 /* Reserved group descriptors inode */
#define FS_JOURNAL_INO   8 /* Journal inode */


/*
 * Constants relative to the data blocks
 */
#define	FS_NDIR_BLOCKS		12
#define	FS_IND_BLOCK			FS_NDIR_BLOCKS
#define	FS_DIND_BLOCK			(FS_IND_BLOCK + 1)
#define	FS_TIND_BLOCK			(FS_DIND_BLOCK + 1)
#define	FS_N_BLOCKS			(FS_TIND_BLOCK + 1)

#define FS_NORMAL_FILE      0
#define FS_INDEX_FILE       1
#define FS_JOURNAL_DATA_FIL 2

struct fs_inode
{
    uint16_t i_mode;            //file mode
    uint16_t i_uid;             //low 16 bits of Owner uid
    uint32_t i_size;            //size in bytes
    time_t i_ctime;            //create
    time_t i_atime;           //access time
    time_t i_mtime;           //modification time
    time_t i_dtime;           //deletion time
    uint16_t i_gid;             //low 16 bit of group id
    uint16_t i_links_count;     //links count
    uint32_t i_blocks;          //blocks count
    uint32_t i_flags;           //file flags
    uint32_t i_block[FS_N_BLOCKS]; //pointers to blocks
    uint32_t i_file_acl;        //file ACL            
    uint32_t i_dir_acl;        //directory ACL
};
extern uint32_t ginode_num;
uint64_t fs_all_dir[FS_MAX_INODES];
struct fs_inode fs_inode_table[FS_MAX_INODES];

void fs_inode_init(void);
void fs_inode_one_init(int num);
int fs_inode_store(struct fs_inode* inode);
int fs_inode_read(struct fs_inode* inode);
int fs_get_free_inode_num(void);
int fs_inode_read_normal_data(uint8_t *buf, struct fs_inode *inode);
int fs_inode_write_normal_data(uint8_t *buf, struct fs_inode *inode, int count);
int fs_inode_read_data(uint8_t *buf, struct fs_inode *inode);
int fs_inode_delete_data(struct fs_inode *inode);
int fs_inode_write_data(uint8_t *buf, struct fs_inode *inode, int count);
void fs_inode_delete_normal_data(struct fs_inode* inode);
int fs_inode_get_num(struct fs_inode *inode);
void fs_inode_show_detail(struct fs_inode* inode);
struct fs_inode* fs_inode_create(void);
void fs_inode_delete(struct fs_inode* inode);
int fs_inode_read_all(void);
#endif