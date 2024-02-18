#ifndef COMMANDS_H_EXTERNALCOMMAND_H
#define COMMANDS_H_EXTERNALCOMMAND_H

#include "Commands.h"

class ExternalCommand : public Command {
public:
    ExternalCommand(const char* cmd_line): Command(cmd_line){}
    virtual ~ExternalCommand() {}
    void execute() override;
};

#endif //COMMANDS_H_EXTERNALCOMMAND_H
