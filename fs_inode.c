#include <stdlib.h>
#include <time.h>
#include "fs_inode.h"
#include "fs_bitmap.h"
#include "fs_datablock.h"
struct fs_inode fs_inode_table[FS_MAX_INODES] = {0};
uint32_t ginode_num = 0;
void fs_inode_init(void)
{
    int i = 0;
    system("rm -rf ./FileSystem/inodes && cd FileSystem && mkdir inodes");
    gInodeBitmap = fs_bitmap_init(FS_MAX_INODES);
    for (i = 0; i < FS_MAX_INODES; i++)
    {
        fs_inode_one_init(i);
    }
}
struct fs_inode* fs_inode_create()
{
    int inode_num = fs_bitmap_get_free_num(gInodeBitmap);
    if(inode_num < 0) {
        return NULL;
    }
    fs_bitmap_set_bit(gInodeBitmap,inode_num);
    ginode_num++;
    return &fs_inode_table[inode_num];
}
void fs_inode_delete(struct fs_inode* inode)
{
    if(ginode_num < 1) {
        return;
    }
    int inode_num = fs_inode_get_num(inode);
    fs_bitmap_set_bit_zero(gInodeBitmap,inode_num);
    ginode_num--;
}
int fs_inode_store(struct fs_inode *inode)
{
    int inode_num = 0;
    char file_name[50] = {0};
    FILE *fp;
    inode_num = fs_inode_get_num(inode);
    //printf("store inode_num=%d\n",inode_num);
    sprintf(file_name, "./FileSystem/inodes/inode%d", inode_num);
    fp = fopen(file_name, "r+");
    if (!fp)
    {
        printf("inode%d store open error\n", inode_num);
        return NOK;
    }
    if (1 != fwrite(inode, sizeof(struct fs_inode), 1, fp))
    {
        printf("inode%d store write error\n", inode_num);
        return NOK;
    }
    fclose(fp);
    return OK;
}
int fs_inode_read(struct fs_inode *inode)
{
    int inode_num = 0;
    char file_name[50] = {0};
    FILE *fp;
    inode_num = fs_inode_get_num(inode);
    sprintf(file_name, "./FileSystem/inodes/inode%d", inode_num);
    fp = fopen(file_name, "r+");
    if (!fp)
    {
        printf("inode%d read open error\n", inode_num);
        return NOK;
    }
    if (1 != fread(inode, sizeof(struct fs_inode), 1, fp))
    {
        printf("inode%d read error\n", inode_num);
        return NOK;
    }
    fclose(fp);
    return OK;
}
void fs_inode_one_init(int num)
{
    FILE *fp;
    char buf[50] = {0};
    sprintf(buf, "./FileSystem/inodes/inode%d", num);
    fp = fopen(buf, "w+");
    if (!fp)
    {
        printf("inode%d init open error\n", num);
        return;
    }
    memset(&fs_inode_table[num], 0, sizeof(struct fs_inode));
    memset(fs_inode_table[num].i_block, 0xffffffff, sizeof(fs_inode_table[num].i_block[0]) * FS_N_BLOCKS);
    fs_inode_store(&fs_inode_table[num]);
    fclose(fp);
}

int fs_get_free_inode_num(void)
{
    return fs_bitmap_get_free_num(gInodeBitmap);
}

int fs_inode_read_normal_data(uint8_t *buf, struct fs_inode *inode)
{
    int i = 0;
    int total = 0;
    int read_count = 0;
    for (i = 0; i < FS_NDIR_BLOCKS; i++)
    {
        read_count = fs_read_datablock_NDIR(buf + total, inode->i_block[i]);
        total += read_count;
        if (total >= inode->i_size)
        {
            break;
        }
    }
    return total;
}
int fs_inode_write_normal_data(uint8_t *buf, struct fs_inode *inode, int count)
{
    int i = 0;
    int total = 0;
    int write_count = 0;
    int block_num = 0;
    for (i = 0; i < FS_NDIR_BLOCKS; i++)
    {
        block_num = fs_bitmap_get_free_num(gBlockBitmap);
        inode->i_block[i] = block_num;
        write_count = fs_write_datablock_NDIR(buf + total, inode->i_block[i], count-total);
        fs_bitmap_set_bit(gBlockBitmap, block_num);
        total += write_count;
        inode->i_blocks++;
        if (total >= count)
        {
            break;
        }
    }
    inode->i_size = total;
    time(&(inode->i_atime));
    return total;
}
int fs_inode_read_data(uint8_t *buf, struct fs_inode *inode)
{
    int read_count = 0;
    int size = inode->i_size;
    int total = 0;
    time(&(inode->i_atime));
    read_count = fs_inode_read_normal_data(buf+total, inode);
    total += read_count;
    if (total >= size)
    {
        return total;
    }else
    {
        read_count = fs_read_datablock_IND(buf + total, inode->i_block[FS_IND_BLOCK]);
        total += read_count;
        if (total >= size)
        {
            return total;
        }
        else
        {
            read_count = fs_read_datablock_DIND(buf + total, inode->i_block[FS_DIND_BLOCK]);
            total += read_count;
            if (total >= size)
            {
                return total;
            }
            else
            {
                read_count = fs_read_datablock_TIND(buf + total, inode->i_block[FS_TIND_BLOCK]);
                total += read_count;
                if (total >= size)
                {
                    return total;
                }
                else
                {
                    return NOK;
                }
            }
        }
    }
    return NOK;
}

int fs_inode_delete_data(struct fs_inode *inode)
{

    
    uint32_t max = 0xffffffff;
    int inode_num = fs_inode_get_num(inode);
    if (inode->i_block[FS_TIND_BLOCK] != max)
    {
        fs_delete_datablock_TIND(inode->i_block[FS_TIND_BLOCK]);
    }
    else if (inode->i_block[FS_DIND_BLOCK] != max)
    {
        fs_delete_datablock_DIND(inode->i_block[FS_DIND_BLOCK]);
    }
    else if (inode->i_block[FS_IND_BLOCK] != max)
    {
        fs_delete_datablock_IND(inode->i_block[FS_IND_BLOCK]);
    }
    else
    {
        fs_inode_delete_normal_data(inode);
    }
    fs_inode_one_init(inode_num);
    time(&(inode->i_dtime));
    return OK;
}

int fs_inode_write_data(uint8_t *buf, struct fs_inode *inode, int count)
{
    int write_count = 0;
    int total = 0;
    int next_count = count;
    int block_num = 0;
    int inode_num = fs_inode_get_num(inode);
    time(&(inode->i_atime));
    write_count = fs_inode_write_normal_data(buf + total, inode, next_count);
    if (write_count < 0)
    {
        printf("inode%d write error\n", inode_num);
    }
    total += write_count;
    if (total >= count)
    {
        inode->i_size = total;
        return total;
    }else{
        next_count = count - total;
        block_num = fs_bitmap_get_free_num(gBlockBitmap);
        inode->i_block[FS_IND_BLOCK] = block_num;
        fs_bitmap_set_bit(gBlockBitmap, block_num);
        write_count = fs_write_datablock_IND(buf + total, inode->i_block[FS_IND_BLOCK], next_count);
        if(write_count < 0)
        {
            printf("inode%d write error\n", inode_num);
        }
        total += write_count;
        inode->i_blocks++;
        if (total >= count)
        {
            inode->i_size = total;
            return total;
        }else{
            next_count = count - total;
            block_num = fs_bitmap_get_free_num(gBlockBitmap);
            inode->i_block[FS_DIND_BLOCK] = block_num;
            if(NOK == fs_bitmap_set_bit(gBlockBitmap, block_num)) {
                return NOK;
            }
            write_count = fs_write_datablock_DIND(buf + total, inode->i_block[FS_DIND_BLOCK], next_count);
            if (write_count < 0)
            {
                printf("inode%d write error\n", inode_num);
            }
            total += write_count;
            inode->i_blocks++;
            if (total >= count)
            {
                inode->i_size = total;
                return total;
            }else{
                next_count = count - total;
                block_num = fs_bitmap_get_free_num(gBlockBitmap);
                inode->i_block[FS_TIND_BLOCK] = block_num;
                fs_bitmap_set_bit(gBlockBitmap, block_num);
                write_count = fs_write_datablock_TIND(buf + total, inode->i_block[FS_TIND_BLOCK], next_count);
                if (write_count < 0)
                {
                    printf("inode%d write error\n", inode_num);
                }
                total += write_count;
                inode->i_blocks++;
                if (total >= count)
                {
                    inode->i_size = total;
                    return total;
                }else{
                    return NOK;
                }
            }
        }
    }

    return NOK;
}

void fs_inode_delete_normal_data(struct fs_inode *inode)
{
    int i = 0;
    for (int i = 0; i < FS_NDIR_BLOCKS; i++)
    {
        if (inode->i_block[i] != 0xffffffff)
        {
            fs_delete_datablock_NDIR(inode->i_block[i]);
        }
    }
}

int fs_inode_get_num(struct fs_inode *inode)
{
    for (int i = 0; i < FS_MAX_INODES; i++)
    {
        if (inode == &fs_inode_table[i])
        {
            return i;
        }
    }
    return NOK;
}

void fs_inode_show_detail(struct fs_inode *inode)
{
    printf("imode:%d\n", inode->i_mode);
    printf("i_uid:%d\n", inode->i_uid);
    printf("i_size:%d\n", inode->i_size);
    printf("i_ctime:%ld\n", inode->i_ctime);
    printf("i_atime:%ld\n", inode->i_atime);
    printf("i_mtime:%ld\n", inode->i_mtime);
    printf("i_dtime:%ld\n", inode->i_dtime);
    printf("i_gid:%d\n", inode->i_gid);
    printf("i_link_count:%d\n", inode->i_links_count);
    printf("i_blokcs:%d\n", inode->i_blocks);
    printf("i_flags:%d\n", inode->i_flags);
    for (int i = 0; i < FS_N_BLOCKS; i++)
    {
        printf("i_block[%d]:%d\n", i, inode->i_block[i]);
    }
    printf("i_file_acl:%d\n", inode->i_file_acl);
    printf("i_dir_acl:%d\n", inode->i_dir_acl);
}

int fs_inode_read_all(void)
{
    for(int i = 0; i < FS_MAX_INODES;i++) {
        if(NOK == fs_inode_read(&fs_inode_table[i])){
            return NOK;
        }
    }
    return OK;
}