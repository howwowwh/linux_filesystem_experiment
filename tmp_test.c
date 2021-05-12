#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<stdint.h>
#include<sys/stat.h> 
#include<fcntl.h>
union tmp_test
{
    uint8_t list[8];
    int a[2];
};

int main()
{
    int i = 0;
    FILE* fp;
    //system("mkdir FileSystem");
    //system("cd ./FileSystem && mkdir blocks && mkdir inodes && mkdir superblocks");
    char buf[50] = {0};
    int a[2]={111,222};
    union tmp_test x;
    uint8_t test[8];
    for(i =0;i<128;i++) {
        sprintf(buf,"./FileSystem/inodes/inode%d",i);
        fp=fopen(buf, "w");
        memset(buf, 0, sizeof(buf));
        fclose(fp);
    }
    fp = fopen("./FileSystem/inodes/inode0","r+");
    if(2 == fwrite(a,sizeof(int), 2,fp)) {
        printf("ok\n");
    }
    a[0] = 520;
    a[1] = 1314;
    fflush(fp);
    fclose(fp);
    fp = fopen("./FileSystem/inodes/inode0","r+");
    if(2 == fread(a,sizeof(int),2,fp)) {
        printf("%d %d\n",a[0],a[1]);
    }
    fclose(fp);
    fp = fopen("./FileSystem/inodes/inode0","r+");
    if(8 != fread(test,sizeof(uint8_t),8,fp)) {
        printf("error!\n");
    }
    for(i = 0;i<8;i++) {
        x.list[i] = test[i];
    }
    printf("%d %d\n",x.a[0],x.a[1]);
    fclose(fp);
    return 0;
}