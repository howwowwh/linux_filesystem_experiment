#include<stdlib.h>
#include<time.h>
#include "fs_inode.h"
#include "fs_bitmap.h"


struct fs_inode* fs_create_inode()
{
    struct fs_inode* node;
    node = (struct fs_inode*)malloc(sizeof(struct fs_inode));
    memset(node, 0, sizeof(struct fs_inode));
    time(&node->i_ctime);
    return node;
}

static int fs_get_inode_num(struct fs_inode* inode)
{
    return NOK;
}

int fs_delete_inode(struct fs_inode* inode)
{
    
    return 0; 
}


