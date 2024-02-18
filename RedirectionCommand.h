#ifndef COMMANDS_H_REDIRECTIONCOMMAND_H
#define COMMANDS_H_REDIRECTIONCOMMAND_H

#include "Commands.h"

class RedirectionCommand : public Command {
    // TODO: Add your data members
public:
    explicit RedirectionCommand(const char* cmd_line): Command(cmd_line){}
    virtual ~RedirectionCommand() {}
    void execute() override;
    //void prepare() override;
    //void cleanup() override;
};

#endif //COMMANDS_H_REDIRECTIONCOMMAND_H
