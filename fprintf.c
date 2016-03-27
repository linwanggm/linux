1. 
使用语法：
int fprintf(FILE *stream, const char *format, ...);
头文件
#include <stdio.h>

2.示例：
#include <stdio.h>

int main()

{
 FILE *pFile;
 char string[100];
 pFile=fopen("myfile.txt","a");
 int i=0;
 for(i=0;i<5;i++)
 {
     gets(string);
     fprintf(pFile,"Name  [%-5.10s]\n",string);
     fflush(pFile);
  }
 fclose(pFile);

  return 0;
}

3. 
%-5.10s : left-justified (-), minimum of ten characters (5), maximum of ten characters (.10), string (s).
fprintf函数为带缓存函数.
fflush() 是把 FILE *里的缓冲区(位于用户态进程空间)刷新到内核中
fsync()  是把内核中对应的缓冲(是在 vfs 层的缓冲)刷新到硬盘中

有关fflush,fsync,sync
参考：http://blog.csdn.net/bennyfun79/article/details/5721507
有关fprintf参考
http://www.cplusplus.com/reference/cstdio/fprintf/







