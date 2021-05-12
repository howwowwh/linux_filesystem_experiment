#include"fs_common.h"
#include"fs_datablock.h"
#include"fs_inode.h"
#include"fs_rbtree.h"
#include"fs_hash.h"
#include"fs_file.h"

int main(void)
{
    struct fs_file* file = NULL;
    struct fs_file_entry list;
    fs_inode_init();
    fs_datablock_init();

    INIT_LIST_HEAD(&list.list);
    fs_current_dir->file_list = &list;
    gBlockBitmap = fs_bitmap_init(FS_MAX_BLOCKS);
    fs_current_dir->root.node = NULL;
    file = fs_file_create("1552352", FS_NORMAL_FILE);
    char test[2049];
    char res[5000] = {0};
    int tmp = 0;
    for(int i=0;i< 2049;i++) {
        test[i] = i%26+48;
    }
    fs_print_bitmap(gInodeBitmap);
    if(2049 !=fs_file_write_data(file->name,test,2049)) {
        printf("file test write error\n");
        return 0;
    }
    fs_datablock_table_print(fs_block_table);
    if(2049 != (tmp = fs_file_read_data(file->name,res))){
        printf("file test read error %d\n",tmp);
    }
    
    printf("%s\n",res+sizeof(struct fs_file));
    file = fs_file_create("dir",FS_INDEX_FILE);
    uint64_t hash = jhash("dir",3, 0);
    rb_node* node =NULL;
    struct fs_file* tmpFile = NULL;
    node = iterative_rbtree_search(&fs_current_dir->root,hash<<32);
    fs_change_dirent(node->key);
    fs_list_file();
    tmpFile = fs_file_read(fs_current_dir->hash);
    fs_file_print(tmpFile);
    file = fs_file_create("dir2",FS_INDEX_FILE);
    fs_file_create("tmp.text",FS_NORMAL_FILE);
    fs_file_create("tmp.doc",FS_NORMAL_FILE);
    fs_file_create("tmp.doc",FS_NORMAL_FILE);
    fs_change_dirent(file->hash);
    tmpFile = fs_file_read(fs_current_dir->hash);
    fs_file_print(tmpFile);
    fs_change_dirent(fs_current_dir->pre_hash);
    fs_list_file();
    if(2049 !=fs_file_write_data("tmp.doc",test,2049)) {
        printf("file test write error\n");
        return 0;
    }
    if(2049 != (tmp = fs_file_read_data("tmp.doc",res))){
        printf("file test read error %d\n",tmp);
    }
    
    printf("%s:%s\n",file->name,res+sizeof(struct fs_file));
    return 0;
}