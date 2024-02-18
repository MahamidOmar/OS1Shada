#ifndef COMMANDS_H_FOREGROUNDCOMMAND_H
#define COMMANDS_H_FOREGROUNDCOMMAND_H

#include "BuiltInCommand.h"
#include "JobsList.h"

class ForegroundCommand : public BuiltInCommand {
    // TODO: Add your data members
private:
    JobsList* all_jobs;
public:
    ForegroundCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line),all_jobs(jobs){}
    virtual ~ForegroundCommand() {}
    void execute() override;
};

#endif //COMMANDS_H_FOREGROUNDCOMMAND_H
