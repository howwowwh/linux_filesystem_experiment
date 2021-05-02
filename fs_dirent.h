#ifndef __FS_DIRENT_H
#define __FS_DIRENT_H
#include"fs_common.h"
#include"fs_rbtree.h"
struct dirent {
    uint32_t inode;
    
}

struct dir_private_info {
	rb_root	root;
	rb_node	*curr_node;
	struct fname	*extra_fname;
	loff_t		last_pos;
	__u32		curr_hash;
	__u32		curr_minor_hash;
	__u32		next_hash;
};
struct dx_hash_info
{
	u32		hash;
	u32		minor_hash;
	int		hash_version;
	u32		*seed;
};
struct ext3_dir_entry_2 {
	__le32	inode;			/* Inode number */
	__le16	rec_len;		/* Directory entry length */
	__u8	name_len;		/* Name length */
	__u8	file_type;
	char	name[EXT3_NAME_LEN];	/* File name */
};
#endif