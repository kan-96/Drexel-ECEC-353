#include <sys/wait.h>  /*wait functiom*/
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>     /* read/write function */
#include <unistd.h>     /* Dup & read function*/
#include <ctype.h>      /*Toupper function*/
#include <string.h>     /*String function*/

#define BUFFER_SIZE 100
int main (){
pid_t pid;
int p_parent[2], p_child[2] ;
if (pipe(p_parent) == -1) {
  fprintf(stderr, "Failed to create pipe\n");
  return EXIT_FAILURE;
}
if (pipe(p_child) == -1) {
  fprintf(stderr, "Failed to create pipe\n");
  return EXIT_FAILURE;
}
pid = fork ();
if (pid == -1){
  fprintf(stderr, "Failed to work\n");
  return EXIT_FAILURE;
}
if (pid > 0) {
  //parent process
  char input_user;
  char input_child;
  close(p_parent[0]);
  close(p_child[1]);

  while ((read(STDIN_FILENO, &input_user, 1))){
    write(p_parent[1], &input_user, 1);
    int ret = read(p_child[0], &input_child, 1);
    if (ret){
      printf("%c",input_child);
    }
  }
  close (p_parent[1]);
  close (p_child[0]);
  wait (NULL);
}
  
else {
   //child process
  char uppercase;
  char input_child;
  close(p_child[0]);
  close(p_parent[1]);

  while(read(p_parent[0],&input_child,1)){
    uppercase = toupper(input_child);
    write(p_child[1], &uppercase, 1);
  }
  
  close (p_child[1]);
  close (p_parent[0]);
}
return 0;
}
