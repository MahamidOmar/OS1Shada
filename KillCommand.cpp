#include "KillCommand.h"
#include "SmallShell.h"

bool isNumber(const string& str){
    return !str.empty() &&
           (str[0] == '-' ? std::all_of(str.begin() + 1, str.end(), ::isdigit) :
            std::all_of(str.begin(), str.end(), ::isdigit));
}

void KillCommand::execute() {
    char* parsed_command[COMMAND_MAX_ARGS];
    int num_of_args = _parseCommandLine(this->command_line.c_str(), parsed_command);
    if (num_of_args<3){
        cerr <<"smash error: kill: invalid arguments"<<endl;
        return;
    }
    if (isNumber(parsed_command[2]) == 0){
        cerr <<"smash error: kill: invalid arguments"<<endl;
        return;
    }
    if (!this->all_jobs->getJobById(stoi(parsed_command[2]))){
        cerr << "smash error: kill: job-id "<<stoi(parsed_command[2])<< " does not exist"<<endl;
        return;
    }
    if(num_of_args >3){
        cerr <<"smash error: kill: invalid arguments"<<endl;
        return;
    }
    if(isNumber(parsed_command[1]) == 0 || parsed_command[1][0] != '-' ){
        cerr << "smash error: kill: invalid arguments" << endl;
        return;
    }

    int signal = abs(stoi(parsed_command[1]));
    if(signal > 31 || signal < 0){
        cerr << "smash error: kill failed: Invalid argument" << endl;
        return;
    }
    int job_id = stoi(parsed_command[2]);
    JobsList::JobEntry* jobEntry = this->all_jobs->getJobById(job_id);
    int job_pid = jobEntry->getJobPid();
    DO_SYS(kill(job_pid, signal), kill);
    if(signal == SIGSTOP){
        jobEntry->setJobStatus(STOPPED);
    }
    if(signal == SIGCONT){
        jobEntry->setJobStatus(BACKGROUND);
    }
    std::cout << "signal number " << signal << " was sent to pid " << jobEntry -> getJobPid() << std::endl;
}
