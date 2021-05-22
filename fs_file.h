#ifndef __FS_DIRENT_H
#define __FS_DIRENT_H
#include "fs_common.h"
#include "fs_rbtree.h"
#include"fs_superblock.h"
#define LAST_DIR 0xb60808d6
#define CURR_DIR 0xefde57d9
#define FS_ROOT_DIR 0

extern struct dir_private_info dpi;
extern struct dir_private_info *fs_current_dir;

struct fs_file
{	
	uint32_t file_len; //file count in current dirent 
	uint32_t file_size;
	uint8_t name_len; /* Name length */
	uint8_t file_type;
	uint64_t prev_hash;
	uint64_t file_list[FS_MAX_INODES];
	uint64_t hash;
	char name[FILE_NAME_LEN]; /* dirent name */
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
int fs_file_create(char* fileName, int type);
void fs_dirent_add_prev(uint64_t pre_hash, uint64_t new_hash);
void fs_file_store(struct fs_file* dir);
struct fs_file* fs_file_read(uint64_t hash);
int fs_dirent_root_recreate(struct dir_private_info* current_dir);
void fs_change_dirent(uint64_t hash);
void fs_list_file(void);
int fs_file_read_data(char* file_name, char* buf);
int fs_file_write_data(char* file_name,char *buf, int count);
void fs_file_print(struct fs_file* file);
void fs_file_create_root_dir(void);
void fs_current_dir_init(void);
void fs_delete_file(uint64_t prehash,uint64_t hash);
void fs_file_delete_pre(uint64_t prehash, uint64_t hash);
//operation
void ls();
void cd(char* dir_name);
void cat(char* file_name);
void delete_file(char* file_name);
void printFile(char* file_name);
void printInode(char* file_name);
void printIND(uint32_t num);
void printDB(uint32_t num);
#endif 