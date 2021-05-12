#include"fs_inode.h"
#include"fs_bitmap.h"
#include"fs_datablock.h"
int main()
{
    fs_inode_init();
    fs_datablock_init();
    gBlockBitmap = fs_bitmap_init(FS_MAX_BLOCKS);
    struct fs_inode* inode = &(fs_inode_table[0]);
    char test[2049];
    char res[2050] = {0};
    int tmp = 0;
    int inode_num = fs_bitmap_get_free_num(gInodeBitmap);
    for(int i=0;i< 2049;i++) {
        test[i] = i%26+48;
    }
    fs_print_bitmap(gInodeBitmap);
    if(2049 !=fs_inode_write_data(test,inode,2049)) {
        printf("inode test write error\n");
        return 0;
    }
    fs_datablock_table_print(fs_block_table);
    fs_inode_show_detail(inode);
    if(2049 != (tmp = fs_inode_read_data(res,inode))){
        printf("inode test read error %d\n",tmp);
    }
    fs_inode_delete_data(inode);
    printf("%s\n",res);
    fs_inode_show_detail(inode);
    return 0;
}