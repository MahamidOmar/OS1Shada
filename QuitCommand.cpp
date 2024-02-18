#include "QuitCommand.h"

void QuitCommand::execute() {
    string currentCommandTrimmed = _trim(this->command_line);

    if(currentCommandTrimmed.find(" ") != string::npos){
        string commandParameters = _trim(currentCommandTrimmed.substr(currentCommandTrimmed.find(" ") + 1));
        if(commandParameters == "kill"){
            this->all_jobs->killAllJobs();
        }
    }
    exit(0);
}
