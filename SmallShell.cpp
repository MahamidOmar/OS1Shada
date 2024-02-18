#include "SmallShell.h"
#include "RedirectionCommand.h"
#include "PipeCommand.h"
#include "QuitCommand.h"
#include "GetCurrDirCommand.h"
#include "ChangeDirCommand.h"
#include "JobsCommand.h"
#include "ShowPidCommand.h"
#include "KillCommand.h"
#include "ForegroundCommand.h"
#include "ExternalCommand.h"
#include "ChmodCommand.h"

CommandsType  checkCommandType(string cmd){
    if(cmd == "quit"){
        return QUIT;
    }
    if(cmd == "chprompt"){
        return CHPROMPT;
    }
    if(cmd == "showpid"){
        return SHOWPID;
    }
    if(cmd == "pwd"){
        return PWD;
    }
    if(cmd == "cd"){
        return CD;
    }
    if(cmd == "jobs"){
        return JOBS;
    }
    if(cmd == "kill"){
        return KILL;
    }
    if(cmd == "fg"){
        return FG;
    }
    if(cmd == "chmod"){
        return CHMOD;
    }
    if(cmd == ""){
        return NOCOMMAND;
    }
    return EXTERNAL;
}

void Chprompt(SmallShell* shell,string cmd){
    if (cmd.find_first_of(" \n") == string::npos){
        shell->setPrompt("smash");
        return;
    }
    string commandsParams = _trim(cmd.substr(cmd.find_first_of(" \n")));
    shell->setPrompt(commandsParams.substr(0, commandsParams.find_first_of(" \n")));
}

Command *SmallShell::CreateCommand(const char *cmd_line) {
    string trimmed_command_line = _trim(cmd_line);
    int end_line = trimmed_command_line.find_first_of(" \n");
    string new_command = trimmed_command_line.substr(0,end_line);
    CommandsType curr_type = checkCommandType(new_command);
    if (std::string(cmd_line).find('|') != string::npos)
        curr_type = PIPE;
    if (std::string(cmd_line).find('>') != string::npos)
        curr_type = REDIRECTION;

    switch (curr_type) {
        case CHPROMPT:
            Chprompt(this, trimmed_command_line);
            return nullptr;
        case SHOWPID:
            return new ShowPidCommand(cmd_line);
        case PWD:
            return new GetCurrDirCommand(cmd_line);
        case CD:
            return new ChangeDirCommand(cmd_line);
        case JOBS:
            return new JobsCommand(cmd_line, this->all_jobs);
        case FG:
            return new ForegroundCommand(cmd_line, this->all_jobs);
        case QUIT:
            return new QuitCommand(cmd_line, this->all_jobs);
        case KILL:
            return new KillCommand(cmd_line, this->all_jobs);
        case EXTERNAL:
            return new ExternalCommand(cmd_line);
        case REDIRECTION:
            return new RedirectionCommand(cmd_line);
            break;
        case PIPE:
            return new PipeCommand(cmd_line);
        case CHMOD:
            return new ChmodCommand(cmd_line);
        default:
            return nullptr;
    }
}

SmallShell::SmallShell() {
    this->currPrompt = "smash";
    this->curr_pid = -1;
    this->curr_id = -1;
    this->curr_command_line = "";
    this->all_jobs = new JobsList();
    this->previous_directory = "";
    DO_SYS(this->smash_pid = getpid(), getpid);
}

SmallShell::~SmallShell() {
    delete this->all_jobs;
}

void SmallShell::executeCommand(const char* cmd_line)
{
    this->all_jobs->removeFinishedJobs();
    Command* command_to_execute = CreateCommand(cmd_line);
    if(command_to_execute != nullptr)
    {
        command_to_execute->execute();
    }
}

