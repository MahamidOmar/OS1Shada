#ifndef COMMANDS_H_SMALLSHELL_H
#define COMMANDS_H_SMALLSHELL_H

 #include "Commands.h"
#include "JobsList.h"


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

#endif //COMMANDS_H_SMALLSHELL_H
