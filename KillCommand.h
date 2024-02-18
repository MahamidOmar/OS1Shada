#ifndef COMMANDS_H_KILLCOMMAND_H
#define COMMANDS_H_KILLCOMMAND_H

#include "BuiltInCommand.h"
#include "JobsList.h"

class KillCommand : public BuiltInCommand {
    // TODO: Add your data members
private:
    JobsList* all_jobs;
public:
    KillCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line),all_jobs(jobs){}
    virtual ~KillCommand() {}
    void execute() override;
};

#endif //COMMANDS_H_KILLCOMMAND_H
