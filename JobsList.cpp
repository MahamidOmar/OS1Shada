#include "JobsList.h"
#include "Commands.h"

JobsList::JobEntry::JobEntry(int jobId, int jobPid, string jobCmdLine, jobStatus status)
        : job_id(jobId), job_pid(jobPid), job_command_line(jobCmdLine), job_status(status) {}

void JobsList::addJob(Command *cmd, int jobPid, jobStatus status) {
    removeFinishedJobs();
    vector<shared_ptr<JobEntry>>::iterator jobIter = find_if(allJobs.begin(), allJobs.end(), [jobPid](const shared_ptr<JobEntry>& job) { return job->getJobPid() == jobPid; });

    if (jobIter != allJobs.end()) {
        int prevJobId = (*jobIter)->getJobId();
        allJobs.push_back(make_shared<JobEntry>(prevJobId, jobPid, cmd->getCommandLine(), status));
        removeJobById(prevJobId);
    } else {
        int maxIdForJob = getMaxJobId() + 1;
        setMaxJobId(maxIdForJob);
        allJobs.push_back(make_shared<JobEntry>(maxIdForJob, jobPid, cmd->getCommandLine(), status));
    }
}

void JobsList::printJobsList() {
    this->removeFinishedJobs();
    for (const auto &item: this->allJobs){
        if (item.get()->getJobStatus() == FOREGROUND){
            continue;
        }
        cout << "["<< item.get()->getJobId() << "] " <<item.get()->getJobCmdLine() <<endl;
    }
}

void JobsList::killAllJobs() {
    cout << "smash: sending SIGKILL signal to " << this->allJobs.size() << " jobs:" << endl;
    for (const auto &item: this->allJobs)
    {
        cout << item.get()->getJobPid() << ": " << item.get()->getJobCmdLine() << endl;
        DO_SYS(kill(item.get()->getJobPid(), SIGKILL), kill);
    }
}


void JobsList::removeFinishedJobs() {
    vector<shared_ptr<JobEntry>> still_running;
    for (const auto &item: this->allJobs){
        if (item -> getJobStatus() == FOREGROUND)
            continue;
        int result = 0;
        DO_SYS(result = waitpid(item.get()->getJobPid(),NULL,WNOHANG),waitpid);
        if(result == 0){
            still_running.push_back(item);
        }
    }
    int new_max = 0;
    for (const auto &item: still_running){
        if(item.get()->getJobId() > new_max){
            new_max = item.get()->getJobId();
        }
    }
    this->setMaxJobId(new_max);
    this->allJobs = still_running;
}

JobsList::JobEntry *JobsList::getJobById(int jobId) {
    vector<shared_ptr<JobEntry>>::iterator jobIter = find_if(allJobs.begin(), allJobs.end(), [jobId](const shared_ptr<JobEntry>& job) { return job->getJobId() == jobId; });
    return jobIter != allJobs.end() ? jobIter->get() : nullptr;
}

void JobsList::removeJobById(int jobId) {
    allJobs.erase(remove_if(allJobs.begin(), allJobs.end(), [jobId](const shared_ptr<JobEntry>& job) { return job->getJobId() == jobId; }), allJobs.end());
    setMaxJobId(max_element(allJobs.begin(), allJobs.end(), [](const shared_ptr<JobEntry>& a, const shared_ptr<JobEntry>& b) {
        return a->getJobId() < b->getJobId();
    })->get()->getJobId());
}

JobsList::JobEntry *JobsList::getLastJob(int *lastJobId) {
    *lastJobId = getMaxJobId();
    return getJobById(*lastJobId);
}

JobsList::JobEntry *JobsList::getLastStoppedJob(int *jobId) {
    vector<shared_ptr<JobEntry>>::iterator jobIter = max_element(allJobs.begin(), allJobs.end(), [](const shared_ptr<JobEntry>& a, const shared_ptr<JobEntry>& b) {
        return a->getJobStatus() == STOPPED && a->getJobId() < b->getJobId();
    });
    *jobId = jobIter != allJobs.end() ? jobIter->get()->getJobId() : 0;
    return jobIter != allJobs.end() ? jobIter->get() : nullptr;
}

int JobsList::JobEntry::getJobId() {
    return this->job_id;
}

int JobsList::JobEntry::getJobPid() {
    return this->job_pid;
}

string JobsList::JobEntry::getJobCmdLine() {
    return this->job_command_line;
}

jobStatus JobsList::JobEntry::getJobStatus() {
    return this->job_status;
}

void JobsList::setMaxJobId(int id) {
    this->maxJobId = id;
}

int JobsList::getMaxJobId() {
    return this->maxJobId;
}