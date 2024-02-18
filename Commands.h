#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <string>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include <errno.h>
#include <csignal>
#include <signal.h>
#include <fcntl.h>
#include <memory>
#include <limits.h>
#include <utility>
#include <algorithm>
#include <cstring>
#include <sys/stat.h>

using namespace std;

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

#define DO_SYS(syscall, name) do { \
    if((syscall) == -1){           \
        perror("smash error: " #name " failed"); \
        return;\
    }                               \
}while(0)  \


const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

inline string _ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

inline string _rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

inline string _trim(const std::string& s)
{
    return _rtrim(_ltrim(s));
}

inline int _parseCommandLine(const char* cmd_line, char** args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for(std::string s; iss >> s; ) {
        args[i] = (char*)malloc(s.length()+1);
        memset(args[i], 0, s.length()+1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}

inline bool _isBackgroundComamnd(const char* cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

inline void _removeBackgroundSign(char* cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

using namespace std;



typedef enum {STOPPED , FOREGROUND , BACKGROUND , FINISHED , EMPTY}jobStatus;




class Command {
// TODO: Add your data members

 protected:
    string command_line;

 public:
  Command(const char* cmd_line);
  virtual ~Command() = default;
  virtual void execute() = 0;
  string getCommandLine(){
      return this->command_line;
  }
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

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


class BuiltInCommand : public Command {
public:
    BuiltInCommand(const char* cmd_line): Command(cmd_line){}
    virtual ~BuiltInCommand() {}
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
public:
    ChangeDirCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
    virtual ~ChangeDirCommand() {}
    void execute() override;
};

class ChmodCommand : public BuiltInCommand {
public:
    ChmodCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
    virtual ~ChmodCommand() {}
    void execute() override;
};


class ExternalCommand : public Command {
public:
    ExternalCommand(const char* cmd_line): Command(cmd_line){}
    virtual ~ExternalCommand() {}
    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    // TODO: Add your data members
private:
    JobsList* all_jobs;
public:
    ForegroundCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line),all_jobs(jobs){}
    virtual ~ForegroundCommand() {}
    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
public:
    GetCurrDirCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
    virtual ~GetCurrDirCommand() {}
    void execute() override;
};

class JobsCommand : public BuiltInCommand {
    // TODO: Add your data members
private:
    JobsList* all_jobs;
public:
    JobsCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line),all_jobs(jobs){}
    virtual ~JobsCommand() {}
    void execute() override;
};


class KillCommand : public BuiltInCommand {
    // TODO: Add your data members
private:
    JobsList* all_jobs;
public:
    KillCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line),all_jobs(jobs){}
    virtual ~KillCommand() {}
    void execute() override;
};

class PipeCommand : public Command {
    // TODO: Add your data members
public:
    PipeCommand(const char* cmd_line): Command(cmd_line){}
    virtual ~PipeCommand() {}
    void execute() override;
};

class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
private:
    JobsList* all_jobs;
public:
    QuitCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line),all_jobs(jobs){}
    virtual ~QuitCommand() {}
    void execute() override;
};

class RedirectionCommand : public Command {
    // TODO: Add your data members
public:
    explicit RedirectionCommand(const char* cmd_line): Command(cmd_line){}
    virtual ~RedirectionCommand() {}
    void execute() override;
    //void prepare() override;
    //void cleanup() override;
};

class ShowPidCommand : public BuiltInCommand {
public:
    ShowPidCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
    virtual ~ShowPidCommand() {}
    void execute() override;
};

typedef enum {CHPROMPT,SHOWPID,PWD,CD,JOBS,FG,QUIT,KILL,REDIRECTION,PIPE,CHMOD,NOCOMMAND,EXTERNAL} CommandsType;

class SmallShell {
private:
    // TODO: Add your data members
    SmallShell();
    string currPrompt;
    int curr_pid;
    int curr_id;
    string curr_command_line;
    JobsList* all_jobs;
    int smash_pid;
    string previous_directory;

public:
    Command *CreateCommand(const char* cmd_line);
    SmallShell(SmallShell const&)      = delete; // disable copy ctor
    void operator=(SmallShell const&)  = delete; // disable = operator
    static SmallShell& getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    ~SmallShell();
    void executeCommand(const char* cmd_line);
    void setPrompt(string new_prompt){
        this->currPrompt = new_prompt;
    }

    string getPrompt(){
        return this->currPrompt;
    }

    void setCurrPid(int new_pid){
        this->curr_pid = new_pid;
    }

    int getCurrPid(){
        return this->curr_pid;
    }

    void setLine(string new_line){
        this->curr_command_line = new_line;
    }

    string getCurrLine(){
        return this->curr_command_line;
    }

    void setCurrId(int new_id){
        this->curr_id = new_id;
    }

    int getCurrId(){
        return this->curr_id;
    }

    void setJobList(JobsList* new_list){
        this->all_jobs = new_list;
    }

    JobsList* getJobList(){
        return this->all_jobs;
    }

    int getSmashPid(){
        return this->smash_pid;
    }

    void setPreviousDirectory(string new_directory){
        this->previous_directory = new_directory;
    }

    string getPreviousDirectory(){
        return this->previous_directory;
    }


    // TODO: add extra methods as needed
};




#endif //SMASH_COMMAND_H_
