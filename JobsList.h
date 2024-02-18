#ifndef COMMANDS_H_JOBSLIST_H
#define COMMANDS_H_JOBSLIST_H

#include "Commands.h"


class JobsList {
public:
    class JobEntry {
        int job_id;
        int job_pid;
        string job_command_line;
        jobStatus job_status;
    public:
        JobEntry(int jobId, int jobPid, string JobCmdLine, jobStatus status);
        int getJobId();
        int getJobPid();
        string getJobCmdLine();
        jobStatus getJobStatus();
        void setJobStatus(jobStatus new_status){
            this->job_status = new_status;
        }
        // TODO: Add your data members
    };

    int maxJobId = 0;
    // TODO: Add your data members
public:
    JobsList() = default;
    ~JobsList() = default;
    void addJob(Command* cmd, int jobPid, jobStatus status);
    void printJobsList();
    void killAllJobs();
    void removeFinishedJobs();
    JobEntry * getJobById(int jobId);
    void removeJobById(int jobId);
    JobEntry * getLastJob(int* lastJobId);
    JobEntry *getLastStoppedJob(int *jobId);

    void setMaxJobId(int id);
    int getMaxJobId();
    vector<shared_ptr<JobEntry>> allJobs;
    // TODO: Add extra methods or modify exisitng ones as needed
};

#endif //COMMANDS_H_JOBSLIST_H
