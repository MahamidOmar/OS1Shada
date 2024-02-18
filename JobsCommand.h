
#ifndef COMMANDS_H_JOBSCOMMAND_H
#define COMMANDS_H_JOBSCOMMAND_H

#include "BuiltInCommand.h"
#include "JobsList.h"

class JobsCommand : public BuiltInCommand {
    // TODO: Add your data members
private:
    JobsList* all_jobs;
public:
    JobsCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line),all_jobs(jobs){}
    virtual ~JobsCommand() {}
    void execute() override;
};

#endif //COMMANDS_H_JOBSCOMMAND_H
