#include <stdio.h>
#include <stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
/* argv[1]: filename :: the file which need to be read
 * argv[2]: offset   :: the number of bytes off file'head
 * argv[3]: number   :: the number of bytes that wanted to be read
 *
 */

void usage()
{
    printf("\033[31musage:\033[0m\n");
    printf("\033[31margv[1]: filename :: the file which need to be read.\033[0m\n");
    printf("\033[31margv[2]: offset   :: the number of bytes off file'head.\033[0m\n");
    printf("\033[31margv[3]: number   :: the number of bytes that wanted to be read.\033[0m\n\n");

}
int main(int argc, char* argv[])
{
    if (argc!=4)
    {
        printf("the command isnot correct.\n");
        usage();
        exit(1);
    }

    #define nBuffer 4096
    char buffer[nBuffer];

    int fd;
    /* open file */
    if ( (fd=open(argv[1],O_RDONLY))==-1)
    {
        //perror("open file error!"); the next is better.
        fprintf(stderr, "open file %s error! %s\n", argv[1], strerror(errno));
        exit(1);
    }
    
    /*seek the file */
    if(lseek(fd,atoi(argv[2]),SEEK_SET) == -1)
    {
        fprintf(stderr, "lseek file %s error! %s\n", argv[1], strerror(errno));
        close(fd);
        exit(1);
    }

    int nRead=0;
    memset(buffer,0,nBuffer);
    /* read file */
    if( (nRead=read(fd,buffer,atoi(argv[3]))) == -1)
    {
       fprintf(stderr, "read file %s error! %s\n", argv[1], strerror(errno));
       close(fd);
       exit(1);
    }    

    close(fd);
    printf("nRead=%d\n",nRead);
    printf("%s\n",buffer);
  
    return 1;

}
