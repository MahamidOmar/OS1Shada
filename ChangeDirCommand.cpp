#include "ChangeDirCommand.h"
#include "SmallShell.h"
#include <unistd.h>
#include <iostream>

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
