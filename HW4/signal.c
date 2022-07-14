#define _GNU_SOURCE 
#include <sys/wait.h>
#include <signal.h>     //signal(), SIGINT, SIGQUIT
#include <limits.h>     //UNIT_MAX
#include <stdio.h>      //printf()
#include <stdlib.h>     // exit(), EXIT_SUCCESS
#include <string.h>
#include <unistd.h>
#include <errno.h>


int kill(pid_t pid, int sig);     //return 0 on success,;-1 on error


int main(int argc, char **argv){
  pid_t pid;
  int signal;
  int e;
  if (argc == 1)  {
    printf("Usage: ./signal [options] <pid>\n");
    printf("\nOptions:\n");
    printf("\t-s <signal>  Sends <signal> to <pid>\n");
    printf("\t-l           Lists all signal numbers with their names\n");
    return EXIT_FAILURE;
    }
  if ( strcmp(argv[1], "-s") == 0){
    signal = atoi(argv[2]);
    pid = atoi(argv[3]);
    if (signal){
      kill(pid,signal);
      e = 0 ;
    }
    else {
      kill(pid,0);
      e = 1;
    }
  }

  else if (strcmp(argv [1], "-l") == 0 ){
    for (int i =1; i < NSIG; i++)  {
      printf("%2d: SIG%s (%s).\n", i, sys_siglist[i], strsignal(i));
    }

  }

  else {
    signal = SIGTERM;
    pid = atoi(argv[1]);
    kill (pid, signal);
  }

  if( e == 1) {
    if (errno == EPERM) {
			printf("PID %i exists, but we can't send it signals\n",pid);
		} 
    else if (errno == ESRCH) {
			printf("PID %i does not exist\n",pid);
		} 
    else {
			printf("PID %i exists and is able to receive signals\n",pid);
		}
	}
  return 0;
}

