#include "fs_common.h"
#include "fs_superblock.h"
#include "fs_datablock.h"
#include "fs_inode.h"
#include "fs_file.h"
#include "fs_hash.h"
#include "list.h"

struct dir_private_info *fs_current_dir = &dpi;
struct dir_private_info dpi;
void fs_file_list_clear_zero(uint64_t *list, uint32_t count)
{
    uint64_t tmp = 0;
    for (int i = 0; i < count - 1; i++)
    {
        if (list[i] == 0)
        {
            tmp = list[i];
            list[i] = list[i + 1];
            list[i + 1] = tmp;
        }
    }
}
int fs_file_create(char *fileName, int type)
{
    struct fs_inode *inode;
    struct fs_file *file;
    uint32_t inode_num = 0;
    uint64_t hash = 0;
    rb_node *tree = NULL;

    file = (struct fs_file *)malloc(sizeof(struct fs_file));
    memset(file, 0, sizeof(struct fs_file));
    inode = fs_inode_create();
    if (inode == NULL)
    {
        return NOK;
    }
    memset(inode, 0, sizeof(struct fs_inode));
    memset(inode->i_block, 0xffffffff, sizeof(inode->i_block[0]) * FS_N_BLOCKS);
    inode_num = fs_inode_get_num(inode);
    time(&inode->i_ctime);
    if (type == FS_INDEX_FILE)
    {
        inode->i_file_acl = 444;
        file->file_type = inode->i_flags = FS_INDEX_FILE;
    }
    else if (type == FS_NORMAL_FILE)
    {
        inode->i_file_acl = 666;
        file->file_type = inode->i_flags = FS_NORMAL_FILE;
    }

    file->hash = jhash(fileName, strlen(fileName), 0);
    file->hash <<= 32;
    file->file_size = 0;
    if (NULL == (tree = iterative_rbtree_search(&fs_current_dir->root, file->hash)))
    {
        file->hash += inode_num;
    }
    else
    {
        file->hash = jhash(fileName, strlen(fileName), fs_current_dir->hash);
        file->hash <<= 32;
        if (NULL == (tree = iterative_rbtree_search(&fs_current_dir->root, file->hash)))
        {
            file->hash += inode_num;
        }
        else
        {
            free(file);
            file = NULL;
            if (strcmp("/", fileName) != 0)
                printf("%s is existed\n", fileName);
            fs_bitmap_set_bit_zero(&gInodeBitmap, inode_num);
            return NOK;
        }
    }
    strncpy(file->name, fileName, strlen(fileName));
    file->name_len = strlen(fileName);
    file->file_len = 0;
    memset(file->file_list, 0, sizeof(file->file_list));
    if (sb.sb_root_dir == 0)
    {
        if (type == FS_NORMAL_FILE)
        {
            printf("cant not be a file!\n");
            fs_bitmap_set_bit_zero(&gInodeBitmap, inode_num);
            return NOK;
        }
        file->prev_hash = file->hash;
        fs_current_dir->pre_hash = file->hash;
        fs_current_dir->hash = file->hash;
        sb.sb_root_dir = 1;
    }
    else
    {
        if (sb.sb_root_dir == 0)
        {
            fs_current_dir->pre_hash = file->hash;
            fs_current_dir->hash = file->hash;
        }
        file->prev_hash = fs_current_dir->hash;
        fs_dirent_add_prev(fs_current_dir->hash, file->hash);
    }
    sb.fs_all_file[inode_num] = file->hash;
    if (NOK == insert_rbtree(&fs_current_dir->root, file->hash))
    {
        fs_bitmap_set_bit_zero(&gInodeBitmap, inode_num);
        return NOK;
    }
    fs_file_store(file);
    fs_bitmap_set_bit(&gInodeBitmap, inode_num);
    fs_superblock_write(&sb);
    free(file);
    file = NULL;
    fs_change_dirent(fs_current_dir->hash);
    return OK;
}

void fs_dirent_add_prev(uint64_t pre_hash, uint64_t new_hash)
{
    struct fs_file *last = NULL;
    int flag = 0;
    int len = 0;
    last = fs_file_read(pre_hash);
    len = last->file_len;
    last->file_list[last->file_len++] = new_hash;
    for(int i = 0; i < len; i++) {
        if(last->file_list[i] == 0){
            break;
        }
        if(last->file_list[i] == new_hash){
            last->file_list[i-flag] = 0;
            fs_file_list_clear_zero(last->file_list, last->file_len);
            last->file_len--;
            flag++;
        }
    }
    fs_file_store(last);
}

void fs_file_store(struct fs_file *dir)
{
    struct fs_inode *inode;
    inode = &sb.sb_inode_table[(uint32_t)dir->hash];
    if (sizeof(struct fs_file) != fs_inode_write_data((uint8_t *)dir, inode, sizeof(struct fs_file)))
    {
        printf("dirent store error\n");
    }
}

struct fs_file *fs_file_read(uint64_t hash)
{
    struct fs_file *dir = NULL;
    char *file = NULL;
    char *tmp = NULL;
    struct fs_inode *inode = &sb.sb_inode_table[(uint32_t)hash];
    tmp = (char *)malloc(inode->i_size);
    dir = (struct fs_file *)malloc(sizeof(struct fs_file));
    file = (char *)dir;
    if (inode->i_size != fs_inode_read_data(tmp, inode))
    {
        return NULL;
    }
    for (int i = 0; i < sizeof(struct fs_file); i++)
    {
        file[i] = tmp[i];
    }
    free(tmp);
    tmp = NULL;
    return dir;
}

int fs_dirent_root_recreate(struct dir_private_info *current_dir)
{
    struct fs_file *file;
    struct fs_file_entry *newNode = NULL;
    struct list_head *tmp = &(current_dir->file_list->list);
    INIT_LIST_HEAD(tmp);
    file = fs_file_read(sb.fs_all_file[0]);
    if (file == NULL)
    {
        printf("recreate failed!\n");
        return NOK;
    }
    for (int i = 0; i < FS_MAX_INODES; i++)
    {
        if (sb.fs_all_file[i] != 0)
        {
            insert_rbtree(&current_dir->root, sb.fs_all_file[i]);
        }
        else
        {
            break;
        }
    }
    for (int i = 0; i < FS_MAX_INODES; i++)
    {
        newNode = (struct fs_file_entry *)malloc(sizeof(struct fs_file_entry));
        newNode->file_hash = file->file_list[i];
        if (file->file_list[i] == 0)
        {
            free(newNode);
            newNode == NULL;
            break;
        }
        list_add(&newNode->list, tmp);
        tmp = tmp->next;
    }
    current_dir->hash = sb.fs_all_file[0];
    current_dir->pre_hash = sb.fs_all_file[0];
    sb.sb_root_dir = 1;
    return OK;
}
void fs_change_dirent(uint64_t hash)
{
    struct fs_file_entry *pos = NULL;
    struct fs_file_entry *newNode = NULL;
    struct list_head *tmp = NULL;
    struct fs_file_entry *n = NULL;
    struct fs_file *cd = NULL;

    cd = fs_file_read(hash);
    if (!cd)
    {
        return;
    }
    if (cd->file_type != FS_INDEX_FILE)
    {
        printf("this file is not a dirent\n");
        return;
    }
    if (fs_current_dir->file_list->list.next == NULL)
    {
        goto NEXT;
    }
    list_for_each_entry_safe(pos, n, &fs_current_dir->file_list->list, list)
    {
        list_del(&pos->list);
        free(pos);
    }
NEXT:
    INIT_LIST_HEAD(&fs_current_dir->file_list->list);
    tmp = &fs_current_dir->file_list->list;
    for (int i = 0; i < cd->file_len; i++)
    {
        if (cd->file_list[i] != 0)
        {
            newNode = (struct fs_file_entry *)malloc(sizeof(struct fs_file_entry));
            newNode->file_hash = cd->file_list[i];
            list_add(&newNode->list, tmp);
            tmp = tmp->next;
        }
    }
    fs_current_dir->pre_hash = cd->prev_hash;
    fs_current_dir->hash = cd->hash;
}

void fs_list_file(void)
{
    struct fs_file_entry *pos = NULL;
    int i = 0;
    struct fs_file *file = NULL;
    list_for_each_entry(pos, &fs_current_dir->file_list->list, list)
    {
        file = fs_file_read(pos->file_hash);
        if(!file) {
            continue;
        }
        if (file->file_type == FS_INDEX_FILE)
        {
            printf(" \e[1;32m%s\e[0m" + !(i++), file->name);
        }
        else
        {
            printf(" %s" + !(i++), file->name);
        }
    }
    printf("\n");
}

int fs_file_read_data(char *file_name, char *buf)
{
    uint64_t hash = 0;
    rb_node *node = NULL;
    int read_count = 0;
    char *read_buf = NULL;
    struct fs_file *file = NULL;
    struct fs_inode *inode = NULL;
    if (strlen(file_name) > 255)
    {
        printf("file name too long\n");
        return NOK;
    }
    hash = jhash(file_name, strlen(file_name), 0);
    if (NULL == (node = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
    {
        hash = jhash(file_name, strlen(file_name), fs_current_dir->hash);
        if (NULL == (node = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
        {
            printf("%s is existed\n", file_name);
            return NOK;
        }
    }
    file = fs_file_read(node->key);
    if (!file)
    {
        printf("file read error\n");
        return NOK;
    }
    if (file->file_type == FS_INDEX_FILE)
    {
        printf("can't read a dirent\n");
        return NOK;
    }
    read_buf = (char *)malloc(file->file_size + sizeof(struct fs_file) + 1);
    memset(read_buf, 0, file->file_size + sizeof(struct fs_file) + 1);
    read_count = fs_inode_read_data(read_buf, &sb.sb_inode_table[(uint32_t)node->key]);
    if (file->file_size + sizeof(struct fs_file) != read_count)
    {
        free(read_buf);
        read_buf = NULL;
        printf("read file error\n");
        return NOK;
    }
    memcpy(buf, read_buf + sizeof(struct fs_file), file->file_size);
    free(read_buf);
    read_buf = NULL;
    return file->file_size;
}

int fs_file_write_data(char *file_name, char *buf, int count)
{
    uint64_t hash = 0;
    rb_node *node = NULL;
    int i = 0;
    char *file_c = NULL;
    char *write_buf = NULL;
    struct fs_file *file = NULL;
    struct fs_inode *inode = NULL;
    if (strlen(file_name) > 255)
    {
        printf("file name too long\n");
        return NOK;
    }
    hash = jhash(file_name, strlen(file_name), 0);
    if (NULL == (node = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
    {
        hash = jhash(file_name, strlen(file_name), fs_current_dir->hash);
        if (NULL == (node = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
        {
            printf("%s is existed\n", file_name);
            return NOK;
        }
    }
    file = fs_file_read(node->key);
    if (!file)
    {
        printf("file read inode error\n");
        return NOK;
    }
    if (file->file_type == FS_INDEX_FILE)
    {
        printf("can't write a dirent\n");
        return NOK;
    }
    write_buf = (char *)malloc(sizeof(struct fs_file) + strlen(buf) + 1);
    file_c = (char *)file;
    memset(write_buf, 0, sizeof(struct fs_file) + strlen(buf) + 1);
    file->file_size = count;
    memcpy(write_buf, file_c, sizeof(struct fs_file));
    memcpy(write_buf + sizeof(struct fs_file), buf, count);
    if (count + sizeof(struct fs_file) != fs_inode_write_data(write_buf, &sb.sb_inode_table[(uint32_t)node->key], count + sizeof(struct fs_file)))
    {
        printf("write file error\n");
        fs_inode_one_init((uint32_t)hash);
        free(write_buf);
        write_buf = NULL;
        return NOK;
    }
    free(write_buf);
    write_buf = NULL;
    fs_superblock_write(&sb);
    return count;
}

void fs_file_print(struct fs_file *file)
{
    printf("fileName:%s\n", file->name);
    printf("fileSize:%d\n",file->file_size);
    printf("fileLen:%d\n", file->file_len);
    printf("fileType:%d\n", file->file_type);
    for(int i =0;i<file->file_len;i++) {
        printf(" %lx"+!i,file->file_list[i]);
    }
    printf("\n");
    printf("fileHash:%lx\n", file->hash);
    printf("filePrehash:%lx\n", file->prev_hash);
}
void cd(char *dir_name)
{
    uint64_t hash = 0;
    rb_node* node1 = NULL;
    rb_node* node2 = NULL;
    if (strlen(dir_name) > 255)
    {
        printf("file name too long\n");
        return;
    }
    hash = jhash(dir_name, strlen(dir_name), 0);
    if (hash == CURR_DIR)
    {
        fs_change_dirent(fs_current_dir->hash);
    }
    else if (hash == LAST_DIR)
    {
        fs_change_dirent(fs_current_dir->pre_hash);
    }
    else
    {
        if (NULL == (node1 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
        {
            hash = jhash(dir_name, strlen(dir_name), fs_current_dir->hash);
            if (NULL == (node1 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
            {
                printf("%s invaild\n", dir_name);
                return;
            }
        } else{
            
            hash = jhash(dir_name, strlen(dir_name), fs_current_dir->hash);
            if (NULL != (node2 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
            {
                fs_change_dirent(node2->key);
                return;
            }
        }
        fs_change_dirent(node1->key);
    }
}
void ls()
{
    fs_list_file();
}

void cat(char *file_name)
{
    uint64_t hash = 0;
    rb_node *node = NULL;
    int read_count = 0;
    char *read_buf = NULL;
    struct fs_file *file = NULL;
    struct fs_inode *inode = NULL;
    if (!file_name)
    {
        return;
    }
    if (strlen(file_name) > 255)
    {
        printf("file name too long\n");
        return;
    }
    hash = jhash(file_name, strlen(file_name), 0);
    if (NULL == (node = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
    {
        hash = jhash(file_name, strlen(file_name), fs_current_dir->hash);
        if (NULL == (node = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
        {
            printf("%s is not existed\n", file_name);
            return;
        }
    }
    file = fs_file_read(node->key);
    if (!file)
    {
        printf("file read error\n");
        return;
    }
    if (file->file_type == FS_INDEX_FILE)
    {
        printf("can't read a dirent\n");
        return;
    }
    read_buf = (char *)malloc(file->file_size + sizeof(struct fs_file) + 1);
    memset(read_buf, 0, file->file_size + sizeof(struct fs_file) + 1);
    read_count = fs_inode_read_data(read_buf, &sb.sb_inode_table[(uint32_t)node->key]);
    if (file->file_size + sizeof(struct fs_file) != read_count)
    {
        free(read_buf);
        read_buf = NULL;
        printf("read file error\n");
        return;
    }
    printf("%s:\n%s\n", file_name, read_buf + sizeof(struct fs_file));
    free(read_buf);
    read_buf = NULL;
}
void fs_file_create_root_dir()
{
    if (NOK == fs_file_create("/", FS_INDEX_FILE))
    {
        return;
    }
    cd("/");
}
void fs_current_dir_init()
{
    struct fs_file_entry *pos = NULL;
    struct fs_file_entry *n = NULL;
    destroy_rbtree(&fs_current_dir->root);
    fs_current_dir->root.node = NULL;
    fs_current_dir->hash = 0;
    fs_current_dir->pre_hash = 0;
    if (fs_current_dir->file_list->list.next == NULL)
    {
        INIT_LIST_HEAD(&fs_current_dir->file_list->list);
        return;
    }
    list_for_each_entry_safe(pos, n, &fs_current_dir->file_list->list, list)
    {
        list_del(&pos->list);
        free(pos);
    }
    INIT_LIST_HEAD(&fs_current_dir->file_list->list);
}

void fs_file_delete_pre(uint64_t prehash, uint64_t hash)
{
    struct fs_file *last = NULL;
    last = fs_file_read(prehash);
    for (int i = 0; i < last->file_len; i++)
    {
        if (last->file_list[i] == hash)
        {
            last->file_list[i] = 0;
            break;
        }
    }
    fs_file_list_clear_zero(last->file_list,last->file_len);
    last->file_len--;
    fs_file_store(last);
}
void fs_delete_file(uint64_t prehash, uint64_t hash)
{
    fs_delete_inode((uint32_t)hash);
    fs_file_delete_pre(prehash, hash);
}
void fs_file_delete(uint64_t hash)
{
    struct fs_file *file = NULL;
    uint64_t prehash = 0;
    if(!hash) {
        return;
    }
    file = fs_file_read(hash);
    if (!file)
    {
        printf("delete error\n");
        return;
    }
    prehash = file->prev_hash;
    if (file->file_type == FS_INDEX_FILE)
    {
        for (int i = 0; i < file->file_len; i++)
        {
            if(file->file_list[i] == 0)
                return;
            fs_file_delete(file->file_list[i]);
        }
    }
    fs_delete_inode((uint32_t)hash);
    fs_file_delete_pre(prehash, hash);
    fs_inode_store(&sb.sb_inode_table[(uint32_t)prehash]);
    delete_rbtree(&fs_current_dir->root, hash);

    
}

void delete_file(char *file_name)
{
    uint64_t hash = 0;
    rb_node *node1 = NULL;
    rb_node *node2 = NULL;
    if (strlen(file_name) > 255)
    {
        printf("file name too long\n");
        return;
    }
 
    hash = jhash(file_name, strlen(file_name), 0);

    if (NULL == (node1 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
        {
            hash = jhash(file_name, strlen(file_name), fs_current_dir->hash);
            if (NULL == (node1 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
            {
                printf("%s invaild\n", file_name);
                return;
            }
        } else{
            
            hash = jhash(file_name, strlen(file_name), fs_current_dir->hash);
            if (NULL != (node2 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
            {
                fs_file_delete(node2->key);
                sb.fs_all_file[(uint32_t)(node2->key)] = 0;
                fs_superblock_write(&sb);
                cd(".");
                return;
            }
        }
        fs_file_delete(node1->key);
        sb.fs_all_file[(uint32_t)(node2->key)] = 0;
        fs_superblock_write(&sb);
        cd(".");
}

void printFile(char* file_name)
{
    uint64_t hash = 0;
    rb_node *node1 = NULL;
    rb_node *node2 = NULL;
    struct fs_file* file = NULL;
    struct fs_inode* inode = NULL;
    if (strlen(file_name) > 255)
    {
        printf("file name too long\n");
        return;
    }
 
    hash = jhash(file_name, strlen(file_name), 0);

    if (NULL == (node1 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
        {
            hash = jhash(file_name, strlen(file_name), fs_current_dir->hash);
            if (NULL == (node1 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
            {
                printf("%s invaild\n", file_name);
                return;
            }
        } else{
            
            hash = jhash(file_name, strlen(file_name), fs_current_dir->hash);
            if (NULL != (node2 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
            {
                file = fs_file_read(node2->key);
                fs_file_print(file);
                return;
            }
        }
        file = fs_file_read(node1->key);
        fs_file_print(file);
}

void printInode(char* file_name)
{
    uint64_t hash = 0;
    rb_node *node1 = NULL;
    rb_node *node2 = NULL;
    struct fs_file* file = NULL;
    struct fs_inode* inode = NULL;
    if (strlen(file_name) > 255)
    {
        printf("file name too long\n");
        return;
    }
 
    hash = jhash(file_name, strlen(file_name), 0);

    if (NULL == (node1 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
        {
            hash = jhash(file_name, strlen(file_name), fs_current_dir->hash);
            if (NULL == (node1 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
            {
                printf("%s invaild\n", file_name);
                return;
            }
        } else{
            
            hash = jhash(file_name, strlen(file_name), fs_current_dir->hash);
            if (NULL != (node2 = iterative_rbtree_search(&fs_current_dir->root, hash << 32)))
            {
                fs_inode_show_detail(&sb.sb_inode_table[(uint32_t)node2->key]);
                return;
            }
        }
        fs_inode_show_detail(&sb.sb_inode_table[(uint32_t)node1->key]);
}

void printDB(uint32_t num)
{
    char str[FS_MAX_DATA+1] = {0};
    if(sb.sb_block_table[num]!= fs_read_datablock(str, num)) {
        printf("read datablock error\n");
        return;
    }
    printf("dataBlock%d:\n%s\n",num, str);
}
void printIND(uint32_t num)
{
    uint32_t str[FS_MAX_DATA/4] = {0};
    memset(str,0xffffffff,FS_MAX_DATA/4);
    if(sb.sb_block_table[num]!= fs_read_datablock((uint8_t*)str, num)) {
        printf("read datablock error\n");
        return;
    }
    for(int i = 1; i <= FS_MAX_DATA/4; i++){
        if(str[i-1] == 0xffffffff){
            break;
        }
        if (i % 32 == 0)
        {
            printf("\n");
        }
        printf(" %d"+!(i-1),str[i-1]);
    }
    printf("\n");
}