#ifndef COMMANDS_H_BUILTINCOMMAND_H
#define COMMANDS_H_BUILTINCOMMAND_H

#include "Commands.h"

class BuiltInCommand : public Command {
public:
    BuiltInCommand(const char* cmd_line): Command(cmd_line){}
    virtual ~BuiltInCommand() {}
};

#endif //COMMANDS_H_BUILTINCOMMAND_H
