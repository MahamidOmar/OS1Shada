#include "PipeCommand.h"
#include "SmallShell.h"

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

