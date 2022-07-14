#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "builtin.h"
#include "parse.h"
#include "jobs.h"

/*******************************************
 * Set to 1 to view the command line parse *
 *******************************************/
#define DEBUG_PARSE 0
#define READ_SIDE  0
#define WRITE_SIDE 1


static pid_t our_tty;

void print_banner ()
{
    printf ("                    ________   \n");
    printf ("_________________________  /_  \n");
    printf ("___  __ \\_  ___/_  ___/_  __ \\ \n");
    printf ("__  /_/ /(__  )_(__  )_  / / / \n");
    printf ("_  .___//____/ /____/ /_/ /_/  \n");
    printf ("/_/ Type 'exit' or ctrl+c to quit\n\n");
}


/* returns a string for building the prompt
 *
 * Note:
 *   If you modify this function to return a string on the heap,
 *   be sure to free() it later when appropirate!  */
static char* build_prompt ()
{
    char* temp;

    char* cwd = getcwd (NULL, 0);
    char prompt[] = "$ ";


    temp = malloc (strlen (cwd) + strlen(prompt) + 1);
    sprintf (temp, "%s%s", cwd, prompt);
    free (cwd);

    return temp;
}


/* return true if command is found, either:
 *   - a valid fully qualified path was supplied to an existing file
 *   - the executable file was found in the system's PATH
 * false is returned otherwise */
static int command_found (const char* cmd)
{
    char* dir;
    char* tmp;
    char* PATH;
    char* state;
    char probe[PATH_MAX];

    int ret = 0;

    if (access (cmd, F_OK) == 0)
        return 1;

    PATH = strdup (getenv("PATH"));

    for (tmp=PATH; ; tmp=NULL) {
        dir = strtok_r (tmp, ":", &state);
        if (!dir)
            break;

        strncpy (probe, dir, PATH_MAX);
        strncat (probe, "/", PATH_MAX);
        strncat (probe, cmd, PATH_MAX);

        if (access (probe, F_OK) == 0) {
            ret = 1;
            break;
        }
    }

    free (PATH);
    return ret;
}

static void redirect(int old_fd, int new_fd)
{
    if (old_fd != new_fd)  {
        dup2(new_fd, old_fd);
        close (new_fd);
    }
}

static int in_fd (Parse *P)
{
    if (P->infile)
        return open(P->infile,0);
    else
        return STDIN_FILENO;
}

static int out_fd (Parse *P)
{
    if (P->outfile)
        return open (P->outfile, O_CREAT | O_WRONLY | O_TRUNC, 0664);
    else
        return STDOUT_FILENO;
}

static int close_file (int fd)
{
    if ((fd != STDIN_FILENO) && fd != (STDOUT_FILENO))
        return close (fd);

    return -1;
}

void set_fg_pgid (pid_t pgid)
{
    void (*sav)(int);

    sav = signal (SIGTTOU, SIG_IGN);
    tcsetpgrp (STDIN_FILENO, pgid);
    tcsetpgrp (STDOUT_FILENO, pgid);
    signal (SIGTTOU, sav);
}
static void signal_handler (int sig)
{
    pid_t chld;
    char* job_name;
    unsigned int job_num;
    int status;

    switch (sig) {
    case SIGTTOU:
        while (tcgetpgrp (STDIN_FILENO) != our_tty)
            pause ();

        break;

    case SIGTTIN:
        while (tcgetpgrp (STDIN_FILENO) != our_tty)
            pause ();

        break;

    case SIGCHLD:
        while ((chld = waitpid (-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
            job_num = job_get_num (chld);
            
            if (WIFSTOPPED (status)) {
            //child recieved SIGTSTP, and check current state
                if (job_get_stt(job_num) != STOPPED)
                {
                    //only report when chld in running state and give back ternminal
                    job_set_stt (job_num, STOPPED);
                    set_fg_pgid (our_tty);                   
                    job_name = job_get_name (job_num);              
                    printf ("\n[%u] + suspended \t%s\n", job_num, job_name);
                    fflush (stdout);
                    free (job_name);
                }
                continue;
            }
            else if (WIFCONTINUED (status)) {
                //child received SIGCONT
                if (job_get_stt(job_num) == STOPPED)
                {    
                    //for chld being stopped in background and not moved to fg
                    job_set_stt (job_num, BG);
                    job_name = job_get_name (job_num);
                    printf ("\n[%u] + continued \t%s\n", job_num, job_name);
                    fflush (stdout);
                    free (job_name);
                    
                }
                continue;
            }
            else {
                //if WIFEXITED or  WIFSIGNALED
                //child being terminated;
                terminate_pid (chld);
            }
            /*Keep in mind that when a foreground
            job completes, the shell’s process group does not automatically get set to the foreground!*/ 
            if (job_completed (job_num)) {
                job_name = job_get_name (job_num);
                //"done" not be displayed when a foreground process completes
                
                if (job_get_stt (job_num) == FG) {
                    set_fg_pgid (our_tty);
                }
                else
                {
                
                    printf ("\n[%u] + done \t%s\n", job_num, job_name);
                    fflush (stdout);
                }
                
                job_remove (job_num);
                free (job_name);
            }
        }
    }
}

/* Called upon receiving a successful parse.
 * This function is responsible for cycling through the
 * tasks, and forking, executing, etc as necessary to get
 * the job done! */
void execute_tasks (Parse* P, char* job_name)
{
    unsigned int t;
    int fd[2];
    int in, out;
    pid_t* pid;
    
    //check possible
    for (t=0; t<P->ntasks; t++) {
        if (!is_builtin (P->tasks[t].cmd) && !command_found (P->tasks[t].cmd)) {
            printf ("pssh: command not found: %s\n",P->tasks[t].cmd);
            fflush (stdout);
            return;
        }
    }

    if (P->infile) {
        if (access (P->infile, R_OK) != 0) {
            printf ("pssh: no such file or directory: %s\n", P->infile);
            fflush (stdout);
            return;
        }
    }

    if (P->outfile)  { 
        if (open (P->outfile, O_CREAT | O_WRONLY | O_TRUNC, 0664) == -1)  {
            printf ("Failed to access outfile: %s\n", P->outfile);
            fflush(stdout);
            return;
        }
    }
    //check build-in cmd
    if (!strcmp (P->tasks[0].cmd, "exit")) {
        exit (EXIT_SUCCESS);
    }
    else if (!strcmp (P->tasks[0].cmd, "fg")) {
        builtin_fg (P->tasks[0]);
        return;
    }
    else if (!strcmp (P->tasks[0].cmd, "bg")) {
        builtin_bg (P->tasks[0]);
        return;
    }
    else if (!strcmp (P->tasks[0].cmd, "jobs")) {
        builtin_jobs (P->tasks[0]);
        return;
    }
    else if (!strcmp (P->tasks[0].cmd, "kill")) {
        builtin_kill (P->tasks[0]);
        return;
    }
    
    //pipe and fork
    pid = malloc (P->ntasks * sizeof(*pid));

    in = in_fd (P);

    for (t=0; t<P->ntasks-1; t++) {
        if (pipe (fd))
        {
             fprintf (stderr, "Failed to pipe. \n");
             exit (EXIT_FAILURE);
        }
        pid[t] = fork ();
        if (pid < 0)
        {
           fprintf (stderr, "Failed to fork child. \n");
           exit (EXIT_FAILURE);
        }
        setpgid (pid[t], pid[0]);
       
        if (!pid[t]) {
            close (fd[READ_SIDE]);
            redirect (STDIN_FILENO, in);
            redirect (STDOUT_FILENO, fd[WRITE_SIDE]);
            if (is_builtin (P->tasks[t].cmd))
                builtin_execute (P->tasks[t]);
            else if (P->tasks[t].cmd)
                execvp(P->tasks[t].cmd,&P->tasks[t].argv[0]);  
        }

        close (fd[WRITE_SIDE]);
        close_file (in);

        in = fd[READ_SIDE];
    }
    
    //last task
    out = out_fd(P);

    pid[t] = fork ();
    if (pid < 0)
    {
        fprintf (stderr, "Failed to fork child. \n");
        exit (EXIT_FAILURE);
    }
        
    setpgid (pid[t], pid[0]);
    
    if (!P->background)
        set_fg_pgid (pid[0]);

    if (!pid[t])  {
        redirect (STDIN_FILENO, in);
        redirect (STDOUT_FILENO, out);
         if (is_builtin (P->tasks[t].cmd))
              builtin_execute (P->tasks[t]);
         else if (P->tasks[t].cmd)
              execvp(P->tasks[t].cmd,&P->tasks[t].argv[0]);    
     }

    close_file (in);
    close_file (out);

    job_add (pid, P, job_name);
    /* for ( t= 0 ; t < P-> ntasks ; t ++
            wait (NULL);
            will be done in signal handler*/
}


int main (int argc, char** argv)
{
    char* temp;
    char* cmdline;
    char* job_name;
    Parse* P;
    
    print_banner ();
    job_setup ();
    our_tty = getpgrp ();        //save pgid
    
    signal (SIGCHLD, signal_handler);
    signal (SIGTTIN, signal_handler);
    signal (SIGTTOU, signal_handler);

    while (1) {
        temp = build_prompt ();
        cmdline = readline (temp);
        
        free (temp);

        if (!cmdline)       /* EOF (ex: ctrl-d) */
            exit (EXIT_SUCCESS);
        job_name = strdup (cmdline);
        P = parse_cmdline (cmdline);
        if (!P)
            goto next;

        if (P->invalid_syntax) {
            printf ("pssh: invalid syntax\n");
            fflush (stdout);
            goto next;
        }

#if DEBUG_PARSE
        parse_debug (P);
#endif

        execute_tasks (P, job_name);
        free (job_name);

    next:
        parse_destroy (&P);
        free(cmdline);
    }
}