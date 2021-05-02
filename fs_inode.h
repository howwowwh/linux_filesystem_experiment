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

#define 

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
    uint32_t i_dir_acl;         //directory ACL
};

struct fs_inode* fs_create_inode(void);
int fs_delete_inode(struct fs_inode*);

#endif