#include <sys/mman.h>   //mmap(), munmap(),PROT_READ
#include <fcntl.h>      //open(), O_RDWR
#include <unistd.h>     //close, fork()
#include <stdlib.h>     //EXIT_
#include <stdio.h>      //printf
#include <sys/wait.h>   //wait()

int main(){
  int fd;
  int *addr;
  pid_t pid;
  fd = open("/dev/zero",  O_RDWR);
  if (fd == -1) {
    fprintf(stderr, "Cannot open file zero\n");
    return EXIT_FAILURE;
  }
  addr = mmap(NULL, sizeof(*addr), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED)  {
    fprintf(stderr, "Cannot mmap.\n");
    return EXIT_FAILURE;
  }
  
  if (close(fd) == -1)  {
    fprintf(stderr, "Cannot close file.\n");
    return EXIT_FAILURE;
  }
  *addr = 1;
  pid = fork();
  if (pid == -1 )  {
    fprintf(stderr, "Cannot fork.\n");
    return EXIT_FAILURE;
  }
  if (pid > 0)  {
    //parent process
    wait(NULL);
    printf("Parent: value = %d\n",*addr);
    if (munmap(addr, sizeof(*addr)) == -1)  {
      fprintf(stderr, "Cannot munmap file (parent).\n");
      return EXIT_FAILURE;
    }
    exit(EXIT_SUCCESS);
  }
  else  {
    //child process
    printf("Child: value = %d\n",*addr);
    (*addr)++;
    if (munmap(addr, sizeof(*addr)) == -1)  {
      fprintf(stderr, "Cannot munmap child (chil).\n");
      return EXIT_FAILURE;
    }
    exit(EXIT_SUCCESS);
  }

  return 0;
}