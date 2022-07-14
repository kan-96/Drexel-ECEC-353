#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "parse.h"
#include "jobs.h"

#define MAX_JOBS 100

static volatile Job J[MAX_JOBS];


void jobs_init ()
{
    unsigned int i;

    for (i=0; i<MAX_JOBS; i++) {
        J[i].name = NULL;
        J[i].pids = NULL;
        J[i].npids = 0;
        J[i].status = STOPPED;
    }
}


int job_add (pid_t* pids, char* cmdline, Parse* P)
{
    unsigned int i;

    for (i=0; i<MAX_JOBS; i++) {
        if (J[i].name == NULL)
            break;
    }

    if (i == MAX_JOBS)
        return 0;

    J[i].name = strdup (cmdline);
    J[i].pids = pids;
    J[i].npids = P->ntasks;
    J[i].pgrp = pids[0];

    if (P->background)
        J[i].status = BG;
    else
        J[i].status = FG;

    if (J[i].status == BG) {
        printf ("[%u] ", i);
        for (i=0; i<P->ntasks; i++)
            printf ("%i ", pids[i]);

        printf ("\n");
        fflush (stdout);
    }

    return 1;
}


/* get the job number for a given pid, returns -1
 * if not found */
int job_get_number (pid_t pid)
{
    int i, j;

    for (j=0; j<MAX_JOBS; j++)
        for (i=0; i<J[j].npids; i++)
            if (J[j].pids[i] == pid)
                return j;

    return -1;
}


char* job_get_name (unsigned int jnum)
{
    char* name = strdup (J[jnum].name);

    return name;
}


pid_t job_get_pgrp (int jnum)
{
    return J[jnum].pgrp;
}


/* removes a given pid from its associated job */
void job_remove_pid (pid_t pid)
{
    int i;

    int jnum = job_get_number (pid);

    if (jnum < 0)
        return;

    for (i=0; i<J[jnum].npids; i++)
        if (J[jnum].pids[i] == pid)
            J[jnum].pids[i] = 0;
}


/* if all processes comprising the job have been removed,
 * returns 1
 *
 * otherwise, returns 0 */
int job_is_done (int jnum)
{
    unsigned int i;

    for (i=0; i<J[jnum].npids; i++)
        if (J[jnum].pids[i] != 0)
            return 0;

    return 1;
}


void job_delete (int jnum)
{
    J[jnum].npids = 0;
    free (J[jnum].pids);
    free (J[jnum].name);
    J[jnum].name = NULL;
}


int job_exists (int jnum)
{
    if (!J[jnum].name)
        return 0;

    return 1;
}


void job_kill (int jnum, int sig)
{
    if (!job_exists (jnum))
        return;

    kill (-1*J[jnum].pgrp, sig);
}


void job_set_status (int jnum, JobStatus status)
{
    if (!job_exists (jnum))
        return;

    J[jnum].status = status;
}


JobStatus job_status (int jnum)
{
    if (!job_exists (jnum))
        return TERM;

    return J[jnum].status;
}


void job_print (int jnum)
{
    if (!job_exists (jnum))
        return;

    printf ("[%u] + ", jnum);
    if (J[jnum].status == STOPPED)
        printf ("stopped\t");
    else
        printf ("running\t");

    printf ("%s\n", J[jnum].name);
}


void jobs_print ()
{
    int i;

    for (i=0; i<MAX_JOBS; i++)
        if ((J[i].name != NULL) && strcmp (J[i].name, "jobs" ))
            job_print (i);

    fflush (stdout);
}