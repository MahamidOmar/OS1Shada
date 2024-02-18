#include "ExternalCommand.h"
#include "SmallShell.h"


void ExternalCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    char* command = (char*)malloc(strlen(this->command_line.c_str())+1);
    strcpy(command, this->command_line.c_str());
    bool isSimpleCommand = this->command_line.find('?') == std::string::npos && this->command_line.find('*') == std::string::npos;
    char *parsed[COMMAND_MAX_ARGS + 1];
    _parseCommandLine(this->command_line.c_str(), parsed);
    if (_isBackgroundComamnd(command)) {
        _removeBackgroundSign(command);
    }
    _parseCommandLine(command, parsed);
    int pid;
    DO_SYS(pid = fork(), fork);
    if (pid == 0) {
        DO_SYS(setpgrp() , setpgrp);
        if (isSimpleCommand){
            if(execvp(parsed[0],parsed) == -1)
            {
                perror("smash error: execvp failed");
                exit(1);
            }
        }else{
            char *argv[] ={(char*)"/bin/bash", (char*)"-c", command, nullptr};
            DO_SYS(execv("/bin/bash", argv), execv);
        }
        exit(0);
    }
    if( _isBackgroundComamnd(this->command_line.c_str()) ){
        smash.getJobList()->removeFinishedJobs();
        smash.getJobList()->addJob(this,pid,BACKGROUND);
    }
    else{
        smash.setLine(this->command_line);
        smash.setCurrPid(pid);
        DO_SYS(waitpid(pid, NULL, WUNTRACED) , waitpid);
        smash.setLine("") ;
        smash.setCurrPid(-1);
        smash.setCurrId(-1);
    }
}