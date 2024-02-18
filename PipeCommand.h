#ifndef COMMANDS_H_PIPECOMMAND_H
#define COMMANDS_H_PIPECOMMAND_H
#include "Commands.h"

class PipeCommand : public Command {
    // TODO: Add your data members
public:
    PipeCommand(const char* cmd_line): Command(cmd_line){}
    virtual ~PipeCommand() {}
    void execute() override;
};

#endif //COMMANDS_H_PIPECOMMAND_H
