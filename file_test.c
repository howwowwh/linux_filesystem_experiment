#include"fs_common.h"
#include"fs_superblock.h"
#include"fs_datablock.h"
#include"fs_inode.h"
#include"fs_rbtree.h"
#include"fs_hash.h"
#include"fs_file.h"

int main(void)
{
    struct fs_file* file = NULL;
    struct fs_file_entry list;
    fs_init();
    fs_superblock_init(&sb);

    char test[2049];
    char res[2050] = {0};
    int tmp = 0;
    for(int i=0;i< 2049;i++) {
        test[i] = i%26+48; 
    }
    fs_print_bitmap(&gInodeBitmap);

    printf("write OK\n");
    int a = sizeof(struct fs_file);
    
    fs_file_create("dir",FS_INDEX_FILE);
    fs_print_bitmap(&gInodeBitmap);
    uint64_t hash = jhash("dir",3, 0);
    rb_node* node =NULL;
    struct fs_file* tmpFile = NULL;
    node = iterative_rbtree_search(&fs_current_dir->root,hash<<32);
    fs_change_dirent(node->key);
    fs_list_file();
    tmpFile = fs_file_read(fs_current_dir->hash);
    fs_file_print(tmpFile);
    fs_file_create("dir2",FS_INDEX_FILE);
    fs_file_create("tmp.text",FS_NORMAL_FILE);
    fs_file_create("tmp.doc",FS_NORMAL_FILE);
    fs_file_create("tmp.doc",FS_NORMAL_FILE);
    cd("dir2");
    tmpFile = fs_file_read(fs_current_dir->hash);
    fs_file_print(tmpFile);
    fs_change_dirent(fs_current_dir->pre_hash);
    fs_list_file();
    fs_print_bitmap(&gInodeBitmap);
    if(2049 !=fs_file_write_data("tmp.doc",test,2049)) {
        printf("file test write error\n");
        return 0;
    }
    if(2049 != (tmp = fs_file_read_data("tmp.doc",res))){
        printf("file test read error %d\n",tmp);
    }
    
    printf("tmp.doc:\n%s\n",res);
    cat("tmp.doc");
    inorder_rbtree(&fs_current_dir->root);
    printf("\n");
    
    return 0;
}