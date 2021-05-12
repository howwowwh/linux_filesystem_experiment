#ifndef __FS_DIRENT_H
#define __FS_DIRENT_H
#include "fs_common.h"
#include "fs_rbtree.h"
#define FS_DIRENT_INO 11
#define LAST_DIR 0xb60808d6
#define CURR_DIR 0xefde57d9

extern struct dir_private_info dpi;
extern struct dir_private_info *fs_current_dir;

extern int fs_root_dir;
struct fs_file
{	
	uint32_t file_len; //当前目录下的文件数当为目录时可用
	uint32_t file_size;
	uint8_t name_len; /* Name length */
	uint8_t file_type;
	uint64_t prev_hash;
	uint64_t file_list[FS_MAX_INODES];
	uint64_t hash;
	uint64_t hash2;
	
	char name[FILE_NAME_LEN]; /* File name */
};

struct fs_file_entry {
	uint64_t file_hash;
	struct list_head list;
};

struct dir_private_info
{
	rb_root root;
	uint64_t pre_hash;
	uint64_t hash;
	struct fs_file_entry* file_list;
};
struct fs_file* fs_file_create(char* fileName, int type);
void fs_dirent_add_prev(uint64_t pre_hash, uint64_t new_hash);
void fs_file_store(struct fs_file* dir);
struct fs_file* fs_file_read(uint64_t hash);
int fs_dirent_root_create(struct dir_private_info* current_dir);
void fs_change_dirent(uint64_t hash);
void fs_list_file(void);
int fs_file_read_data(char* file_name, char* buf);
int fs_file_write_data(char* file_name,char *buf, int count);
void fs_file_print(struct fs_file* file);
#endif