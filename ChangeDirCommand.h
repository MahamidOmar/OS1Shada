#ifndef COMMANDS_H_CHANGEDIRCOMMAND_H
#define COMMANDS_H_CHANGEDIRCOMMAND_H

#include "BuiltInCommand.h"

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
public:
    ChangeDirCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
    virtual ~ChangeDirCommand() {}
    void execute() override;
};

#endif //COMMANDS_H_CHANGEDIRCOMMAND_H
