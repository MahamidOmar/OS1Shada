#include "Commands.h"

Command::Command(const char *cmd_line) {
    command_line=cmd_line;
}

bool isChangeDirCommandValid(int commandWords){
    if(commandWords == 1){
        std::cerr << "smash error:>\"cd\"";
        return false;
    }
    if(commandWords > 2){
        std::cerr << "smash error: cd: too many arguments" << std::endl;
        return false;
    }
    return true;
}

void ChangeDirCommand::execute() {
    SmallShell &shell = SmallShell::getInstance();
    char* parsedCommand[COMMAND_ARGS_MAX_LENGTH + 3];
    int commandWords = _parseCommandLine(this->command_line.c_str(), parsedCommand);
    if(!isChangeDirCommandValid(commandWords)){
        return;
    }
    std::string newDirectory;
    if(std::string(parsedCommand[1]) != "-"){
        newDirectory = std::string(parsedCommand[1]);
    }else{
        if(!shell.getPreviousDirectory().empty()){
            newDirectory = shell.getPreviousDirectory();
        }
        else{
            std::cerr << "smash error: cd: OLDPWD not set" << std::endl;
            return;
        }
    }

    char currentDirectory[PATH_MAX];
    if(!getcwd(currentDirectory, sizeof(currentDirectory))){
        perror("smash error: getcwd failed");
    }
    int changeDirResult;
    DO_SYS(changeDirResult = chdir(newDirectory.c_str()), chdir);
    if(changeDirResult != -1){
        shell.setPreviousDirectory(currentDirectory);
    }
}

void ChmodCommand::execute() {
    char* arguments[COMMAND_MAX_ARGS];
    int numOfArgs = _parseCommandLine(this->command_line.c_str(), arguments);
    if(numOfArgs != 3 || atoi(arguments[1]) < 0){
        std::cerr << "smash error: chmod: invalid arguments\n";
        return;
    }
    DO_SYS(chmod(arguments[2], strtol(arguments[1], NULL, 8)), chmod);
}

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

void GetCurrDirCommand::execute() {
    char current_directory[PATH_MAX];
    char* syscall_result = getcwd(current_directory, sizeof(current_directory));
    if(syscall_result == NULL){
        perror("smash error: getcwd failed");
        return;
    }
    cout << current_directory << endl;
}


void JobsCommand::execute() {
    this->all_jobs->printJobsList();
}

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


void executeInChildProcess(const string& command, int newFd, int oldFd, int pipeCommands[2]) {
    SmallShell& shell = SmallShell::getInstance();
    setpgrp();
    DO_SYS(dup2(pipeCommands[oldFd], newFd), dup2);
    DO_SYS(close(pipeCommands[0]), close);
    DO_SYS(close(pipeCommands[1]), close);
    shell.executeCommand(command.c_str());
    exit(0);
}

void pipeHelper(const string& leftCommand, const string& rightCommand, int numFd) {
    pid_t leftPid, rightPid;
    int pipeCommands[2];
    DO_SYS(pipe(pipeCommands), pipe);
    DO_SYS(leftPid = fork(), fork);
    if (leftPid == 0) {
        executeInChildProcess(leftCommand, numFd, 1, pipeCommands);
    }
    DO_SYS(rightPid = fork(), fork);
    if (rightPid == 0) {
        executeInChildProcess(rightCommand, STDIN_FILENO, 0, pipeCommands);
    }
    DO_SYS(close(pipeCommands[0]), close);
    DO_SYS(close(pipeCommands[1]), close);
    DO_SYS(waitpid(leftPid, NULL, 0), waitpid);
    DO_SYS(waitpid(rightPid, NULL, 0), waitpid);
}

void PipeCommand::execute() {
    string currCommandLine = _trim(this->command_line);
    string leftCommand = _trim(currCommandLine.substr(0, currCommandLine.find_first_of("|")));
    if (currCommandLine.find("|&") != string::npos) {
        pipeHelper(leftCommand, currCommandLine.substr(currCommandLine.find_first_of("|&") + 2), STDERR_FILENO);
    } else {
        pipeHelper(leftCommand, currCommandLine.substr(currCommandLine.find_first_of('|') + 1), STDOUT_FILENO);
    }
}

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

void handleRedirection(const string& command, const string& direction, int flag) {
    SmallShell& smash = SmallShell::getInstance();
    int fdOut;
    DO_SYS(fdOut = dup(STDOUT_FILENO), dup);
    DO_SYS(close(STDOUT_FILENO), close);
    int fd;
    DO_SYS(fd = open(direction.c_str(), flag | O_WRONLY | O_CREAT ,0655), open);
    smash.executeCommand(command.c_str());

    if (fd == -1) {
        perror("smash error: open failed");
    } else if (fd != STDOUT_FILENO) {
        cout << "you have open the wrong fd" << endl;
    }

    if (fd != -1) {
        DO_SYS(close(fd), close);
    }
    DO_SYS(dup2(fdOut, STDOUT_FILENO), dup2);
    DO_SYS(close(fdOut), close);
}

void RedirectionCommand::execute() {
    string trimmed = _trim(this->command_line);
    string command = _trim(trimmed.substr(0 , trimmed.find_first_of('>')));

    if(trimmed.find(">>") != string::npos) {
        handleRedirection(command, _trim(trimmed.substr(trimmed.find_first_of(">>") + 2)), O_APPEND);
    } else {
        handleRedirection(command, _trim(trimmed.substr(trimmed.find_first_of(">") + 1)), O_TRUNC);
    }
}


void ShowPidCommand::execute(){
    SmallShell& shell = SmallShell::getInstance();
    cout << "smash pid is " << shell.getSmashPid() << endl;
}
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


