#include "RedirectionCommand.h"
#include "SmallShell.h"

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
