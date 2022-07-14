#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "parse.h"
#include "jobs.h"

#define MAX_JOBS 100

static volatile Job jobs[MAX_JOBS];

void job_setup ()
{
    unsigned int i;

    for (i=0; i<MAX_JOBS; i++) {
        jobs[i].name = NULL;
        jobs[i].pids = NULL;
        jobs[i].npids = 0;
        jobs[i].status = STOPPED;
    }
}

int job_get_num (pid_t pid)
{
    int i,j;
    for ( i= 0; i< MAX_JOBS ; i++ )
        for (j =0; j< jobs[i].npids; j++)
            if (jobs[i].pids[j] == pid)
                return i;
                 
    return -1;
}

int job_add (pid_t* pids, Parse* P, char* cmd)
{
    unsigned int i;
    for(i =0; i< MAX_JOBS; i++)
    {
        if (jobs[i].npids == 0 )
        {
            jobs[i].name = strdup (cmd);
            jobs[i].pids = pids;
            jobs[i].npids = P->ntasks;
            jobs[i].pgid = pids[0];

            if (P->background)
            {
                jobs[i].status = BG;
                printf ("[%u] ", i);
                for (i=0; i<P->ntasks; i++)
                    printf ("%i ", pids[i]);
                printf("\n");
                fflush (stdout);
            }
            else
                jobs[i].status = FG;
            return i;
        }
    }
    return -1;
}

char* job_get_name (int jnum)
{
    char* name;
    name = strdup(jobs[jnum].name);
   
    return name;
}


pid_t job_get_pgid (int jnum)
{
    pid_t pg;
    pg = jobs[jnum].pgid;
    return pg;
}

void terminate_pid (pid_t pid)
{
    unsigned int i;
    int jnum = job_get_num(pid);
    if (jnum <0)
        return;
    for ( i = 0; i < jobs[jnum].npids; i++)
    {
        if (jobs[jnum].pids[i] == pid)
        {
            jobs[jnum].pids[i] = 0;
        }
    }
}

int job_completed (int jnum)
{
    unsigned int i;
    //int e = 1; 

    for (i=0; i<jobs[jnum].npids; i++)
        if (jobs[jnum].pids[i] != 0)  {
            return 0;
        }

    return 1;
}

void job_remove (int jnum)
{
    free(jobs[jnum].name);
    free(jobs[jnum].pids);
    jobs[jnum].npids = 0;
    jobs[jnum].status = TERM;
    jobs[jnum].name = NULL;
}

JobStatus job_get_stt (int jnum)
{
    if (jobs[jnum].name == NULL)
        return TERM;

    return jobs[jnum].status;
}

void job_set_stt (int jnum, JobStatus stt)
{
    if (jobs[jnum].name != NULL)
        jobs[jnum].status = stt;
}

void job_print (int jnum)
{
    if (jobs[jnum].name == NULL)
        return;

   if (jobs[jnum].status == STOPPED)
        printf ("[%u] + stopped \t%s\n" ,jnum,jobs[jnum].name);
    else
        printf("[%u] + running \t%s\n" ,jnum,jobs[jnum].name);
}

void kill_cmd (int jnum, int sig)
{
    if (jobs[jnum].name != NULL)
        kill (-1*jobs[jnum].pgid, sig);
}