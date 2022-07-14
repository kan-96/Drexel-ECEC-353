#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "builtin.h"
#include "parse.h"
#include "jobs.h"

#define MAX_JOBS 100

static char* builtin[] = {
    "exit",   /* exits the shell */
    "which",  /* displays full path to command */
    "jobs",
    "kill",
    "bg",
    "fg",
    NULL
};


void set_fg_pgid (pid_t pgid);

int is_builtin (char* cmd)
{
    int i;

    for (i=0; builtin[i]; i++) {
        if (!strcmp (cmd, builtin[i]))
            return 1;
    }

    return 0;
}

void builtin_fg (Task T)
{
    int job_num;
    
    if (T.argv[1] == NULL)
    {
        printf("Usage: fg %%<job number>\n");
        return;
    }
    
    else
    {
        job_num = atoi(T.argv[1] +1);
        
        if (job_get_stt(job_num) == TERM)  {
            printf("pssh: invalid job number: %i\n",job_num);
        }
        else  {
           job_set_stt(job_num,FG);
           set_fg_pgid (job_get_pgid(job_num));
           kill_cmd(job_num,SIGCONT);
           //kill(-1*job_num,SIGCONT);
        }
    }
    //fflush (stdout);
}

void builtin_bg (Task T)
{
   
    int job_num;
    
    if (T.argv[1] == NULL)
    {
        printf("Usage: bg %%<job number>\n");
        return;
    }
    
    else
    {
        job_num = atoi(T.argv[1] +1);
        if (job_get_stt(job_num) == TERM) 
            printf("pssh: invalid job number: %i\n",job_num);
        else  {
            kill_cmd(job_num, SIGCONT);
            //job_set_status (job_num, BG);
            //have to set stt at SIGCHLD) */
        }
    }
    // fflush (stdout);
}

void builtin_kill (Task T)
{

    int signal ;
    int i,job_num;
    i = 1;
    //int temp;
    
    if ( T.argv[i] == NULL)  {
        printf ("Usage: kill [-s <signal>] <pid> | %%<job> \n");
        return;
    }
    
    
    if (!strcmp (T.argv[1], "-s"))  {
        signal = atoi(T.argv[2]);
        i =3;
    }
    while(T.argv[i] != NULL)  {
        job_num = atoi (T.argv[i] +1);
        if (T.argv[i][0] == '%')  
        {
            if (job_get_stt(job_num) == TERM)
                printf("pssh: invalid job number: %i\n",job_num);
            else  
                kill_cmd(job_num, SIGINT);
                //kill(-1*job_num,signal);
        }
        else  
        {
           if (kill(atoi(T.argv[i]),0) == 0)
                kill(atoi(T.argv[i]),signal);
            else
                printf ("pssh: invalid job number(pid): %i\n", atoi(T.argv[i]) );
        }
        i++;
    }
    //fflush (stdout);      
}
void builtin_jobs (Task T)
{
    int i;
    for ( i =0 ; i < MAX_JOBS; i++)
    {
        job_print(i);
    }
    fflush(stdout);
}

void builtin_execute (Task T)
{
    char* dir;
    char* PATH = (char *) malloc(sizeof(char*)*strlen(getenv("PATH")));
    char* probe;
    char* tmp;
    

    if (!strcmp (T.cmd, "exit")) {
        exit (EXIT_SUCCESS);
    }
    
    else if (!strcmp(T.cmd, "which")){
        if (T.argv[1]==NULL){
            exit(EXIT_SUCCESS);
        }
        if (is_builtin(T.argv[1])){
           printf("%s: shell built-in command\n", T.argv[1]);
           exit(EXIT_SUCCESS);
        }
        
        strcpy(PATH,getenv("PATH"));
        
        for (tmp=PATH; ; tmp=NULL) {
            dir = strtok (tmp, ":");
            if (!dir)
              break;
            probe = (char *) malloc(sizeof(char*)*strlen(dir));
            strcpy (probe, dir);
            strcat (probe, "/");
            strcat (probe, T.argv[1]);
            if (access(probe, F_OK) == 0)  {
                printf("%s\n",probe);
                free(PATH);
                free(probe);
                exit(EXIT_SUCCESS);
            }
            free(probe);
        }

        exit(EXIT_SUCCESS);
    }
    else {
        printf ("pssh: builtin command: %s (not implemented!)\n", T.cmd);
    }
}