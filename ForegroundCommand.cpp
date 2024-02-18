#include "ForegroundCommand.h"
#include "SmallShell.h"
#include <cctype>
#include <stdexcept>
#include <algorithm>

void handleForegroundCommand(int jobId, JobsList::JobEntry* currentJob){
    SmallShell& smash = SmallShell::getInstance();
    smash.setLine(currentJob->getJobCmdLine());
    smash.setCurrPid(currentJob->getJobPid());
    DO_SYS(kill(currentJob->getJobPid(),SIGCONT) , kill);
    std::cout << currentJob->getJobCmdLine() <<" "<<currentJob->getJobPid() << std::endl;
    currentJob->setJobStatus(FOREGROUND);
    waitpid (currentJob->getJobPid() , nullptr , WUNTRACED);
    smash.setLine("") ;
    smash.setCurrPid(-1);
    return;
}

bool isNumericString(std::string str){
    int startIndex = str[0] == '-' ? 1 : 0;
    return std::all_of(str.begin() + startIndex, str.end(), ::isdigit);
}

void ForegroundCommand::execute() {
    std::string trimmedCommand = _trim(this->command_line);
    std::string jobNumber = "";
    char* parsedCommand[COMMAND_MAX_ARGS];
    int argCount = _parseCommandLine(this->command_line.c_str(), parsedCommand);
    if (argCount >= 2 && !isNumericString(parsedCommand[1])){
        std::cerr << "smash error: fg: invalid arguments" << std::endl;
        return;
    }
    int jobId;
    JobsList::JobEntry* currentJob;
    if(argCount >= 2){
        jobId = std::stoi(parsedCommand[1]);
        currentJob = this->all_jobs->getJobById(jobId);
        if(!currentJob){
            std::cerr << "smash error: fg: job-id " << jobId << " does not exist" << std::endl;
            return;
        }
    }else{
        jobId = this->all_jobs->maxJobId;
        currentJob = this->all_jobs->getJobById(jobId);
        if (trimmedCommand.find_first_of(" \n") != std::string::npos)
        {
            std::cerr << "smash error: fg: invalid arguments" << std::endl;
            return;
        }
    }
    if(this->all_jobs->allJobs.size() == 0){
        std::cerr << "smash error: fg: jobs list is empty" << std::endl;
        return;
    }

    if (argCount > 2){
        std::cerr << "smash error: fg: invalid arguments" << std::endl;
        return;
    }
    handleForegroundCommand(jobId, currentJob);
}