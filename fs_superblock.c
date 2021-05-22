#include"fs_common.h"
#include"fs_bitmap.h"
#include"fs_datablock.h"
#include"fs_inode.h"
#include"fs_superblock.h"
#include"fs_file.h"
struct fs_superblock sb; 
void fs_superblock_init(struct fs_superblock* sb)
{

    struct fs_file_entry* file = NULL;
    file = (struct fs_file_entry*)malloc(sizeof(struct fs_file_entry));
    memset(file, 0, sizeof(struct fs_file_entry));
    fs_current_dir->file_list = file;
    if(OK == fs_superblock_read(sb) && (sb->fs_all_file[0] != 0)){
        fs_bitmap_init();
        fs_current_dir_init();
        fs_dirent_root_recreate(fs_current_dir);
        return;
    }
    memset(sb,0,sizeof(struct fs_superblock));
    fs_bitmap_init();
    fs_datablock_init();
    fs_inode_init();
    fs_current_dir_init();
    fs_file_create_root_dir();
    if(NOK == fs_superblock_write(sb)){
        printf("初始化失败\n");
        return;
    }
}
int fs_superblock_read(struct fs_superblock* sb)
{
    char* c_sb = (char*)sb;
    int count = sizeof(struct fs_superblock);
    FILE *fp = NULL;
    char buf[50];
    sprintf(buf, "./FileSystem/superblock");
    fp = fopen(buf, "r");
    if (!fp)
    {
        //printf("block%d not exist\n", block_num);
        return NOK;
    }
    if (count != fread(c_sb, sizeof(uint8_t), count, fp))
    {
        //printf("read block%d error!\n", block_num);
        return NOK;
    }
    fclose(fp);
    return OK;
}

int fs_superblock_write(struct fs_superblock* sb)
{
   char* c_sb = (char*)sb;
    int count = sizeof(struct fs_superblock);
    FILE *fp = NULL;
    char buf[50];
    sprintf(buf, "./FileSystem/superblock");
    fp = fopen(buf, "w");
    if (!fp)
    {
        return NOK;
    }
    if (count != fwrite(c_sb, sizeof(uint8_t), count, fp))
    {
        return NOK;
    }
    fclose(fp);
    return OK;
}
void fs_init()
{
    system("rm -rf ./FileSystem && mkdir FileSystem && cd FileSystem && touch superblock");
    memset(&sb, 0, sizeof(struct fs_superblock));
    fs_bitmap_init();
    fs_datablock_init();
    fs_inode_init();
    if(NOK == fs_superblock_write(&sb)){
        printf("init error\n");
    }
    fs_superblock_init(&sb);
}