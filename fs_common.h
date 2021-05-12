#ifndef __FS_COMMON_H
#define __FS_COMMON_H

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h> 
#include<time.h>
#include"list.h"

#define OK 0
#define NOK -1
#define FS_MAX_INODES 128  //最大inode数量
#define FS_MAX_BLOCKS 1024   //最大block数量
#define FS_MAX_DATA 1020 //每个block最大存储数据量
#define FILE_NAME_LEN 256
#endif