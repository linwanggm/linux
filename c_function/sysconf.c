sysconf()函数返回给定系统对应的变量值。
头文件 <unistd.h>
返回值：
如果出错，那么函数返回 -1 ，并适当地设置 errno 。当没有错误发生时， -1 也是一个合法的返回值。
因此，程序要检查错误，应该在调用 sysconf() 之前将 errno 设置为 0 ，然后，如果返回 -1，则检验到错误。（来自百度百科）

[wln@localhost 2016]$ cat sysconf.c
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#define ONE_MB (1024 * 1024)

int main (void)
{
  long num_procs;
  long page_size;
  long num_pages;
  long free_pages;
  long long mem;
  long long free_mem;
  int res = 0;
  num_procs = sysconf (_SC_NPROCESSORS_CONF);
  printf ("CPU 个数为: %ld 个\n", num_procs);
  page_size = sysconf (_SC_PAGESIZE);
  printf ("系统页面的大小为: %ld K\n", page_size / 1024 );
  num_pages = sysconf (_SC_PHYS_PAGES);
  printf ("系统中物理页数个数: %ld 个\n", num_pages);
  free_pages = sysconf (_SC_AVPHYS_PAGES);
  printf ("系统中可用的页面个数为: %ld 个\n", free_pages);
  mem = (long long) ((long long)num_pages * (long long)page_size);
  mem /= ONE_MB;
  free_mem = (long long)free_pages * (long long)page_size;
  free_mem /= ONE_MB;
  printf ("总共有 %lld MB 的物理内存, 空闲的物理内存有: %lld MB\n", mem, free_mem);
  
  res = sysconf(1000000000);
  printf("%d %d\n", res, errno);
  perror("get error:");
  
  return (0);
}
[wln@localhost 2016]$ ./sysconf
CPU 个数为: 1 个
系统页面的大小为: 4 K
系统中物理页数个数: 128506 个
系统中可用的页面个数为: 31943 个
总共有 501 MB 的物理内存, 空闲的物理内存有: 124 MB
-1 22
get error:: Invalid argument
[wln@localhost 2016]$ free -m
             total       used       free     shared    buffers     cached
Mem:           501        377        124          0         12        182
-/+ buffers/cache:        182        319
Swap:          511         49        462


[wln@localhost 2016]$ getconf -a |grep NPROCESSORS_CONF
_NPROCESSORS_CONF                  1

需要注意的是：num_procs = sysconf (_SC_NPROCESSORS_CONF) 这个是获取全部CPU个数（也可以使用/etc/cpuinfo查看）；如果获取可用CPU个数，
使用num_procs = sysconf(_SC_NPROCESSORS_ONLN);




