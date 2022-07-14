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
    pid_t pgid;
    JobStatus status;
} Job;

void job_setup ();
int job_get_num (pid_t pid);
int job_add (pid_t* pids, Parse* , char* cmd);
char* job_get_name (int jnum);
pid_t job_get_pgid (int jnum);
void terminate_pid (pid_t pid);
int job_completed (int jnum);
void job_remove(int jnum);
JobStatus job_get_stt (int jnum);
void job_set_stt (int jnum, JobStatus stt);
void job_print (int jnum);
void kill_cmd (int jnum, int sig);

#endif