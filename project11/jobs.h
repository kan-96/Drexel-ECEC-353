#ifndef _jobs_h_
#define _jobs_h_

#include "parse.h"

typedef enum {
    STOPPED,
    TERM,
    BG,
    FG,
} JobStatus;

typedef struct {
    char* name;
    pid_t* pids;
    unsigned int  npids;
    pid_t pgrp;
    JobStatus status;
} Job;

void jobs_init ();
int job_add (pid_t* pids, char* cmdline, Parse* P);
int job_get_number (pid_t pid);
char* job_get_name (unsigned int jnum);
pid_t job_get_pgrp (int jnum);
void job_remove_pid (pid_t pid);
int job_is_done (int jnum);
void job_delete (int jnum);
int job_exists (int jnum);
void job_set_status (int jnum, JobStatus status);
JobStatus job_status ();
void job_kill (int jnum, int sig);
void job_print (int jnum);
void jobs_print (void);

JobStatus job_status (int jnum);

#endif /* _jobs_h_ */