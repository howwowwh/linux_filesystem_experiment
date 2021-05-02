#ifndef __FS_COMMON_H
#define __FS_COMMON_H

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h> 
#define OK 0
#define NOK -1
#define FS_MAX_INODES 128   //最大inode数量
#define FS_MAX_BLOCKS 256   //最大block数量
#define FS_MAX_DATA 32 //每个block最大存储数据量
#endif