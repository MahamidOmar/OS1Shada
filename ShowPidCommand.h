#ifndef COMMANDS_H_SHOWPIDCOMMAND_H
#define COMMANDS_H_SHOWPIDCOMMAND_H

#include "BuiltInCommand.h"


class ShowPidCommand : public BuiltInCommand {
public:
    ShowPidCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
    virtual ~ShowPidCommand() {}
    void execute() override;
};

#endif //COMMANDS_H_SHOWPIDCOMMAND_H
