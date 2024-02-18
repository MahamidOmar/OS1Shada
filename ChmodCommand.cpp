#include "ChmodCommand.h"

void ChmodCommand::execute() {
    char* arguments[COMMAND_MAX_ARGS];
    int numOfArgs = _parseCommandLine(this->command_line.c_str(), arguments);
    if(numOfArgs != 3 || atoi(arguments[1]) < 0){
        std::cerr << "smash error: chmod: invalid arguments\n";
        return;
    }
    DO_SYS(chmod(arguments[2], strtol(arguments[1], NULL, 8)), chmod);
}


