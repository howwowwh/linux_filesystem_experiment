#include"fs_hash.h"
int main()
{
  char file1[256] = {'.','.'};
  char file2[256] = {'.','.'};
  int32_t hashValue1 = jhash(file1,strlen(file1),0);
  int32_t hashValue2 = jhash(file2,strlen(file2),hashValue1);
  printf("%x\t%x\n",hashValue1,hashValue2);
  printf("file1 %s %s\n",file1,file2);
  return 0;
}