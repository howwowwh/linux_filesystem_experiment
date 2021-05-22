#include "fs_datablock.h"
#include "fs_superblock.h"

void fs_datablock_init(void)
{
    int i = 0;

    system("rm -rf ./FileSystem/blocks && cd FileSystem && mkdir blocks");
    for (i = 0; i < FS_MAX_BLOCKS; i++)
    {
        fs_datablock_init_one(i);
    }
}
void fs_datablock_init_one(int i)
{
    FILE *fp;
    char buf[50] = {0};
    sb.sb_block_table[i] = 0;
    sprintf(buf, "./FileSystem/blocks/block%d", i);
    fp = fopen(buf, "w");
    fclose(fp);
}
void fs_datablock_clean(int num)
{
    FILE *fp;
    char buf[50] = {0};
    sb.sb_block_table[num] = 0;
    sprintf(buf, "./FileSystem/blocks/block%d", num);
    fp = fopen(buf, "w");
    fclose(fp);
}

int fs_read_datablock(uint8_t *read_buf, int block_num)
{
    int count = sb.sb_block_table[block_num];
    FILE *fp = NULL;
    char buf[50];
    sprintf(buf, "./FileSystem/blocks/block%d", block_num);
    fp = fopen(buf, "r");
    if (!fp)
    {
        //printf("block%d not exist\n", block_num);
        return NOK;
    }
    if (count != fread(read_buf, sizeof(uint8_t), count, fp))
    {
        //printf("read block%d error!\n", block_num);
        return NOK;
    }
    fclose(fp);
    return count;
}

int fs_read_datablock_NDIR(uint8_t *buf, int block_num)
{
    return fs_read_datablock(buf, block_num);
}

int fs_write_datablock(uint8_t *write_buf, int block_num, int count)
{
    fs_datablock_clean(block_num);
    int size = sb.sb_block_table[block_num];
    int free_count = FS_MAX_DATA - size;
    int write_count = free_count > count ? count : free_count;
    int tmp = 0;
    FILE *fp = NULL;
    char buf[50];
    sprintf(buf, "./FileSystem/blocks/block%d", block_num);
    fp = fopen(buf, "r+");
    if (!fp)
    {
        //printf("block%d not exist\n", block_num);
        return NOK;
    }
    if (write_count != (tmp = fwrite(write_buf, sizeof(uint8_t), write_count, fp)))
    {
        //printf("write block%d error!\n", block_num);
        //printf("write_count = %d tmp = %d\n", write_count, tmp);
        return NOK;
    }
    fclose(fp);
    if (size + count >= FS_MAX_DATA)
    {
        sb.sb_block_table[block_num] = FS_MAX_DATA;
    }
    else
    {
        sb.sb_block_table[block_num] = size + count;
    }
    return write_count;
}

int fs_write_datablock_NDIR(uint8_t *buf, int block_num, int count)
{
    return fs_write_datablock(buf, block_num, count);
}

void fs_delete_datablock_NDIR(int block_num)
{
    sb.sb_block_table[block_num] = 0;
    fs_bitmap_set_bit_zero(&gBlockBitmap, block_num);
    printf("delete block%d success!\n",block_num);
}

int fs_read_index(int *read_buf, int block_num)
{
    int count = sb.sb_block_table[block_num] / 4;
    FILE *fp = NULL;
    char buf[50];
    sprintf(buf, "./FileSystem/blocks/block%d", block_num);
    fp = fopen(buf, "r");
    if (!fp)
    {
        //printf("block%d not exist\n", block_num);
        return NOK;
    }
    if (count != fread(read_buf, sizeof(int), count, fp))
    {
        //printf("read block%d error!\n", block_num);
        return NOK;
    }
    fclose(fp);
    return count;
}

int fs_read_datablock_IND(uint8_t *buf, int block_num)
{
    int list[sb.sb_block_table[block_num] / 4];
    int read_count = 0;
    int total = 0;
    int count = fs_read_index(list, block_num);
    for (int i = 0; i < count; i++)
    {
        read_count = fs_read_datablock(buf + total, list[i]);
        total += read_count;
        //printf("read_count %d\n", read_count);
    }
    return total;
}

int fs_write_index(int *read_buf, int block_num, int count)
{
    char buf[50] = {0};
    int size = sb.sb_block_table[block_num];
    int free_count = FS_MAX_DATA - size;
    int write_count = free_count > (count * 4) ? (count * 4) : free_count;
    FILE *fp = NULL;
    sprintf(buf, "./FileSystem/blocks/block%d", block_num);
    fp = fopen(buf, "a+");
    if (!fp)
    {
        //printf("block%d not exist\n", block_num);
        return NOK;
    }
    if (write_count != fwrite(read_buf, sizeof(uint8_t), write_count, fp))
    {
        //printf("write block%d error!\n", block_num);
        return NOK;
    }
    fclose(fp);
    if (size + write_count >= FS_MAX_DATA)
    {
        sb.sb_block_table[block_num] = FS_MAX_DATA;
    }
    else
    {
        sb.sb_block_table[block_num] = size + write_count;
    }
    return write_count / 4;
}

int fs_write_datablock_IND(uint8_t *buf, int block_num, int count)
{
    int free_block = 0;
    int pcount = 0;
    int write_count = 0;
    int write_num = 0;
    int max_index = FS_MAX_DATA / sizeof(int);
    int index[FS_MAX_DATA / sizeof(int)] = {0};
    int index_count = 0;
    index_count = fs_read_index(index,block_num);
    //printf("data_block= %d\n", data_block_count);
    fs_datablock_clean(block_num);
    for (int i = 0; i < max_index; i++)
    {   
        if(index_count == 0){
            free_block = fs_bitmap_get_free_num(&gBlockBitmap);    
        }else{
            free_block = index[i];
        }
        fs_bitmap_set_bit(&gBlockBitmap, free_block);
        fs_write_index(&free_block, block_num, 1);
        write_num = (count - write_count) > FS_MAX_DATA ? FS_MAX_DATA : (count - write_count);
        pcount = fs_write_datablock(buf + write_count, free_block, write_num);
        write_count += pcount;
        //printf("pcount = %d write_count = %d %d\n", pcount, write_count, write_num);
        if (write_count >= count)
        {
            break;
        }
    }
    return write_count;
}

void fs_delete_datablock_IND(int block_num)
{
    int count = 0;
    int i = 0;
    int list[FS_MAX_DATA / 4];
    count = fs_read_index(list, block_num);
    for (i = 0; i < count; i++)
    {
        fs_delete_datablock_NDIR(list[i]);
    }
    fs_delete_datablock_NDIR(block_num);
}
void fs_datablock_table_print(int *list)
{
    printf("datablock_table:\n");
    for (int i = 1; i < FS_MAX_BLOCKS + 1; i++)
    {
        printf("%5d", list[i - 1]);
        if (i % 32 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

int fs_read_datablock_DIND(uint8_t *buf, int block_num)
{
    int list[sb.sb_block_table[block_num] / 4];
    int read_count = 0;
    int total = 0;
    int max_index = FS_MAX_DATA / 4;
    int count = fs_read_index(list, block_num);
    //printf("****count = %d*****\n", count);
    for (int i = 0; i < count; i++)
    {
        read_count = fs_read_datablock_IND(buf + total, list[i]);
        total += read_count;
        //printf("%d read_count=%d total = %d\n", i, read_count, total);
    }
    return total;
}

int fs_write_datablock_DIND(uint8_t *buf, int block_num, int count)
{
    int data_block_count = 0;
    int free_block = 0;
    int pcount = 0;
    int write_count = 0;
    int write_num = 0;
    int max_index = FS_MAX_DATA / 4;
    int size = count / FS_MAX_DATA / max_index;
    if (count % FS_MAX_DATA == 0)
        data_block_count = size;
    else
        data_block_count = size + 1;
    int index[FS_MAX_DATA / sizeof(int)] = {0};
    int index_count = 0;
    index_count = fs_read_index(index,block_num);
    //printf("DIND data_block_count = %d\n", data_block_count);
    fs_datablock_clean(block_num);
    for (int i = 0; i < max_index; i++)
    {
        if(index_count == 0){
            free_block = fs_bitmap_get_free_num(&gBlockBitmap);    
        }else{
            free_block = index[i];
        }
        fs_bitmap_set_bit(&gBlockBitmap, free_block);
        fs_write_index(&free_block, block_num, 1);
        write_num = (count - write_count) > (FS_MAX_DATA * max_index) ? (FS_MAX_DATA * max_index) : (count - write_count);
        pcount = fs_write_datablock_IND(buf + write_count, free_block, write_num);
        write_count += pcount;
        //printf("DIND i=%d\n", i);
        if (write_count >= count)
        {
            break;
        }
    }
    return write_count;
}

void fs_delete_datablock_DIND(int block_num)
{
    int count = 0;
    int i = 0;
    int list[FS_MAX_DATA / 4];
    count = fs_read_index(list, block_num);
    for (i = 0; i < count; i++)
    {
        fs_delete_datablock_IND(list[i]);
    }
    fs_delete_datablock_NDIR(block_num);
}

int fs_read_datablock_TIND(uint8_t *buf, int block_num)
{
    int list[sb.sb_block_table[block_num] / 4];
    int read_count = 0;
    int total = 0;
    int max_index = FS_MAX_DATA / 4;
    int count = fs_read_index(list, block_num);
    //printf("****count = %d*****\n", count);
    for (int i = 0; i < count; i++)
    {
        read_count = fs_read_datablock_DIND(buf + total, list[i]);
        total += read_count;
        //printf("%d read_count=%d total = %d\n", i, read_count, total);
    }
    return total;
}

int fs_write_datablock_TIND(uint8_t *buf, int block_num, int count)
{
    int data_block_count = 0;
    int free_block = 0;
    int pcount = 0;
    int write_count = 0;
    int write_num = 0;
    int max_index = FS_MAX_DATA / 4;
    int size = count / FS_MAX_DATA / max_index;
    int TIND_size = size / max_index;
    if (count % FS_MAX_DATA == 0)
        data_block_count = TIND_size;
    else
        data_block_count = TIND_size + 1;
    int index[FS_MAX_DATA / sizeof(int)] = {0};
    int index_count = 0;
    index_count = fs_read_index(index,block_num);
    fs_datablock_clean(block_num);
    //printf("TIND data_block_count = %d\n", data_block_count);
    for (int i = 0; i < max_index; i++)
    {
        if(index_count == 0){
            free_block = fs_bitmap_get_free_num(&gBlockBitmap);    
        }else{
            free_block = index[i];
        }
        fs_bitmap_set_bit(&gBlockBitmap, free_block);
        fs_write_index(&free_block, block_num, 1);
        write_num = (count - write_count) > (FS_MAX_DATA * max_index) ? (FS_MAX_DATA * max_index * max_index) : (count - write_count);
        pcount = fs_write_datablock_DIND(buf + write_count, free_block, write_num);
        write_count += pcount;
        //printf("DIND i=%d\n", i);
        if (write_count >= count)
        {
            break;
        }
    }
    return write_count;
}

void fs_delete_datablock_TIND(int block_num)
{
    int count = 0;
    int i = 0;
    int list[FS_MAX_DATA / 4];
    count = fs_read_index(list, block_num);
    for (i = 0; i < count; i++)
    {
        fs_delete_datablock_DIND(list[i]);
    }
    fs_delete_datablock_NDIR(block_num);
}