#include"fs_common.h"
#include"fs_superblock.h"
#include"fs_datablock.h"
#include"fs_inode.h"
#include"fs_rbtree.h"
#include"fs_hash.h"
#include"fs_file.h"

#define FS_FREE(cmd, para) {free(cmd);cmd = NULL;free(para);para=NULL;}
char* slip_str(char* str,int num,char ch)
{
    int count = 0;
    int flag = 0;
    int last_ch = 0;
    char* tmp = str;
    int i = 0;
    int j = 0;
    char* res = NULL;
    for(i = 0;i < strlen(str); i++) {
        if(*tmp == ch) {
            if(flag == num)
                break;
            flag++;
            last_ch = i+1;
        }
        tmp++;
    }
    res = (char*)malloc(i-last_ch+1);
    memset(res, 0, i-last_ch+1);
    strncpy(res, str+last_ch, i-last_ch);
    return res;
}
uint32_t str_to_int(char* list)
{
    uint32_t tmp = 0;
    uint32_t res = 0; 
    tmp = ~tmp;
    res = ~res;
    for(int i = 0; i < strlen(list);i++) {
        if((0<=(list[i]-48))&&((list[i]-48)<=9)){
            tmp = tmp+list[i]-48;
            tmp*=10;
        }else {
            return res;
        }
    }
    res = tmp*10;
    return res;
}
int main()
{
	char str[1024] = {0};
    char* cmd = NULL;
    char* para = NULL;
    char* para2 = NULL;
    fs_superblock_init(&sb);
    while(1) {
        memset(str, 0, sizeof(str));
        printf("\e[1;36mFileSystem:/$\e[0m ");
        fgets(str, sizeof(str), stdin);
        str[strlen(str)-1] = 0;
    //    printf("\n str:%s\n",str);
        cmd = slip_str(str, 0, ' ');
  //     printf("\n cmd:%s\n",cmd);
        para = slip_str(str, 1, ' ');
 //      printf("\n para:%s\n",para);
        if(0 == strcmp("mkdir",cmd)) {
            fs_file_create(para, FS_INDEX_FILE);
            FS_FREE(cmd, para);
        } 
        else if(0 == strcmp("touch",cmd)) {
            fs_file_create(para, FS_NORMAL_FILE);
            FS_FREE(cmd, para)
        }else if(0 == strcmp("cd", cmd)) {
            cd(para);
            FS_FREE(cmd, para)
        }else if(0 == strcmp("ls", cmd)) {
            ls();
            FS_FREE(cmd, para)
        }else if(0 == strcmp("cat", cmd)) {
            cat(para);
            FS_FREE(cmd, para)
        }else if(0 == strcmp("init", cmd)) {
            fs_init();
            FS_FREE(cmd, para)
        }else if(0 == strcmp("write", cmd)) {
            para2 = slip_str(str, 2, ' ');
            fs_file_write_data(para, para2, strlen(para2));
            FS_FREE(cmd, para);
            free(para2);
            para2 = NULL;
        }else if(0 == strcmp("IBM", cmd)) {
            fs_print_bitmap(&gInodeBitmap);
            FS_FREE(cmd, para);
        }else if(0 == strcmp("BBM", cmd)) {
            fs_print_bitmap(&gBlockBitmap);
            FS_FREE(cmd, para);
        }else if(0 == strcmp("rm", cmd)) {
            delete_file(para);
            FS_FREE(cmd, para);
        }else if(0 == strcmp("printInode", cmd)) {
            printInode(para);
            FS_FREE(cmd, para);
        }else if(0 == strcmp("printFile", cmd)) {
            printFile(para);
            FS_FREE(cmd, para);
        }else if(0 == strcmp("printDB", cmd)) {
            printDB(atoi(para));
            FS_FREE(cmd, para);
        }else if(0 == strcmp("printIND", cmd)) {
            printIND(atoi(para));
            FS_FREE(cmd, para);
        }
    }
	return 0;
}