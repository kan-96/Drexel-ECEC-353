#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "builtin.h"
#include "parse.h"
#include "jobs.h"

static char* builtin[] = {
    "exit",   /* exits the shell */
    "which",  /* displays full path to command */
    "jobs",
    "kill",
    "bg",
    "fg",
    NULL
};


void set_fg_process_group (pid_t pgrp);


int is_builtin (char* cmd)
{
    int i;

    for (i=0; builtin[i]; i++) {
        if (!strcmp (cmd, builtin[i]))
            return 1;
    }

    return 0;
}


void builtin_bg (Task T)
{
    int argc, jnum;

    for (argc=0; T.argv[argc]; argc++);

    if (argc != 2) {
        printf ("Usage: bg %%<job number>\n");
        return;
    }

    jnum = atoi (T.argv[1]+1);

    if (job_exists (atoi (T.argv[1]+1))) {
        /* job_set_status (jnum, BG); */
        job_kill (jnum, SIGCONT);
    } else {
        printf ("pssh: invalid job number: %s\n", T.argv[1]+1);
    }
}


void builtin_fg (Task T)
{
    int argc, jnum;

    for (argc=0; T.argv[argc]; argc++);

    if (argc != 2) {
        printf ("Usage: fg %%<job number>\n");
        return;
    }

    jnum = atoi (T.argv[1]+1);

    if (job_exists (jnum)) {
        job_set_status (jnum, FG);
        set_fg_process_group (job_get_pgrp (jnum));
        job_kill (jnum, SIGCONT);
    } else {
        printf ("pssh: invalid job number: %s\n", T.argv[1]+1);
    }
}


void builtin_jobs (Task T)
{
    jobs_print ();
}


void builtin_kill (Task T)
{
    int argc, ret;

    int i = 1;
    int sig = SIGINT;

    for (argc=0; T.argv[argc]; argc++);

    if (argc < 2) {
        printf ("Usage: kill [-s <signal>] <pid> ...\n");
        return;
    }

    if (!strcmp (T.argv[1], "-s")) {
        sig = atoi (T.argv[2]);
        i=3;
    }

    for (; i<argc; i++) {
        if (*T.argv[i] == '%') {
            if (job_exists (atoi (T.argv[i]+1)))
                job_kill (atoi (T.argv[i]+1), sig);
            else
                printf ("pssh: invalid job number: %s\n", T.argv[i]+1);
        } else {
            ret = kill (atoi (T.argv[i]), sig);
            if (ret < 0)
                printf ("pssh: invalid pid: %s\n", T.argv[i]);
        }
    }
}


int builtin_which (Task T)
{
    char* dir;
    char* tmp;
    char* PATH;
    char* state;
    char probe[PATH_MAX];

    int ret = EXIT_FAILURE;

    if (!T.argv || !T.argv[1])
        exit (EXIT_FAILURE);

    if (access (T.argv[1], F_OK) == 0) {
        printf ("%s\n", T.argv[1]);
        exit (EXIT_SUCCESS);
    }

    if (is_builtin (T.argv[1])) {
        printf ("%s: shell built-in command\n", T.argv[1]);
        exit (EXIT_SUCCESS);
    }

    PATH = strdup (getenv ("PATH"));

    for (tmp=PATH; ; tmp=NULL) {
        dir = strtok_r (tmp, ":", &state);
        if (!dir)
            break;

        strncpy (probe, dir, PATH_MAX);
        strncat (probe, "/", PATH_MAX);
        strncat (probe, T.argv[1], PATH_MAX);

        if (access (probe, X_OK) == 0) {
            printf ("%s\n", probe);
            ret = EXIT_SUCCESS;
            break;
        }
    }

    free (PATH);
    exit (ret);
}


void builtin_execute (Task T)
{
    if (!strcmp (T.cmd, "which")) {
        builtin_which (T);
    }
    else {
        printf ("pssh: builtin command: %s (not implemented!)\n", T.cmd);
    }
}