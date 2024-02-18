#ifndef COMMANDS_H_GETCURRDIRCOMMAND_H
#define COMMANDS_H_GETCURRDIRCOMMAND_H

#include "BuiltInCommand.h"

class GetCurrDirCommand : public BuiltInCommand {
public:
    GetCurrDirCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
    virtual ~GetCurrDirCommand() {}
    void execute() override;
};

#endif //COMMANDS_H_GETCURRDIRCOMMAND_H
