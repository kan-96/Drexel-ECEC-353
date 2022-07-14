#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main()
{
 pid_t pid;
 pid = vfork();
 if (pid < 0) {
   printf("Failed to fork\n");
   return EXIT_FAILURE;
 }
 if (pid > 0) {
   int child_status;
   waitpid(pid, &child_status, 0);
   printf("Child exited with return code: %i\n", WEXITSTATUS(child_status));
  return 0;
 } 
 else {
   char *argv_list[]= {"ls","-l","-h",NULL};
   execvp(argv_list[0], argv_list);
   printf("Unknow command\n");
   exit(EXIT_FAILURE);
 }
}
